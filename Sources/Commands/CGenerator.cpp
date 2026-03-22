#include "CGenerator.h"

std::string val::CGenerator::indent(size_t tabs) const noexcept
{
    return std::string(tabs, '\t');
}

void val::CGenerator::AddHeaders(std::ostream& to) noexcept
{
    to << R"(#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#define bool _Bool
#define uint uint32_t

typedef void (*_MVS_Deleter)(uintptr_t);

typedef struct HeapMemory
{
    size_t ref_count;
    size_t size_in_bytes;
    _MVS_Deleter deleter;
} HeapMemory;

typedef struct MVSNode
{
    uintptr_t key;
    HeapMemory value;
    struct MVSNode* next;
} MVSNode;

typedef struct
{
    MVSNode** buckets;
    size_t capacity;
    size_t size;
} MVSHashMap;

static MVSHashMap mu;

static size_t _hash_address(uintptr_t key_address, size_t capacity)
{
    return (key_address * 2654435761u) % capacity;
}

static void _resize()
{
    if ((double)mu.size / mu.capacity < 0.75) { return; }

    size_t new_capacity = mu.capacity * 2;
    MVSNode** new_buckets = calloc(new_capacity, sizeof(MVSNode*));

    for (size_t i = 0; i < mu.capacity; i++)
    {
        MVSNode* current = mu.buckets[i];

        while (current)
        {
            MVSNode* next = current->next;

            size_t new_index = _hash_address(current->key, new_capacity);

            current->next = new_buckets[new_index];
            new_buckets[new_index] = current;

            current = next;
        }
    }

    free(mu.buckets);
    mu.buckets = new_buckets;
    mu.capacity = new_capacity;
}

void MVS_Init()
{
    mu.capacity = 16;
    mu.size = 0;
    mu.buckets = calloc(mu.capacity, sizeof(MVSNode*));
}

void MVS_Destroy()
{
    for (size_t i = 0; i < mu.capacity; i++)
    {
        MVSNode* current = mu.buckets[i];

        while (current)
        {
            MVSNode* next = current->next;
            free(current);
            current = next;
        }
    }

    free(mu.buckets);
}

static MVSNode* _find_node(uintptr_t address)
{
    size_t index = _hash_address(address, mu.capacity);
    MVSNode* current = mu.buckets[index];

    while (current)
    {
        if (current->key == address)
            return current;

        current = current->next;
    }

    return NULL;
}

size_t MVS_RefCount(uintptr_t address)
{
    MVSNode* node = _find_node(address);

    if (!node) { return 0; }

    return node->value.ref_count;
}

void MVS_RegisterNew(uintptr_t address, size_t size, _MVS_Deleter deleter)
{
    MVSNode* node = _find_node(address);

    if (node)
    {
        node->value.ref_count++;
        return;
    }

    _resize();

    size_t index = _hash_address(address, mu.capacity);

    MVSNode* new_node = malloc(sizeof(MVSNode));
    new_node->key = address;
    new_node->value.ref_count = 1;
    new_node->value.size_in_bytes = size;
    new_node->value.deleter = deleter;

    new_node->next = mu.buckets[index];
    mu.buckets[index] = new_node;

    mu.size++;
}

bool MVS_DetachPointer(uintptr_t address) {
    size_t index = _hash_address(address, mu.capacity);
    MVSNode* current = mu.buckets[index];
    MVSNode* prev = NULL;

    while (current) {
        if (current->key == address) {
            if (current->value.ref_count > 1) {
                current->value.ref_count--;
                return 0;
            }

            if (prev) { prev->next = current->next; }
            else { mu.buckets[index] = current->next; }

            current->value.deleter(address);
            free(current);
            mu.size--;
            return 1;
        }
        prev = current;
        current = current->next;
    }
    return 0;
}

bool MVS_SameLoc(uintptr_t a, uintptr_t b)
{
    return a == b;
}

typedef struct array
{
    size_t length;
    size_t elem_size;
    bool is_pointer;
    _MVS_Deleter element_deleter;
    void* data;
} array;

void xx_free_array(uintptr_t address)
{
    array* arr = (array*)address;

    if (arr->data)
    {
        if (arr->is_pointer)
        {
            void** ptrs = (void**)arr->data;

            for (size_t i = 0; i < arr->length; i++)
            {
                if (ptrs[i])
                    MVS_DetachPointer((uintptr_t)ptrs[i]);
            }
        }

        free(arr->data);
    }

    free(arr);
}

array* xx_init_array(const void* data,
    size_t length,
    size_t elem_size,
    bool is_pointer,
    _MVS_Deleter element_deleter)
{
    array* arr = malloc(sizeof(array));

    arr->length = length;
    arr->elem_size = elem_size;
    arr->is_pointer = is_pointer;
    arr->element_deleter = element_deleter;

    arr->data = malloc(length * elem_size);

    if (data)
        memcpy(arr->data, data, length * elem_size);

    if (is_pointer && data)
    {
        void** ptrs = (void**)arr->data;

        for (size_t i = 0; i < length; i++)
        {
            if (ptrs[i])
                MVS_RegisterNew((uintptr_t)ptrs[i], 0, element_deleter);
        }
    }

    return arr;
}

void xx_array_set(array* arr, size_t index, void* value)
{
    if (!arr) return;
    if (index >= arr->length) return;

    char* base = (char*)arr->data + index * arr->elem_size;

    if (arr->is_pointer)
    {
        void* old_ptr = *(void**)base;

        if (old_ptr)
            MVS_DetachPointer((uintptr_t)old_ptr);

        void* new_ptr = *(void**)value;

        if (new_ptr)
            MVS_RegisterNew((uintptr_t)new_ptr, 0, arr->element_deleter);
    }

    memcpy(base, value, arr->elem_size);
}

void* xx_array_get(array* arr, size_t index)
{
    if (!arr) return NULL;
    if (index >= arr->length) return NULL;

    return (char*)arr->data + index * arr->elem_size;
}

array* xx_array_add(array* a, bool a_temp,
    array* b, bool b_temp)
{
    if (!a || !b) return NULL;
    if (a->elem_size != b->elem_size) return NULL;
    if (a->is_pointer != b->is_pointer) return NULL;

    array* result = malloc(sizeof(array));

    result->elem_size = a->elem_size;
    result->is_pointer = a->is_pointer;
    result->length = a->length + b->length;

    size_t bytes = result->length * result->elem_size;

    result->data = malloc(bytes);

    memcpy(result->data,
        a->data,
        a->length * a->elem_size);

    memcpy((char*)result->data + a->length * a->elem_size,
        b->data,
        b->length * b->elem_size);

    if (result->is_pointer)
    {
        void** ptrs = (void**)result->data;

        for (size_t i = 0; i < result->length; i++)
        {
            if (ptrs[i])
                MVS_RegisterNew((uintptr_t)ptrs[i], 0, a->element_deleter);
        }
    }

    if (a_temp)
        xx_free_array((uintptr_t)a);

    if (b_temp)
        xx_free_array((uintptr_t)b);

    return result;
}

array* xx_array_add_elem(array* a, bool a_temp, void* elem)
{
    if (!a) return NULL;

    array* result = malloc(sizeof(array));

    result->length = a->length + 1;
    result->elem_size = a->elem_size;
    result->is_pointer = a->is_pointer;

    result->data = malloc(result->length * result->elem_size);

    memcpy(result->data,
        a->data,
        a->length * a->elem_size);

    memcpy((char*)result->data + a->length * a->elem_size,
        elem,
        a->elem_size);

    if (result->is_pointer)
    {
        void* ptr = *(void**)elem;

        if (ptr)
            MVS_RegisterNew((uintptr_t)ptr, 0, a->element_deleter);
    }

    if (a_temp)
        xx_free_array((uintptr_t)a);

    return result;
}

array* xx_array_sub_right(array* arr, bool arr_temp, size_t n)
{
    if (!arr) return NULL;

    array* result;

    if (n >= arr->length)
    {
        result = xx_init_array(NULL, 0, arr->elem_size, arr->is_pointer, arr->element_deleter);
    }
    else
    {
        size_t new_len = arr->length - n;

        result = malloc(sizeof(array));

        result->length = new_len;
        result->elem_size = arr->elem_size;
        result->is_pointer = arr->is_pointer;

        result->data = malloc(new_len * result->elem_size);

        memcpy(result->data,
            arr->data,
            new_len * result->elem_size);

        if (result->is_pointer)
        {
            void** ptrs = (void**)result->data;

            for (size_t i = 0; i < new_len; i++)
            {
                if (ptrs[i])
                    MVS_RegisterNew((uintptr_t)ptrs[i], 0, arr->element_deleter);
            }
        }
    }

    if (arr_temp)
        xx_free_array((uintptr_t)arr);

    return result;
}

array* xx_array_sub_left(size_t n, array* arr, bool arr_temp)
{
    if (!arr) return NULL;

    array* result;

    if (n >= arr->length)
    {
        result = xx_init_array(NULL, 0, arr->elem_size, arr->is_pointer, arr->element_deleter);
    }
    else
    {
        size_t new_len = arr->length - n;

        result = malloc(sizeof(array));

        result->length = new_len;
        result->elem_size = arr->elem_size;
        result->is_pointer = arr->is_pointer;

        result->data = malloc(new_len * result->elem_size);

        memcpy(result->data,
            (char*)arr->data + n * arr->elem_size,
            new_len * result->elem_size);

        if (result->is_pointer)
        {
            void** ptrs = (void**)result->data;

            for (size_t i = 0; i < new_len; i++)
            {
                if (ptrs[i])
                    MVS_RegisterNew((uintptr_t)ptrs[i], 0, arr->element_deleter);
            }
        }
    }

    if (arr_temp)
        xx_free_array((uintptr_t)arr);

    return result;
}

array* xx_clone_array(array* ptr)
{
    if (!ptr)
        return NULL;

    array* clone = malloc(sizeof(array));

    clone->length = ptr->length;
    clone->elem_size = ptr->elem_size;
    clone->is_pointer = ptr->is_pointer;

    size_t bytes = ptr->length * ptr->elem_size;

    clone->data = malloc(bytes);

    memcpy(clone->data, ptr->data, bytes);

    if (clone->is_pointer)
    {
        void** ptrs = (void**)clone->data;

        for (size_t i = 0; i < clone->length; i++)
        {
            if (ptrs[i])
                MVS_RegisterNew((uintptr_t)ptrs[i], 0, ptr->element_deleter);
        }
    }

    MVS_RegisterNew((uintptr_t)clone, sizeof(array), xx_free_array);

    return clone;
}

typedef struct string {
    int length;
    char* data;
} string;

void xx_free_string(uintptr_t address)
{
    string* s = (string*)address;

    if (s->data != NULL)
        free(s->data);

    free(s);
}

void xx_string_set(string* s, size_t index, char value)
{
    if (!s) return;
    if (index >= s->length) return;

    s->data[index] = value;
}

string* xx_init_string(const char* literal)
{
    string* s = malloc(sizeof(string));

    s->length = strlen(literal);
    s->data = malloc(s->length + 1);

    memcpy(s->data, literal, s->length + 1);

    return s;
}

string* xx_string_add(string* a, bool a_temp,
    string* b, bool b_temp)
{
    if (!a || !b) return NULL;

    string* result = malloc(sizeof(string));

    result->length = a->length + b->length;
    result->data = malloc(result->length + 1);

    memcpy(result->data, a->data, a->length);
    memcpy(result->data + a->length, b->data, b->length);

    result->data[result->length] = '\0';

    if (a_temp)
        xx_free_string((uintptr_t)a);

    if (b_temp)
        xx_free_string((uintptr_t)b);

    return result;
}

string* xx_string_add_char(string* a, bool a_temp, char c)
{
    if (!a) return NULL;

    string* result = malloc(sizeof(string));

    result->length = a->length + 1;
    result->data = malloc(result->length + 1);

    memcpy(result->data, a->data, a->length);

    result->data[a->length] = c;
    result->data[result->length] = '\0';

    if (a_temp)
        xx_free_string((uintptr_t)a);

    return result;
}

string* xx_string_sub_right(string* s, bool s_temp, size_t n)
{
    if (!s) return NULL;

    string* result;

    if (n >= s->length)
    {
        result = xx_init_string("");
    }
    else
    {
        size_t new_len = s->length - n;

        result = malloc(sizeof(string));
        result->length = new_len;
        result->data = malloc(new_len + 1);

        memcpy(result->data, s->data, new_len);
        result->data[new_len] = '\0';
    }

    if (s_temp)
        xx_free_string((uintptr_t)s);

    return result;
}

string* xx_string_sub_left(size_t n, string* s, bool s_temp)
{
    if (!s) return NULL;

    string* result;

    if (n >= s->length)
    {
        result = xx_init_string("");
    }
    else
    {
        size_t new_len = s->length - n;

        result = malloc(sizeof(string));
        result->length = new_len;
        result->data = malloc(new_len + 1);

        memcpy(result->data, s->data + n, new_len);
        result->data[new_len] = '\0';
    }

    if (s_temp)
        xx_free_string((uintptr_t)s);

    return result;
}

string* xx_clone_string(string* ptr)
{
    if (ptr == NULL)
        return NULL;

    string* clone = malloc(sizeof(string));

    clone->length = ptr->length;
    clone->data = malloc(ptr->length + 1);

    memcpy(clone->data, ptr->data, ptr->length + 1);

    MVS_RegisterNew((uintptr_t)clone, sizeof(string), xx_free_string);

    return clone;
}

)";
}

std::string val::CGenerator::ToString(const Expression& expr) noexcept
{
    switch (expr.sel())
    {
    case selector::EmptyExpr:
        return "NULL";
    case selector::IntLiteralExpr:
        return std::to_string(expr.view_IntLiteral().value());
    case selector::DoubleLiteralExpr:
        return std::to_string(expr.view_DoubleLiteral().value());
    case selector::StringLiteralExpr:
        return expr.view_StringLiteral().value();
    case selector::BoolLiteralExpr:
        return expr.view_BoolLiteral().value() ? "1" : "0";
    case selector::CharLiteralExpr:
        return "'" + std::string(1, expr.view_CharLiteral().value()) + "'";
    case selector::VarNameExpr:
        return expr.view_VarName().name();
    case selector::FieldCallExpr:
        return EvalExpression(expr).str_expr;
    case selector::StructInitExpr:
    {
        std::string s = "xx_init_" + expr.view_StructInit().struct_name() + "(";
        for (size_t i = 0; i < expr.view_StructInit().size(); i++)
        {
            auto ini = EvalExpression(expr.view_StructInit().inits(i));
            if (i > 0) { s += ", "; }

            if (ini.type == ExprType::String)
            {
                s += "xx_init_string(" + ini.str_expr + ")";
            }
            else if (ini.type == ExprType::Property && opt_to_prop.contains(ini.type_name))
            {
                s += "xx_init_" + opt_to_prop.at(ini.type_name) + "_" + ini.type_name + "(" + ini.str_expr + ")";
            }
            else {
                s += ini.str_expr;
            }
        }
        return s + ")";
    }
    case selector::FnCallExpr:
    {
        const std::string& fn_name = expr.view_FnCall().fn_name();

        if (fn_name == "len")
        {
            return EvalExpression(expr.view_FnCall().args(0)).str_expr + "->length";
        }

        std::string expr_str = fn_name + "(";

        if (fn_name == "printf" || fn_name == "scanf")
        {
            expr_str += EvalExpression(expr.view_FnCall().args(0)).str_expr;

            for (size_t i = 1; i < expr.view_FnCall().size(); i++)
            {
                expr_str += ", ";
                auto next_expr = EvalExpression(expr.view_FnCall().args(i));

                expr_str += next_expr.str_expr;
                if (next_expr.type_name == "string")
                {
                    expr_str += "->data";
                }
            }

            return expr_str + ')';
        }

        for (size_t i = 0; i < expr.view_FnCall().size(); i++)
        {
            if (i > 0) { expr_str += ", "; }
            expr_str += EvalExpression(expr.view_FnCall().args(i)).str_expr;
        }

        return expr_str + ')';
    }
    case selector::ArrayIndexExpr:
    {
        return EvalArrIndex(expr).str_expr;
    }
    case selector::InitListExpr:
    {
        return EvalExpression(expr).str_expr;
    }
    case selector::BinaryExpr:
    {
        auto left = EvalExpression(expr.view_Binary().lhs());
        auto right = EvalExpression(expr.view_Binary().rhs());

        if (expr.view_Binary().op() == "+")
        {
            if (left.type == ExprType::String && right.type == ExprType::String)
            {
                return "xx_string_add(" + left.str_expr + ", " + (left.is_lvalue ? "0, " : "1, ") +
                    right.str_expr + ", " + (right.is_lvalue ? "0)" : "1)");
            }
            if (left.type == ExprType::String && right.type == ExprType::Primitive)
            {
                return "xx_string_add_char(" + left.str_expr + ", " + (left.is_lvalue ? "0, " : "1, ") +
                    right.str_expr + ")";
            }
            if (IsExprTypeArray(left.type) && IsExprTypeArray(right.type))
            {
                return "xx_array_add(" + left.str_expr + ", " + (left.is_lvalue ? "0, " : "1, ") +
                    right.str_expr + ", " + (right.is_lvalue ? "0)" : "1)");
            }
            if (IsExprTypeArray(left.type) && not IsExprTypeArray(right.type))
            {
                return "xx_array_add_elem(" + left.str_expr + ", " + (left.is_lvalue ? "0, " : "1, ") +
                    right.str_expr + ")";
            }
        }
        if (expr.view_Binary().op() == "-")
        {
            if (left.type == ExprType::String && right.type == ExprType::Primitive)
            {
                return "xx_string_sub_right(" + left.str_expr + ", " + (left.is_lvalue ? "0, " : "1, ") +
                    right.str_expr + ")";
            }
            if (left.type == ExprType::Primitive && right.type == ExprType::String)
            {
                return "xx_string_sub_left(" + left.str_expr + ", " + right.str_expr + ", " + (right.is_lvalue ? "0)" : "1)");
            }
            if (IsExprTypeArray(left.type) && right.type == ExprType::Primitive)
            {
                return "xx_array_sub_right(" + left.str_expr + ", " + (left.is_lvalue ? "0, " : "1, ") +
                    right.str_expr + ")";
            }
            if (left.type == ExprType::Primitive && IsExprTypeArray(right.type))
            {
                return "xx_array_sub_left(" + left.str_expr + ", " + right.str_expr + ", " + (right.is_lvalue ? "0)" : "1)");
            }
        }

        return left.str_expr + expr.view_Binary().op() + right.str_expr;
    }
    default:
        return "???";
    }
}

void val::CGenerator::GenVarInit(const Statement& var_init_stmt, std::ostream& to, size_t tabs, std::vector<std::string>& allocs) noexcept
{
    const auto& view = var_init_stmt.view_VarInit();

    if (TypeIsPrimitive(view.type_name()) || TypeIsEnum(view.type_name()))
    {
        to << indent(tabs) << view.type_name() << ' ' << view.var_name();
        if (not view.init_expr().option_is_EmptyLiteral())
        {
            to << " = ";
            auto expr = EvalExpression(view.init_expr());
            to << expr.str_expr;
        }
        to << ";\n";
    }
    else if (TypeIsStruct(view.type_name()))
    {
        allocs.push_back(view.var_name());
        to << indent(tabs) << view.type_name() << "* " << view.var_name() << " = ";
        if (not view.init_expr().option_is_EmptyLiteral())
        {
            auto expr = EvalExpression(view.init_expr());
            to << expr.str_expr << ";\n";
            if (not view.init_expr().option_is_FnCall())
            {
                to << indent(tabs)
                    << "MVS_RegisterNew((uintptr_t)" << view.var_name()
                    << ", sizeof(" << view.type_name() << "), xx_free_"
                    << view.type_name() << ");\n";
            }
        }
        else
        {
            to << "NULL;\n";
        }
    }
    else if (TypeIsProperty(view.type_name()))
    {
        allocs.push_back(view.var_name());
        to << indent(tabs) << view.type_name() << "* " << view.var_name() << " = ";
        if (view.init_expr().option_is_StructInit())
        {
            to << "xx_init_" << view.type_name() << "_" << view.init_expr().view_StructInit().struct_name() << '(';
            auto expr = EvalExpression(view.init_expr());
            to << expr.str_expr << ");\n";
            if (not view.init_expr().option_is_FnCall())
            {
                to << indent(tabs)
                    << "MVS_RegisterNew((uintptr_t)" << view.var_name()
                    << ", sizeof(" << view.type_name() << "), xx_free_"
                    << view.type_name() << ");\n";
            }
        }
        else if (not view.init_expr().option_is_EmptyLiteral())
        {
            auto expr = EvalExpression(view.init_expr());
            to << expr.str_expr << ";\n";
            //if (not view.init_expr().option_is_FnCall())
            //{
            to << indent(tabs)
                << "MVS_RegisterNew((uintptr_t)" << view.var_name()
                << ", sizeof(" << view.type_name() << "), xx_free_"
                << view.type_name() << ");\n";
            //}
        }
        else {
            to << "NULL;\n";
        }
    }
    else {
        allocs.push_back(view.var_name());
        to << indent(tabs) << "string* " << view.var_name() << " = ";
        if (not view.init_expr().option_is_EmptyLiteral())
        {
            auto expr = EvalExpression(view.init_expr());
            to << "xx_init_string(" << expr.str_expr << ");\n";
        }
        else {
            to << "xx_init_string(\"\");\n";
        }
        //if (not view.init_expr().option_is_FnCall())
        //{
        to << indent(tabs) << "MVS_RegisterNew((uintptr_t)" << view.var_name()
            << ", sizeof(string), xx_free_string);\n";
        //}
    }
}

void val::CGenerator::GenArrInit(const Statement& arr_init_stmt, std::ostream& to, size_t tabs, std::vector<std::string>& allocs) noexcept
{
    const auto& view = arr_init_stmt.view_ArrayInit();
    
    to << indent(tabs) << "array* " << view.type_info().view_VarInit().var_name();
    if (not view.init_expr().option_is_EmptyLiteral())
    {
        to << " = " << EvalExpression(view.init_expr()).str_expr << ";\n";
        to << indent(tabs) << "MVS_RegisterNew((uintptr_t)" << view.type_info().view_VarInit().var_name()
            << ", sizeof(array), xx_free_array);\n";
    }
    else {
        to << " = NULL;";
    }

    allocs.push_back(view.type_info().view_VarInit().var_name());
}

void val::CGenerator::GenReturn(const Statement& return_stmt, std::ostream& to, size_t tabs) noexcept
{
    const auto& view = return_stmt.view_Return();
    to << indent(tabs) << "return";

    if (not view.return_expr().option_is_EmptyLiteral())
    {
        to << " ";
        auto expr = EvalExpression(view.return_expr());
        to << expr.str_expr;
    }

    to << ";\n";
}

void val::CGenerator::GenFnBlock(const Statement& fn_block, std::ostream& to, size_t tabs) noexcept
{
    to << indent(tabs) << "/* Skipping Fn Body */\n";
}

void val::CGenerator::GenMakeFn(const Statement& make_fn, std::ostream& to, size_t tabs) noexcept
{
    const auto& view = make_fn.view_MakeFunction();

    to << indent(tabs) << view.ret_type_name() << " " << view.fn_name() << "(";
    for (size_t i = 0; i < view.size(); i++)
    {
        const auto& view_param = view.params(i).view_FnArgs();

        if (i > 0) { to << ", "; }

        if (view_param.is_array())
        {
            to << "array*";
        }
        else {
            to << view_param.type_name();
        }

        if (view_param.is_inout())
        {
            to << "*";
        }

        to << " " << view_param.var_name();
    }
    to << ")\n";

    GenBlock(view.fn_body(), to, tabs);
}

void val::CGenerator::GenStructDecl(const Statement& struct_stmt, std::ostream& to, size_t tabs) noexcept
{
    const auto& view = struct_stmt.view_MakeStruct();
    to << indent(tabs) << "typedef struct " << view.struct_name();

    if (view.size() == 0)
    {
        to << ' ' << view.struct_name() << ";\n\n";
        return;
    }

    to << " {\n";

    for (size_t i = 0; i < view.size(); i++)
    {
        if (view.inits(i).option_is_VarInit())
        {
            
            if (
                _IsPrimitive(view.struct_name(), view.inits(i).view_VarInit().var_name()) ||
                _IsEnumType(view.struct_name(), view.inits(i).view_VarInit().var_name())
            )
            {
                to << indent(tabs + 1) << view.inits(i).view_VarInit().type_name() << " " << view.inits(i).view_VarInit().var_name() << ";\n";
            }
            else if (
                _IsStructType(view.struct_name(), view.inits(i).view_VarInit().var_name()) ||
                _IsProperty(view.struct_name(), view.inits(i).view_VarInit().var_name())
            )
            {
                if (view.inits(i).view_VarInit().type_name() == view.struct_name())
                {
                    to << indent(tabs + 1) << "struct " << view.struct_name() << "* " << view.inits(i).view_VarInit().var_name() << ";\n";
                }
                else {
                    to << indent(tabs + 1) << view.inits(i).view_VarInit().type_name() << "* " << view.inits(i).view_VarInit().var_name() << ";\n";
                }
            }
            else
            {
                to << indent(tabs + 1) << "string* " << view.inits(i).view_VarInit().var_name() << ";\n";
            }
        }
        else {
            to << indent(tabs + 1) << "array* " << view.inits(i).view_ArrayInit().type_info().view_VarInit().var_name() << ";\n";
        }
    }

    to << indent(tabs + 1) << "} " << view.struct_name() << ";\n\n";
}

void val::CGenerator::GenStructDel(const Statement& struct_stmt, std::ostream& to, size_t tabs) noexcept
{
    const auto& view = struct_stmt.view_MakeStruct();

    to << indent(tabs) << "void xx_free_" << view.struct_name() << "(uintptr_t address)\n";
    to << indent(tabs) << "{\n";

    std::string deleter_pointer = "((" + view.struct_name() + "*)address)";

    for (size_t i = 0; i < view.size(); i++)
    {
        std::string name = view.inits(i).option_is_VarInit() ?
            view.inits(i).view_VarInit().var_name() : view.inits(i).view_ArrayInit().type_info().view_VarInit().var_name();
        if (not _IsPrimitive(view.struct_name(), name) && not _IsEnumType(view.struct_name(), name))
        {
            to << indent(tabs + 1) << "if (" << deleter_pointer << "->" << name << " != NULL)\n";
            to << indent(tabs + 1) << "{\n";
            to << indent(tabs + 2) << "MVS_DetachPointer((uintptr_t)" << deleter_pointer << "->" << name << ");\n";
            to << indent(tabs + 1) << "}\n";
        }
    }

    if (view.size() != 0) 
    {
        to << indent(tabs + 1) << "free(" << deleter_pointer << ");\n";
    }
    to << indent(tabs) << "}\n\n";
}

void val::CGenerator::GenStructInit(const Statement& struct_stmt, std::ostream& to, size_t tabs) noexcept
{
    const auto& view = struct_stmt.view_MakeStruct();
    to << indent(tabs) << view.struct_name() << "* xx_init_" << view.struct_name() << "(";

    if (view.size() == 0)
    {
        to << ")\n" << indent(tabs) << "{\n";
        to << indent(tabs + 1) << "return NULL;\n";
        to << indent(tabs) << "}\n\n";
        return;
    }

    int i = 0;

    auto S = std::get <StructType>(type_table.at(view.struct_name()));
    for (const auto& [field_name, var_kind] : S.fields)
    {
        if (i > 0)
        {
            to << ", ";
        }

        if (_IsPrimitive(view.struct_name(), field_name) || _IsEnumType(view.struct_name(), field_name))
        {
            to << std::get <ObjectKind>(var_kind).type_name << " " << field_name;
        }
        else if (_IsStructType(view.struct_name(), field_name) || _IsProperty(view.struct_name(), field_name))
        {
            to << std::get <ObjectKind>(var_kind).type_name << "* " << field_name;
        }
        else if (_IsString(view.struct_name(), field_name))
        {
            to << "string* " << field_name;
        }
        else {
            to << "array* " << field_name;
        }

        i++;
    }

    to << ") \n";
    to << indent(tabs) << "{\n";

    to << indent(tabs + 1) << view.struct_name() << " *xx_" << view.struct_name()
        << "_init_ptr = malloc(sizeof(" << view.struct_name() << "));\n";

    for (const auto& [field_name, var_kind] : S.fields)
    {
        if (_IsPrimitive(view.struct_name(), field_name) || _IsEnumType(view.struct_name(), field_name))
        {
            to << indent(tabs + 1) << "xx_" << view.struct_name() << "_init_ptr->" <<
                field_name << " = " << field_name << ";\n";
        }
        else if (_IsStructType(view.struct_name(), field_name) || _IsProperty(view.struct_name(), field_name) || _IsString(view.struct_name(), field_name))
        {
            to << indent(tabs + 1) << "xx_" << view.struct_name() << "_init_ptr->" <<
                field_name << " = " << field_name << ";\n";
            to << indent(tabs + 1) << "if (" << field_name << " != NULL) {\n";
            to << indent(tabs + 2) << "MVS_RegisterNew((uintptr_t)"
                << field_name << ", sizeof(" << std::get <ObjectKind>(var_kind).type_name << "), xx_free_"
                << std::get <ObjectKind>(var_kind).type_name << ");\n";
            to << indent(tabs + 1) << "}\n";
        }
        else {
            to << indent(tabs + 1) << "xx_" << view.struct_name() << "_init_ptr->" <<
                field_name << " = " << field_name << ";\n";
            to << indent(tabs + 2) << "if (" << field_name << " != NULL) {\n";
            to << indent(tabs + 2)
                << "MVS_RegisterNew((uintptr_t)" << field_name << ", sizeof(array), xx_free_array);\n";
            to << indent(tabs + 1) << "}\n";
        }
    }

    to << indent(tabs + 1) << "return xx_" << view.struct_name() << "_init_ptr;\n";
    to << indent(tabs) << "}\n\n";
}

void val::CGenerator::GenStructClone(const Statement& struct_stmt, std::ostream& to, size_t tabs) noexcept
{
    const auto& view = struct_stmt.view_MakeStruct();
    to << indent(tabs) << view.struct_name() << "* " << "xx_clone_" << view.struct_name() << "(" << view.struct_name() << "* ptr) \n";
    to << indent(tabs) << "{\n";

    to << indent(tabs + 1) << "if (ptr == NULL) { return NULL; }\n";
    to << indent(tabs + 1) << view.struct_name() << "* clone = xx_init_" << view.struct_name() << "(";

    auto S = std::get <StructType>(type_table.at(view.struct_name()));
    int i = 0;
    for (const auto& [field_name, var_kind] : S.fields)
    {
        if (i > 0) { to << ", "; }
        if (_IsPrimitive(view.struct_name(), field_name) || _IsEnumType(view.struct_name(), field_name))
        {
            to << "ptr->" << field_name;
        }
        else if (_IsStructType(view.struct_name(), field_name) || _IsProperty(view.struct_name(), field_name))
        {
            to << "xx_clone_" << std::get <ObjectKind>(var_kind).type_name << "(ptr->" << field_name << ")";
        }
        else if (_IsString(view.struct_name(), field_name))
        {
            to << "xx_clone_string(ptr->" << field_name << ")";
        }
        else {
            to << "xx_clone_array(ptr->" << field_name << ")";
        }
        i++;
    }

    to << ");\n";
    if (not std::get <StructType>(type_table.at(view.struct_name())).fields.empty()) {
        to << indent(tabs + 1) << "MVS_RegisterNew((uintptr_t)clone, sizeof(" << view.struct_name() << "), xx_free_" << view.struct_name() << ");\n";
    }
    to << indent(tabs + 1) << "return clone;\n";
    to << indent(tabs) << "}\n\n";
}

void val::CGenerator::GenMakeStruct(const Statement& struct_stmt, std::ostream& to, size_t tabs) noexcept
{
    GenStructDecl(struct_stmt, to, tabs);
    GenStructDel(struct_stmt, to, tabs);
    GenStructInit(struct_stmt, to, tabs);
    GenStructClone(struct_stmt, to, tabs);
}

void val::CGenerator::GenFwDeclProp(const Statement& prop_stmt, std::ostream& to, size_t tabs) noexcept
{
    const std::string& prop_name = prop_stmt.view_MakeProperty().prop_name();
    to << indent(tabs) << "typedef struct " << prop_name << " " << prop_name << ";\n";
    to << indent(tabs) << "void xx_free_" << prop_name << "(uintptr_t address);\n\n";
}

void val::CGenerator::GenTagEnumProp(const Statement& prop_stmt, std::ostream& to, size_t tabs) noexcept
{
    const auto& view = prop_stmt.view_MakeProperty();
    to << indent(tabs) << "typedef enum " << view.prop_name() << "_Tag {\n";
    to << indent(tabs + 1);
    for (size_t i = 0; i < view.size(); i++)
    {
        to << view.opts(i).view_MakeStruct().struct_name() << "_tag, ";
    }
    to << '\n' << indent(tabs) << "} " << view.prop_name() << "_Tag;\n\n";
}

void val::CGenerator::GenOptionProp(const Statement& prop_stmt, std::ostream& to, size_t tabs) noexcept
{
    const auto& view = prop_stmt.view_MakeProperty();
    to << indent(tabs) << "typedef union " << view.prop_name() << "_Opts {\n";

    for (size_t i = 0; i < view.size(); i++)
    {
        to << indent(tabs + 1) << view.opts(i).view_MakeStruct().struct_name() << "* " << view.opts(i).view_MakeStruct().struct_name() << "_opt;\n";
    }

    to << indent(tabs) << "} " << view.prop_name() << "_Opts;\n\n";
}

void val::CGenerator::GenPropDecl(const Statement& prop_stmt, std::ostream& to, size_t tabs) noexcept
{
    const auto& view = prop_stmt.view_MakeProperty();
    to << indent(tabs) << "typedef struct " << view.prop_name() << " {\n";
    to << indent(tabs + 1) << view.prop_name() << "_Tag tag;\n";
    to << indent(tabs + 1) << view.prop_name() << "_Opts opts;\n";

    to << indent(tabs) << "} " << view.prop_name() << ";\n\n";
}

void val::CGenerator::GenPropDel(const Statement& prop_stmt, std::ostream& to, size_t tabs) noexcept
{
    const auto& view = prop_stmt.view_MakeProperty();
    to << indent(tabs) << "void xx_free_" << view.prop_name() << "(uintptr_t address)\n";
    to << indent(tabs) << "{\n";

    std::string deleter_pointer = "((" + view.prop_name() + "*)address)";

    for (size_t i = 0; i < view.size(); i++)
    {
        to << indent(tabs + 1) <<
            "if ((" << deleter_pointer << "->tag == " << view.opts(i).view_MakeStruct().struct_name() << "_tag) && (" <<
            deleter_pointer << "->opts." << view.opts(i).view_MakeStruct().struct_name() << "_opt != NULL)) \n";
        to << indent(tabs + 1) << "{\n";
        to << indent(tabs + 2) << "MVS_DetachPointer((uintptr_t)" << deleter_pointer << "->opts." << view.opts(i).view_MakeStruct().struct_name() << "_opt);\n";
        to << indent(tabs + 1) << "}\n";
    }
    to << indent(tabs + 1) << "free(" << deleter_pointer << ");\n";
    to << indent(tabs) << "}\n\n";
}

void val::CGenerator::GenPropInit(const std::string& prop_name, const std::string& opt_name, std::ostream& to, size_t tabs) noexcept
{
    to << indent(tabs) << prop_name << "* xx_init_" << prop_name << "_" << opt_name <<
        '(' << opt_name << "* " << opt_name << "_opt)\n";
    to << indent(tabs) << "{\n";

    to << indent(tabs + 1) << prop_name << "* xx_init_" << prop_name << "_ptr = malloc(sizeof(" << prop_name << "));\n";
    to << indent(tabs + 1) << "xx_init_" << prop_name << "_ptr->tag = " << opt_name << "_tag;\n";
    to << indent(tabs + 1) << "xx_init_" << prop_name << "_ptr->opts." << opt_name << "_opt = " << opt_name << "_opt;\n";

    to << indent(tabs + 1) << "if (" << opt_name << "_opt != NULL)\n";
    to << indent(tabs + 1) << "{\n";
    to << indent(tabs + 2) << "MVS_RegisterNew((uintptr_t)"
        << opt_name << "_opt, sizeof(" << opt_name << "), xx_free_" << opt_name << ");\n";
    to << indent(tabs + 1) << "}\n";
    to << indent(tabs + 1) << "return xx_init_" << prop_name << "_ptr;\n";
    to << indent(tabs) << "}\n\n";
}

void val::CGenerator::GenPropNullInit(const std::string& prop_name, const std::string& opt_name, std::ostream& to, size_t tabs) noexcept
{
    to << indent(tabs) << prop_name << "* xx_init_" << prop_name << "_" << opt_name <<
        '(' << opt_name << "* " << opt_name << "_opt)\n";
    to << indent(tabs) << "{\n";

    to << indent(tabs + 1) << prop_name << "* xx_init_" << prop_name << "_ptr = malloc(sizeof(" << prop_name << "));\n";
    to << indent(tabs + 1) << "xx_init_" << prop_name << "_ptr->tag = " << opt_name << "_tag;\n";
    to << indent(tabs + 1) << "xx_init_" << prop_name << "_ptr->opts." << opt_name << "_opt = " << opt_name << "_opt;\n";

    to << indent(tabs + 1) << "return xx_init_" << prop_name << "_ptr;\n";
    to << indent(tabs) << "}\n\n";
}

void val::CGenerator::GenPropClone(const Statement& prop_stmt, std::ostream& to, size_t tabs) noexcept
{
    const auto& view = prop_stmt.view_MakeProperty();

    to << indent(tabs) << view.prop_name() << "* " << "xx_clone_" << view.prop_name() << "(" << view.prop_name() << "* ptr) \n";
    to << indent(tabs) << "{\n";
    to << indent(tabs + 1) << "if (ptr == NULL) { return NULL; }\n";
    to << indent(tabs + 1) << view.prop_name() << "* clone = malloc(sizeof(" << view.prop_name() << "));\n";
    to << indent(tabs + 1) << "clone->tag = ptr->tag;\n";
    to << indent(tabs + 1) << "switch (ptr->tag)\n";
    to << indent(tabs + 1) << "{\n";

    for (size_t i = 0; i < view.size(); i++)
    {
        std::string opt_name = view.opts(i).view_MakeStruct().struct_name();
        to << indent(tabs + 1) << "case " << opt_name << "_tag:\n";
        to << indent(tabs + 2) << "clone->opts." << opt_name << "_opt = xx_clone_" << opt_name
            << "(ptr->opts." << opt_name << "_opt);\n";
        to << indent(tabs + 2) << "break;\n";
    }

    to << indent(tabs + 1) << "}\n";
    to << indent(tabs + 1) << "MVS_RegisterNew((uintptr_t)clone, sizeof(" << view.prop_name() << "), xx_free_" <<
        view.prop_name() << ");\n";
    to << indent(tabs + 1) << "return clone;\n";
    to << indent(tabs) << "}\n\n";
}

void val::CGenerator::GenMakeProperty(const Statement& property_stmt, std::ostream& to, size_t tabs) noexcept
{
    const auto& view = property_stmt.view_MakeProperty();

    GenFwDeclProp(property_stmt, to, tabs);

    for (size_t i = 0; i < view.size(); i++)
    {
        GenStructDecl(view.opts(i), to, tabs);
        opt_to_prop.insert({ view.opts(i).view_MakeStruct().struct_name(), view.prop_name() });
    }

    GenTagEnumProp(property_stmt, to, tabs);
    GenOptionProp(property_stmt, to, tabs);
    GenPropDecl(property_stmt, to, tabs);

    auto OptionCheckCreate = [&to, &view, &tabs](const std::string& IR_opt_name, size_t i) {
        to << std::string(tabs, '\t') << "bool xx_" << view.prop_name() << "_option_is_" << IR_opt_name << "(const " << view.prop_name() << "* prop)\n";
        to << std::string(tabs, '\t') << "{\n";
        to << std::string(tabs + 1, '\t') << "return prop->tag == " << view.opts(i).view_MakeStruct().struct_name() << "_tag;\n";
        to << std::string(tabs, '\t') << "}\n\n";
    };

    for (size_t i = 0; i < view.size(); i++)
    {
        OptionCheckCreate(view.opts(i).view_MakeStruct().struct_name(), i);
    }

    GenPropDel(property_stmt, to, tabs);

    for (size_t i = 0; i < view.size(); i++)
    {
        GenStructDel(view.opts(i), to, tabs);
        GenStructInit(view.opts(i), to, tabs);
        GenStructClone(view.opts(i), to, tabs);
    }

    for (size_t i = 0; i < view.size(); i++)
    {
        if (view.opts(i).view_MakeStruct().size() == 0)
        {
            GenPropNullInit(view.prop_name(), view.opts(i).view_MakeStruct().struct_name(), to, tabs);
        }
        else {
            GenPropInit(view.prop_name(), view.opts(i).view_MakeStruct().struct_name(), to, tabs);
        }
    }

    GenPropClone(property_stmt, to, tabs);
}

void val::CGenerator::GenMakeEnum(const Statement& enum_stmt, std::ostream& to, size_t tabs) noexcept
{
    const auto& view = enum_stmt.view_MakeEnum();

    to << indent(tabs) << "typedef enum " << view.enum_name() << " {\n";

    for (size_t i = 0; i < view.size(); i++)
    {
        to << indent(tabs + 1) << view.enum_variants(i) << ", ";
    }
    to << '\n' << indent(tabs) << "} " << view.enum_name() << ";\n";
}

void val::CGenerator::GenCaseProperty(const Statement& case_stmt, const std::string& prop_type, const std::string& prop_expr_name, size_t order, std::ostream& to, size_t tabs) noexcept
{
    const auto& view = case_stmt.view_CaseClause();

    if (order == 0)
    {
        to << indent(tabs) << "if (";
    }
    else if (order > 0)
    {
        to << indent(tabs) << "else if (";
    }

    if (view.is_wildcard())
    {
        if (order > 0) {
            to << "1)\n";
        }
    }
    else {
        cur_prop_name = prop_type;
        cur_option_name = view.case_expr().view_VarName().name();
        to << "xx_" << prop_type << "_option_is_" << view.case_expr().view_VarName().name() << "(" << prop_expr_name << "))\n";
    }

    GenBlock(view.case_body(), to, tabs);
}

void val::CGenerator::GenCaseEnum(const Statement& case_stmt, const std::string& enum_expr, size_t order, std::ostream& to, size_t tabs) noexcept
{
    const auto& view = case_stmt.view_CaseClause();

    if (order == 0)
    {
        to << indent(tabs) << "if (";
    }
    else if (order > 0)
    {
        to << indent(tabs) << "else if (";
    }

    if (view.is_wildcard())
    {
        if (order > 0) {
            to << "1)\n";
        }
    }
    else {
        to << enum_expr << " == " << view.case_expr().view_VarName().name() << ")\n";
    }
    GenBlock(view.case_body(), to, tabs);
}

void val::CGenerator::GenMatch(const Statement& match_stmt, std::ostream& to, size_t tabs) noexcept
{
    const auto& view = match_stmt.view_Match();
    auto match_expr_info = EvalExpression(view.matched_expr());

    in_match = true;

    for (size_t i = 0; i < view.size(); i++)
    {
        if (match_expr_info.type == ExprType::Property) {
            cur_match_expr = match_expr_info.str_expr;
            GenCaseProperty(view.cases(i), match_expr_info.type_name, match_expr_info.str_expr, i, to, tabs);
        }
        else {
            GenCaseEnum(view.cases(i), match_expr_info.str_expr, i, to, tabs);
        }
    }

    in_match = false;
}

void val::CGenerator::GenFor(const Statement& for_stmt, std::ostream& to, size_t tabs) noexcept
{
    const auto& view = for_stmt.view_ForLoop();

    to << indent(tabs) << "{\n";

    std::vector <std::string> allocated;

    for (size_t i = 0; i < view.init_part().view_Block().size(); i++)
    {
        if (view.init_part().view_Block().statements(i).option_is_ArrayInit())
        {
            GenArrInit(view.init_part().view_Block().statements(i), to, tabs + 1, allocated);
        }
        else
        {
            GenVarInit(view.init_part().view_Block().statements(i), to, tabs + 1, allocated);
        }
    }

    to << indent(tabs + 1) << "while(1)\n";
    to << indent(tabs + 1) << "{\n";

    to << indent(tabs + 2) << "if (!(";
    auto for_expr = EvalExpression(view.check());
    to << for_expr.str_expr;
    to << ")) { break; }\n";

    std::ostringstream increment_block;
    GenBlock(view.final_expr(), increment_block, tabs + 2);

    GenBlock(view.forloop_body(), to, tabs + 2, allocated);
    to << increment_block.str();

    to << indent(tabs + 1) << "}\n";
    to << indent(tabs) << "}\n";
}

void val::CGenerator::GenWhile(const Statement& while_stmt, std::ostream& to, size_t tabs) noexcept
{
    const auto& view = while_stmt.view_WhileLoop();

    to << indent(tabs) << "while (";
    auto while_expr = EvalExpression(view.cond());
    to << while_expr.str_expr;
    to << ")\n";

    GenBlock(view.whileloop_body(), to, tabs);
}

void val::CGenerator::GenCond(const Statement& cond_stmt, std::ostream& to, size_t tabs) noexcept
{
    const auto& view = cond_stmt.view_Condition();

    to << indent(tabs) << "if (";
    auto if_expr = EvalExpression(view.if_cond());
    to << if_expr.str_expr;
    to << ")\n";

    GenBlock(view.if_body(), to, tabs);

    for (size_t i = 0; i < view.size(); i++)
    {
        to << indent(tabs) << "else if (";
        auto elif_expr = EvalExpression(view.elif_stmt(i).view_ElifCondition().elif_cond());
        to << elif_expr.str_expr;
        to << ")\n";

        GenBlock(view.elif_stmt(i).view_ElifCondition().elif_body(), to, tabs);
    }

    if (not view.else_body().option_is_Empty())
    {
        to << indent(tabs) << "else\n";
        GenBlock(view.else_body(), to, tabs);
    }
}

void val::CGenerator::GenBlock(const Statement& block_stmt, std::ostream& to, size_t tabs, std::vector<std::string> allocs) noexcept
{
    const auto& view = block_stmt.view_Block();
    to << indent(tabs) << "{\n";

    std::vector <std::string> alloc_ptrs;
    alloc_ptrs.reserve(allocs.size());
    for (size_t i = 0; i < allocs.size(); i++)
    {
        alloc_ptrs.push_back(allocs[i]);
    }

    for (size_t i = 0; i < view.size(); i++)
    {
        switch (view.statements(i).sel())
        {
        case selector::VarInitStmt:
            GenVarInit(view.statements(i), to, tabs + 1, alloc_ptrs);
            break;
        case selector::ArrayInitStmt:
            GenArrInit(view.statements(i), to, tabs + 1, alloc_ptrs);
            break;
        case selector::ConditionStmt:
            GenCond(view.statements(i), to, tabs + 1);
            break;
        case selector::WhileLoopStmt:
            GenWhile(view.statements(i), to, tabs + 1);
            break;
        case selector::ReturnStmt:
        {
            GenCleanup(alloc_ptrs, to, tabs + 1);
            GenReturn(view.statements(i), to, tabs + 1);
            to << indent(tabs) << "}\n";
            return;
        }
        case selector::BreakStmt:
            to << indent(tabs + 1) << "break;\n";
            break;
        case selector::ContinueStmt:
            to << indent(tabs + 1) << "continue;\n";
            break;
        case selector::MatchStmt:
            GenMatch(view.statements(i), to, tabs + 1);
            break;
        case selector::ForLoopStmt:
            GenFor(view.statements(i), to, tabs + 1);
            break;
        case selector::AssignmentStmt:
            GenAssign(view.statements(i), to, tabs + 1);
            break;
        case selector::BlockOfStmt:
            GenBlock(view.statements(i), to, tabs + 1);
            break;
        case selector::ExprCallStmt:
            GenExprCall(view.statements(i), to, tabs + 1);
            break;
        }
    }

    GenCleanup(alloc_ptrs, to, tabs + 1);
    to << indent(tabs) << "}\n";
}

void val::CGenerator::GenCleanup(std::vector<std::string>& allocs, std::ostream& to, size_t tabs) noexcept
{
    for (const auto& ptr_name : allocs)
    {
        to << indent(tabs) << "MVS_DetachPointer((uintptr_t)" << ptr_name << ");\n";
    }
}

val::CGenerator::CGenerator(std::unordered_map<std::string, VariableKind>&& symbol_table, std::unordered_map<std::string, TypeClass>&& type_table, std::unordered_map<std::string, FnTable>&& fn_table)
    : symbol_table(std::move(symbol_table)), type_table(std::move(type_table)), fn_table(std::move(fn_table))
{
}

std::filesystem::path val::CGenerator::GenerateC_IR(const Statement& valid_AST, const std::string& val_source_path)
{
    std::filesystem::path c_ir_path = std::filesystem::path(val_source_path).replace_extension(".c");
    std::ofstream out(c_ir_path);

    if (not out.is_open())
    {
        throw std::runtime_error("Failed to Open File " + c_ir_path.string());
    }

    AddHeaders(out);

    const auto& view = valid_AST.view_Block();

    for (size_t i = 0; i < view.size(); i++)
    {
        switch (view.statements(i).sel())
        {
        case selector::MakeFunctionStmt:
            GenMakeFn(view.statements(i), out, 0);
            break;
        case selector::MakeStructStmt:
            GenMakeStruct(view.statements(i), out, 0);
            break;
        case selector::MakePropertyStmt:
            GenMakeProperty(view.statements(i), out, 0);
            break;
        case selector::MakeEnumStmt:
            GenMakeEnum(view.statements(i), out, 0);
            break;
        default:
            break;
        }
    }

    out << "\n\nint main()\n{\n";
    out << "\tMVS_Init();\n";

    std::vector <std::string> alloc_ptrs;

    for (size_t i = 0; i < view.size(); i++)
    {
        switch (view.statements(i).sel())
        {
        case selector::VarInitStmt:
            GenVarInit(view.statements(i), out, 1, alloc_ptrs);
            break;
        case selector::ArrayInitStmt:
            GenArrInit(view.statements(i), out, 1, alloc_ptrs);
            break;
        case selector::ConditionStmt:
            GenCond(view.statements(i), out, 1);
            break;
        case selector::WhileLoopStmt:
            GenWhile(view.statements(i), out, 1);
            break;
        case selector::MatchStmt:
            GenMatch(view.statements(i), out, 1);
            break;
        case selector::ForLoopStmt:
            GenFor(view.statements(i), out, 1);
            break;
        case selector::AssignmentStmt:
            GenAssign(view.statements(i), out, 1);
            break;
        case selector::BlockOfStmt:
            GenBlock(view.statements(i), out, 1);
            break;
        case selector::ExprCallStmt:
            GenExprCall(view.statements(i), out, 1);
            break;
        }
    }

    GenCleanup(alloc_ptrs, out, 1);
    out << "\tMVS_Destroy();\n";
    out << "}\n";

    out.close();

    return c_ir_path;
}

bool val::CGenerator::TypeIsPrimitive(const std::string& type_name) const noexcept
{
    return type_name == "uint" || type_name == "int" || type_name == "char" || type_name == "double" || type_name == "bool";
}

bool val::CGenerator::TypeIsStruct(const std::string& type_name) const noexcept
{
    auto it = type_table.find(type_name);
    return it != type_table.end() && std::holds_alternative<StructType>(it->second);
}

bool val::CGenerator::TypeIsProperty(const std::string& type_name) const noexcept
{
    auto it = type_table.find(type_name);
    return it != type_table.end() && std::holds_alternative<PropertyType>(it->second);
}

bool val::CGenerator::TypeIsEnum(const std::string& type_name) const noexcept
{
    auto it = type_table.find(type_name);
    return it != type_table.end() && std::holds_alternative<EnumType>(it->second);
}

bool val::CGenerator::TypeIsString(const std::string& type_name) const noexcept
{
    return type_name == "string";
}

bool val::CGenerator::_IsPrimitive(const std::string& var_name) const noexcept
{
    auto it = symbol_table.find(var_name);
    if (it == symbol_table.end()) return false;

    auto obj = std::get_if<ObjectKind>(&it->second);
    if (!obj) return false;

    const auto& t = obj->type_name;
    return t == "int" || t == "uint" || t == "bool" || t == "double" || t == "char";
}

bool val::CGenerator::_IsPrimitive(const std::string& struct_name, const std::string& field_name) const noexcept
{
    auto it = type_table.find(struct_name);
    if (it == type_table.end()) return false;

    auto st = std::get_if<StructType>(&it->second);
    if (!st) return false;

    auto fit = st->fields.find(field_name);
    if (fit == st->fields.end()) return false;

    auto obj = std::get_if<ObjectKind>(&fit->second);
    if (!obj) return false;

    const auto& t = obj->type_name;
    return t == "int" || t == "uint" || t == "bool" || t == "double" || t == "char";
}

bool val::CGenerator::_IsString(const std::string& var_name) const noexcept
{
    auto it = symbol_table.find(var_name);
    if (it == symbol_table.end()) return false;

    auto obj = std::get_if<ObjectKind>(&it->second);
    return obj && obj->type_name == "string";
}

bool val::CGenerator::_IsString(const std::string& struct_name, const std::string& field_name) const noexcept
{
    auto it = type_table.find(struct_name);
    if (it == type_table.end()) return false;

    auto st = std::get_if<StructType>(&it->second);
    if (!st) return false;

    auto fit = st->fields.find(field_name);
    if (fit == st->fields.end()) return false;

    auto obj = std::get_if<ObjectKind>(&fit->second);
    return obj && obj->type_name == "string";
}

bool val::CGenerator::_IsStructType(const std::string& var_name) const noexcept
{
    auto it = symbol_table.find(var_name);
    if (it == symbol_table.end()) return false;

    auto obj = std::get_if<ObjectKind>(&it->second);
    if (!obj) return false;

    auto tit = type_table.find(obj->type_name);
    return tit != type_table.end() && std::holds_alternative<StructType>(tit->second);
}

bool val::CGenerator::_IsStructType(const std::string& struct_name, const std::string& field_name) const noexcept
{
    auto it = type_table.find(struct_name);
    if (it == type_table.end()) return false;

    auto st = std::get_if<StructType>(&it->second);
    if (!st) return false;

    auto fit = st->fields.find(field_name);
    if (fit == st->fields.end()) return false;

    auto obj = std::get_if<ObjectKind>(&fit->second);
    if (!obj) return false;

    auto tit = type_table.find(obj->type_name);
    return tit != type_table.end() && std::holds_alternative<StructType>(tit->second);
}

bool val::CGenerator::_IsEnumType(const std::string& var_name) const noexcept
{
    auto it = symbol_table.find(var_name);
    if (it == symbol_table.end()) return false;

    auto obj = std::get_if<ObjectKind>(&it->second);
    if (!obj) return false;

    auto tit = type_table.find(obj->type_name);
    return tit != type_table.end() && std::holds_alternative<EnumType>(tit->second);
}

bool val::CGenerator::_IsEnumType(const std::string& struct_name, const std::string& field_name) const noexcept
{
    auto it = type_table.find(struct_name);
    if (it == type_table.end()) return false;

    auto st = std::get_if<StructType>(&it->second);
    if (!st) return false;

    auto fit = st->fields.find(field_name);
    if (fit == st->fields.end()) return false;

    auto obj = std::get_if<ObjectKind>(&fit->second);
    if (!obj) return false;

    auto tit = type_table.find(obj->type_name);
    return tit != type_table.end() && std::holds_alternative<EnumType>(tit->second);
}

bool val::CGenerator::_IsProperty(const std::string& var_name) const noexcept
{
    auto it = symbol_table.find(var_name);
    if (it == symbol_table.end()) return false;

    auto obj = std::get_if<ObjectKind>(&it->second);
    if (!obj) return false;

    auto tit = type_table.find(obj->type_name);
    return tit != type_table.end() && std::holds_alternative<PropertyType>(tit->second);
}

bool val::CGenerator::_IsProperty(const std::string& struct_name, const std::string& field_name) const noexcept
{
    auto it = type_table.find(struct_name);
    if (it == type_table.end()) return false;

    auto st = std::get_if<StructType>(&it->second);
    if (!st) return false;

    auto fit = st->fields.find(field_name);
    if (fit == st->fields.end()) return false;

    auto obj = std::get_if<ObjectKind>(&fit->second);
    if (!obj) return false;

    auto tit = type_table.find(obj->type_name);
    return tit != type_table.end() && std::holds_alternative<PropertyType>(tit->second);
}

std::optional<std::string> val::CGenerator::_IsArray(const std::string& var_name) const noexcept
{
    auto it = symbol_table.find(var_name);
    if (it == symbol_table.end()) return std::nullopt;

    if (auto arr = std::get_if<ArrayKind>(&it->second))
        return arr->of_kind.type_name;

    return std::nullopt;
}

std::optional<std::string> val::CGenerator::_IsArray(const std::string& struct_name, const std::string& field_name) const noexcept
{
    auto it = type_table.find(struct_name);
    if (it == type_table.end()) return std::nullopt;

    auto st = std::get_if<StructType>(&it->second);
    if (!st) return std::nullopt;

    auto fit = st->fields.find(field_name);
    if (fit == st->fields.end()) return std::nullopt;

    if (auto arr = std::get_if<ArrayKind>(&fit->second))
        return arr->of_kind.type_name;

    return std::nullopt;
}

bool val::CGenerator::IsExprTypeArray(const ExprType& expr)
{
    return expr == ExprType::ArrayOfEnum || expr == ExprType::ArrayOfPrim || expr == ExprType::ArrayOfString
        || expr == ExprType::ArrayOfStruct || expr == ExprType::ArrayOfProp;
}

ExprRet val::CGenerator::EvalExpression(const Expression& expr_call_stmt) noexcept
{
    switch (expr_call_stmt.sel())
    {
    case selector::FieldCallExpr:
        return EvalFieldCall(expr_call_stmt);
    case selector::ArrayIndexExpr:
        return EvalArrIndex(expr_call_stmt);
    case selector::VarNameExpr:
        return EvalVarName(expr_call_stmt);
    case selector::FnCallExpr:
        return EvalFnCallExpr(expr_call_stmt);
    case selector::BinaryExpr:
        return EvalBinaryExpr(expr_call_stmt);
    case selector::UnaryExpr:
        return EvalUnaryExpr(expr_call_stmt);
    case selector::InitListExpr:
        return EvalInitListExpr(expr_call_stmt);
    case selector::StructInitExpr:
        return EvalStructInitExpr(expr_call_stmt);
    default:
        return EvalLiteral(expr_call_stmt);
    }
}

void val::CGenerator::GenExprCall(const Statement& expr_call_stmt, std::ostream& to, size_t tabs) noexcept
{
    if (not expr_call_stmt.view_ExprCall().expr().option_is_FnCall()) { return; }

    to << indent(tabs);

    const std::string& fn_name = expr_call_stmt.view_ExprCall().expr().view_FnCall().fn_name();

    if (fn_name == "printf" || fn_name == "scanf")
    {
        // Skip format string
        to << fn_name << "(" << EvalExpression(expr_call_stmt.view_ExprCall().expr().view_FnCall().args(0)).str_expr;
        for (size_t i = 1; i < expr_call_stmt.view_ExprCall().expr().view_FnCall().size(); i++)
        {
            to << ", ";
            auto arg_expr = EvalExpression(expr_call_stmt.view_ExprCall().expr().view_FnCall().args(i));
            
            if (arg_expr.type_name == "string")
            {
                to << arg_expr.str_expr << "->data";
            }
            else if (IsPrimitive(arg_expr.type_name))
            {
                to << arg_expr.str_expr;
            }
            
        }
        to << ");\n";
    }
    else if (fn_name == "len")
    {
        to << EvalExpression(expr_call_stmt.view_ExprCall().expr().view_FnCall().args(0)).str_expr << "->length";
    }
    else {
        to << EvalExpression(expr_call_stmt.view_ExprCall().expr()).str_expr << ";\n";
    }
}

void val::CGenerator::GenAssign(const Statement& assign_stmt, std::ostream& to, size_t tabs) noexcept
{
    const auto& view = assign_stmt.view_Assignment();

    auto lhs_expr = EvalExpression(view.dest());
    auto rhs_expr = EvalExpression(view.expr());

    // Case 1: varname is assigned
    if (view.dest().option_is_VarName())
    {
        if (lhs_expr.type == ExprType::Primitive || lhs_expr.type == ExprType::Enum)
        {
            to << indent(tabs) << lhs_expr.str_expr << " = " << rhs_expr.str_expr << ";\n";
        }
        else if (lhs_expr.type == ExprType::Struct)
        {
            to << indent(tabs) << "MVS_DetachPointer((uintptr_t)" << lhs_expr.str_expr << ");\n";
            to << indent(tabs) << lhs_expr.str_expr << " = " << rhs_expr.str_expr << ";\n";
            to << indent(tabs) << "MVS_RegisterNew((uintptr_t)" << lhs_expr.str_expr
                << ", sizeof(" << lhs_expr.type_name << "), xx_free_" << lhs_expr.type_name << ");\n";
        }
        else if (lhs_expr.type == ExprType::Property)
        {
            if (rhs_expr.type_name == lhs_expr.type_name)
            {
                to << indent(tabs) << "MVS_DetachPointer((uintptr_t)" << lhs_expr.str_expr << ");\n";
                to << indent(tabs) << lhs_expr.str_expr << " = " << rhs_expr.str_expr << ";\n";
                to << indent(tabs) << "MVS_RegisterNew((uintptr_t)" << lhs_expr.str_expr
                    << ", sizeof(" << lhs_expr.type_name << "), xx_free_" << lhs_expr.type_name << ");\n";
            }
            else if (opt_to_prop.at(rhs_expr.type_name) == lhs_expr.type_name) {
                to << indent(tabs) << "MVS_DetachPointer((uintptr_t)" << lhs_expr.str_expr << ");\n";
                to << indent(tabs) << lhs_expr.str_expr << " = xx_init_" << lhs_expr.type_name
                    << "_" << rhs_expr.type_name << "(" << rhs_expr.str_expr << ");\n";
                to << indent(tabs) << "MVS_RegisterNew((uintptr_t)" << lhs_expr.str_expr
                    << ", sizeof(" << lhs_expr.type_name << "), xx_free_" << lhs_expr.type_name << ");\n";
            }
        }
        else if (lhs_expr.type == ExprType::String)
        {
            to << indent(tabs) << "if (MVS_RefCount((uintptr_t)" << lhs_expr.str_expr << ") > 1)\n";
            to << indent(tabs) << "{\n";
            to << indent(tabs + 1) << "MVS_DetachPointer((uintptr_t)" << lhs_expr.str_expr << ");\n";
            to << indent(tabs + 1) << lhs_expr.str_expr << " = " << rhs_expr.str_expr << ";\n";
            to << indent(tabs + 1) << "MVS_RegisterNew((uintptr_t)" << lhs_expr.str_expr
                << ", sizeof(string), xx_free_string);\n";
            to << indent(tabs) << "}\n";
            to << indent(tabs) << "else { " << lhs_expr.str_expr << " = " << rhs_expr.str_expr << "; }\n";
        }
        else if (lhs_expr.type == ExprType::ArrayOfPrim || lhs_expr.type == ExprType::ArrayOfEnum ||
            lhs_expr.type == ExprType::ArrayOfString || lhs_expr.type == ExprType::ArrayOfStruct ||
            lhs_expr.type == ExprType::ArrayOfProp)
        {
            to << indent(tabs) << "MVS_DetachPointer((uintptr_t)" << lhs_expr.str_expr << ");\n";
            to << indent(tabs) << lhs_expr.str_expr << " = " << rhs_expr.str_expr << ";\n";
            to << indent(tabs) << "MVS_RegisterNew((uintptr_t)" << lhs_expr.str_expr
                << ", sizeof(array), xx_free_array);\n";
        }
    }
    // Cases 2, 6: array_index is assigned (and nested array_index)
    else if (view.dest().option_is_ArrayIndex())
    {
        const auto& arr_idx = view.dest().view_ArrayIndex();

        // Case 6: array_index called with array_index (string[i][j] - char index into string element)
        if (arr_idx.array_expr().option_is_ArrayIndex())
        {
            // outer array holds strings, inner index selects char
            std::string arr_name = arr_idx.array_expr().view_ArrayIndex().array_expr().view_VarName().name();
            std::string outer_idx = EvalExpression(arr_idx.array_expr().view_ArrayIndex().at()).str_expr;
            std::string inner_idx = EvalExpression(arr_idx.at()).str_expr;

            to << indent(tabs) << "if (MVS_RefCount((uintptr_t)" << arr_name << ") > 1)\n";
            to << indent(tabs) << "{\n";
            to << indent(tabs + 1) << "array* xx_clone = xx_clone_array(" << arr_name << ");\n";
            to << indent(tabs + 1) << "MVS_DetachPointer((uintptr_t)" << arr_name << ");\n";
            to << indent(tabs + 1) << "string* xx_elem = *(string**)xx_array_get(xx_clone, " << outer_idx << ");\n";
            to << indent(tabs + 1) << "if (MVS_RefCount((uintptr_t)xx_elem) > 1)\n";
            to << indent(tabs + 1) << "{\n";
            to << indent(tabs + 2) << "string* xx_str_clone = xx_clone_string(xx_elem);\n";
            to << indent(tabs + 2) << "MVS_DetachPointer((uintptr_t)xx_elem);\n";
            to << indent(tabs + 2) << "xx_str_clone->data[" << inner_idx << "] = " << rhs_expr.str_expr << ";\n";
            to << indent(tabs + 2) << "xx_array_set(xx_clone, " << outer_idx << ", &xx_str_clone);\n";
            to << indent(tabs + 1) << "}\n";
            to << indent(tabs + 1) << "else { xx_string_set(xx_elem, " << inner_idx << ", " << rhs_expr.str_expr << "); }\n";
            to << indent(tabs + 1) << arr_name << " = xx_clone;\n";
            to << indent(tabs) << "}\n";
            to << indent(tabs) << "else\n";
            to << indent(tabs) << "{\n";
            to << indent(tabs + 1) << "string* xx_elem = *(string**)xx_array_get(" << arr_name << ", " << outer_idx << ");\n";
            to << indent(tabs + 1) << "if (MVS_RefCount((uintptr_t)xx_elem) > 1)\n";
            to << indent(tabs + 1) << "{\n";
            to << indent(tabs + 2) << "string* xx_str_clone = xx_clone_string(xx_elem);\n";
            to << indent(tabs + 2) << "MVS_DetachPointer((uintptr_t)xx_elem);\n";
            to << indent(tabs + 2) << "xx_str_clone->data[" << inner_idx << "] = " << rhs_expr.str_expr << ";\n";
            to << indent(tabs + 2) << "xx_array_set(" << arr_name << ", " << outer_idx << ", &xx_str_clone);\n";
            to << indent(tabs + 1) << "}\n";
            to << indent(tabs + 1) << "else { xx_string_set(xx_elem, " << inner_idx << ", " << rhs_expr.str_expr << "); }\n";
            to << indent(tabs) << "}\n";
        }
        // Case 2: simple array_index assigned
        else
        {
            std::string arr_name = arr_idx.array_expr().view_VarName().name();
            std::string at_expr_str = EvalExpression(arr_idx.at()).str_expr;

            if (lhs_expr.type == ExprType::Primitive || lhs_expr.type == ExprType::Enum)
            {
                to << indent(tabs) << "if (MVS_RefCount((uintptr_t)" << arr_name << ") > 1)\n";
                to << indent(tabs) << "{\n";
                to << indent(tabs + 1) << "array* xx_clone = xx_clone_array(" << arr_name << ");\n";
                to << indent(tabs + 1) << "MVS_DetachPointer((uintptr_t)" << arr_name << ");\n";
                to << indent(tabs + 1) << "xx_array_set(xx_clone, " << at_expr_str << ", "
                    << (rhs_expr.is_lvalue ? "&" + rhs_expr.str_expr : "&(" + lhs_expr.type_name + "){" + rhs_expr.str_expr + "}") << ");\n";
                to << indent(tabs + 1) << arr_name << " = xx_clone;\n";
                to << indent(tabs) << "}\n";
                to << indent(tabs) << "else { xx_array_set(" << arr_name << ", " << at_expr_str << ", "
                    << (rhs_expr.is_lvalue ? "&" + rhs_expr.str_expr : "&(" + lhs_expr.type_name + "){" + rhs_expr.str_expr + "}") << "); }\n";
            }
            else if (lhs_expr.type == ExprType::Struct)
            {
                to << indent(tabs) << "if (MVS_RefCount((uintptr_t)" << arr_name << ") > 1)\n";
                to << indent(tabs) << "{\n";
                to << indent(tabs + 1) << "array* xx_clone = xx_clone_array(" << arr_name << ");\n";
                to << indent(tabs + 1) << "MVS_DetachPointer((uintptr_t)" << arr_name << ");\n";
                to << indent(tabs + 1) << "xx_array_set(xx_clone, " << at_expr_str << ", &(" << rhs_expr.str_expr << "));\n";
                to << indent(tabs + 1) << arr_name << " = xx_clone;\n";
                to << indent(tabs) << "}\n";
                to << indent(tabs) << "else { xx_array_set(" << arr_name << ", " << at_expr_str << ", &(" << rhs_expr.str_expr << ")); }\n";
            }
            else if (lhs_expr.type == ExprType::Property)
            {
                to << indent(tabs) << "if (MVS_RefCount((uintptr_t)" << arr_name << ") > 1)\n";
                to << indent(tabs) << "{\n";
                to << indent(tabs + 1) << "array* xx_clone = xx_clone_array(" << arr_name << ");\n";
                to << indent(tabs + 1) << "MVS_DetachPointer((uintptr_t)" << arr_name << ");\n";
                to << indent(tabs + 1) << lhs_expr.type_name << "* xx_prop = xx_init_" << lhs_expr.type_name
                    << "_" << rhs_expr.type_name << "(" << rhs_expr.str_expr << ");\n";
                to << indent(tabs + 1) << "MVS_RegisterNew((uintptr_t)xx_prop, sizeof(" << lhs_expr.type_name << "), xx_free_" << lhs_expr.type_name << ");\n";
                to << indent(tabs + 1) << "xx_array_set(xx_clone, " << at_expr_str << ", &xx_prop);\n";
                to << indent(tabs + 1) << arr_name << " = xx_clone;\n";
                to << indent(tabs) << "}\n";
                to << indent(tabs) << "else\n";
                to << indent(tabs) << "{\n";
                to << indent(tabs + 1) << lhs_expr.type_name << "* xx_prop = xx_init_" << lhs_expr.type_name
                    << "_" << rhs_expr.type_name << "(" << rhs_expr.str_expr << ");\n";
                to << indent(tabs + 1) << "MVS_RegisterNew((uintptr_t)xx_prop, sizeof(" << lhs_expr.type_name << "), xx_free_" << lhs_expr.type_name << ");\n";
                to << indent(tabs + 1) << "xx_array_set(" << arr_name << ", " << at_expr_str << ", &xx_prop);\n";
                to << indent(tabs) << "}\n";
            }
            else if (lhs_expr.type == ExprType::String)
            {
                to << indent(tabs) << "if (MVS_RefCount((uintptr_t)" << arr_name << ") > 1)\n";
                to << indent(tabs) << "{\n";
                to << indent(tabs + 1) << "array* xx_clone = xx_clone_array(" << arr_name << ");\n";
                to << indent(tabs + 1) << "MVS_DetachPointer((uintptr_t)" << arr_name << ");\n";
                to << indent(tabs + 1) << "string* xx_new_s = xx_init_string(" << rhs_expr.str_expr << ");\n";
                to << indent(tabs + 1) << "MVS_RegisterNew((uintptr_t)xx_new_s, sizeof(string), xx_free_string);\n";
                to << indent(tabs + 1) << "xx_array_set(xx_clone, " << at_expr_str << ", &xx_new_s);\n";
                to << indent(tabs + 1) << arr_name << " = xx_clone;\n";
                to << indent(tabs) << "}\n";
                to << indent(tabs) << "else\n";
                to << indent(tabs) << "{\n";
                to << indent(tabs + 1) << "string* xx_new_s = xx_init_string(" << rhs_expr.str_expr << ");\n";
                to << indent(tabs + 1) << "MVS_RegisterNew((uintptr_t)xx_new_s, sizeof(string), xx_free_string);\n";
                to << indent(tabs + 1) << "xx_array_set(" << arr_name << ", " << at_expr_str << ", &xx_new_s);\n";
                to << indent(tabs) << "}\n";
            }
        }
    }
    // Cases 3, 4, 5, 7, 8, 9: field call is assigned
    else if (view.dest().option_is_FieldCall())
    {
        const auto& fc = view.dest().view_FieldCall();

        // Cases 3, 5, 7: caller is varname
        if (fc.caller().option_is_VarName())
        {
            std::string ptr_var = fc.caller().view_VarName().name();
            std::string caller_type = EvalExpression(fc.caller()).type_name;

            // Cases 5, 7: field is array_index (possibly nested for char in string)
            if (fc.field().option_is_ArrayIndex())
            {
                std::string field_arr_name = fc.field().view_ArrayIndex().array_expr().view_VarName().name();
                std::string field_idx = EvalExpression(fc.field().view_ArrayIndex().at()).str_expr;
                auto elem_type_opt = _IsArray(caller_type, field_arr_name);
                std::string elem_type = elem_type_opt ? *elem_type_opt : "";

                // Case 7: field is array_index called with array_index (string[i][j])
                if (fc.field().view_ArrayIndex().array_expr().option_is_ArrayIndex())
                {
                    std::string outer_field = fc.field().view_ArrayIndex().array_expr()
                        .view_ArrayIndex().array_expr().view_VarName().name();
                    std::string outer_idx = EvalExpression(fc.field().view_ArrayIndex().array_expr()
                        .view_ArrayIndex().at()).str_expr;
                    std::string inner_idx = EvalExpression(fc.field().view_ArrayIndex().at()).str_expr;

                    to << indent(tabs) << "if (MVS_RefCount((uintptr_t)" << ptr_var << ") > 1)\n";
                    to << indent(tabs) << "{\n";
                    to << indent(tabs + 1) << caller_type << "* xx_clone = xx_clone_" << caller_type << "(" << ptr_var << ");\n";
                    to << indent(tabs + 1) << "MVS_DetachPointer((uintptr_t)" << ptr_var << ");\n";
                    to << indent(tabs + 1) << "array* xx_field_arr = xx_clone->" << outer_field << ";\n";
                    to << indent(tabs + 1) << "if (MVS_RefCount((uintptr_t)xx_field_arr) > 1)\n";
                    to << indent(tabs + 1) << "{\n";
                    to << indent(tabs + 2) << "array* xx_arr_clone = xx_clone_array(xx_field_arr);\n";
                    to << indent(tabs + 2) << "MVS_DetachPointer((uintptr_t)xx_field_arr);\n";
                    to << indent(tabs + 2) << "xx_clone->" << outer_field << " = xx_arr_clone;\n";
                    to << indent(tabs + 2) << "string* xx_elem = *(string**)xx_array_get(xx_arr_clone, " << outer_idx << ");\n";
                    to << indent(tabs + 2) << "if (MVS_RefCount((uintptr_t)xx_elem) > 1)\n";
                    to << indent(tabs + 2) << "{\n";
                    to << indent(tabs + 3) << "string* xx_str_clone = xx_clone_string(xx_elem);\n";
                    to << indent(tabs + 3) << "MVS_DetachPointer((uintptr_t)xx_elem);\n";
                    to << indent(tabs + 3) << "xx_str_clone->data[" << inner_idx << "] = " << rhs_expr.str_expr << ";\n";
                    to << indent(tabs + 3) << "xx_array_set(xx_arr_clone, " << outer_idx << ", &xx_str_clone);\n";
                    to << indent(tabs + 2) << "}\n";
                    to << indent(tabs + 2) << "else { xx_string_set(xx_elem, " << inner_idx << ", " << rhs_expr.str_expr << "); }\n";
                    to << indent(tabs + 1) << "}\n";
                    to << indent(tabs + 1) << "else\n";
                    to << indent(tabs + 1) << "{\n";
                    to << indent(tabs + 2) << "string* xx_elem = *(string**)xx_array_get(xx_field_arr, " << outer_idx << ");\n";
                    to << indent(tabs + 2) << "if (MVS_RefCount((uintptr_t)xx_elem) > 1)\n";
                    to << indent(tabs + 2) << "{\n";
                    to << indent(tabs + 3) << "string* xx_str_clone = xx_clone_string(xx_elem);\n";
                    to << indent(tabs + 3) << "MVS_DetachPointer((uintptr_t)xx_elem);\n";
                    to << indent(tabs + 3) << "xx_str_clone->data[" << inner_idx << "] = " << rhs_expr.str_expr << ";\n";
                    to << indent(tabs + 3) << "xx_array_set(xx_field_arr, " << outer_idx << ", &xx_str_clone);\n";
                    to << indent(tabs + 2) << "}\n";
                    to << indent(tabs + 2) << "else { xx_string_set(xx_elem, " << inner_idx << ", " << rhs_expr.str_expr << "); }\n";
                    to << indent(tabs + 1) << "}\n";
                    to << indent(tabs + 1) << ptr_var << " = xx_clone;\n";
                    to << indent(tabs) << "}\n";
                    to << indent(tabs) << "else\n";
                    to << indent(tabs) << "{\n";
                    to << indent(tabs + 1) << "array* xx_field_arr = " << ptr_var << "->" << outer_field << ";\n";
                    to << indent(tabs + 1) << "if (MVS_RefCount((uintptr_t)xx_field_arr) > 1)\n";
                    to << indent(tabs + 1) << "{\n";
                    to << indent(tabs + 2) << "array* xx_arr_clone = xx_clone_array(xx_field_arr);\n";
                    to << indent(tabs + 2) << "MVS_DetachPointer((uintptr_t)xx_field_arr);\n";
                    to << indent(tabs + 2) << ptr_var << "->" << outer_field << " = xx_arr_clone;\n";
                    to << indent(tabs + 2) << "string* xx_elem = *(string**)xx_array_get(xx_arr_clone, " << outer_idx << ");\n";
                    to << indent(tabs + 2) << "if (MVS_RefCount((uintptr_t)xx_elem) > 1)\n";
                    to << indent(tabs + 2) << "{\n";
                    to << indent(tabs + 3) << "string* xx_str_clone = xx_clone_string(xx_elem);\n";
                    to << indent(tabs + 3) << "MVS_DetachPointer((uintptr_t)xx_elem);\n";
                    to << indent(tabs + 3) << "xx_str_clone->data[" << inner_idx << "] = " << rhs_expr.str_expr << ";\n";
                    to << indent(tabs + 3) << "xx_array_set(xx_arr_clone, " << outer_idx << ", &xx_str_clone);\n";
                    to << indent(tabs + 2) << "}\n";
                    to << indent(tabs + 2) << "else { xx_string_set(xx_elem, " << inner_idx << ", " << rhs_expr.str_expr << "); }\n";
                    to << indent(tabs + 1) << "}\n";
                    to << indent(tabs + 1) << "else\n";
                    to << indent(tabs + 1) << "{\n";
                    to << indent(tabs + 2) << "string* xx_elem = *(string**)xx_array_get(xx_field_arr, " << outer_idx << ");\n";
                    to << indent(tabs + 2) << "if (MVS_RefCount((uintptr_t)xx_elem) > 1)\n";
                    to << indent(tabs + 2) << "{\n";
                    to << indent(tabs + 3) << "string* xx_str_clone = xx_clone_string(xx_elem);\n";
                    to << indent(tabs + 3) << "MVS_DetachPointer((uintptr_t)xx_elem);\n";
                    to << indent(tabs + 3) << "xx_str_clone->data[" << inner_idx << "] = " << rhs_expr.str_expr << ";\n";
                    to << indent(tabs + 3) << "xx_array_set(xx_field_arr, " << outer_idx << ", &xx_str_clone);\n";
                    to << indent(tabs + 2) << "}\n";
                    to << indent(tabs + 2) << "else { xx_string_set(xx_elem, " << inner_idx << ", " << rhs_expr.str_expr << "); }\n";
                    to << indent(tabs + 1) << "}\n";
                    to << indent(tabs) << "}\n";
                }
                // Case 5: field is simple array_index (varname->arr[i] = ...)
                else
                {
                    to << indent(tabs) << "if (MVS_RefCount((uintptr_t)" << ptr_var << ") > 1)\n";
                    to << indent(tabs) << "{\n";
                    to << indent(tabs + 1) << caller_type << "* xx_clone = xx_clone_" << caller_type << "(" << ptr_var << ");\n";
                    to << indent(tabs + 1) << "MVS_DetachPointer((uintptr_t)" << ptr_var << ");\n";
                    to << indent(tabs + 1) << "array* xx_field_arr = xx_clone->" << field_arr_name << ";\n";
                    to << indent(tabs + 1) << "if (MVS_RefCount((uintptr_t)xx_field_arr) > 1)\n";
                    to << indent(tabs + 1) << "{\n";
                    to << indent(tabs + 2) << "array* xx_arr_clone = xx_clone_array(xx_field_arr);\n";
                    to << indent(tabs + 2) << "MVS_DetachPointer((uintptr_t)xx_field_arr);\n";
                    to << indent(tabs + 2) << "xx_clone->" << field_arr_name << " = xx_arr_clone;\n";

                    if (elem_type == "string")
                    {
                        to << indent(tabs + 2) << "string* xx_new_s = xx_init_string(" << rhs_expr.str_expr << ");\n";
                        to << indent(tabs + 2) << "MVS_RegisterNew((uintptr_t)xx_new_s, sizeof(string), xx_free_string);\n";
                        to << indent(tabs + 2) << "xx_array_set(xx_arr_clone, " << field_idx << ", &xx_new_s);\n";
                    }
                    else if (elem_type == "uint" || elem_type == "int" || elem_type == "double" || elem_type == "char" || elem_type == "bool")
                    {
                        to << indent(tabs + 2) << "xx_array_set(xx_arr_clone, " << field_idx << ", "
                            << (rhs_expr.is_lvalue ? "&" + rhs_expr.str_expr : "&(" + elem_type + "){" + rhs_expr.str_expr + "}") << ");\n";
                    }
                    else
                    {
                        to << indent(tabs + 2) << "xx_array_set(xx_arr_clone, " << field_idx << ", &(" << rhs_expr.str_expr << "));\n";
                    }

                    to << indent(tabs + 1) << "}\n";
                    to << indent(tabs + 1) << "else\n";
                    to << indent(tabs + 1) << "{\n";

                    if (elem_type == "string")
                    {
                        to << indent(tabs + 2) << "string* xx_new_s = xx_init_string(" << rhs_expr.str_expr << ");\n";
                        to << indent(tabs + 2) << "MVS_RegisterNew((uintptr_t)xx_new_s, sizeof(string), xx_free_string);\n";
                        to << indent(tabs + 2) << "xx_array_set(xx_field_arr, " << field_idx << ", &xx_new_s);\n";
                    }
                    else if (elem_type == "uint" || elem_type == "int" || elem_type == "double" || elem_type == "char" || elem_type == "bool")
                    {
                        to << indent(tabs + 2) << "xx_array_set(xx_field_arr, " << field_idx << ", "
                            << (rhs_expr.is_lvalue ? "&" + rhs_expr.str_expr : "&(" + elem_type + "){" + rhs_expr.str_expr + "}") << ");\n";
                    }
                    else
                    {
                        to << indent(tabs + 2) << "xx_array_set(xx_field_arr, " << field_idx << ", &(" << rhs_expr.str_expr << "));\n";
                    }

                    to << indent(tabs + 1) << "}\n";
                    to << indent(tabs + 1) << ptr_var << " = xx_clone;\n";
                    to << indent(tabs) << "}\n";
                    to << indent(tabs) << "else\n";
                    to << indent(tabs) << "{\n";
                    to << indent(tabs + 1) << "array* xx_field_arr = " << ptr_var << "->" << field_arr_name << ";\n";
                    to << indent(tabs + 1) << "if (MVS_RefCount((uintptr_t)xx_field_arr) > 1)\n";
                    to << indent(tabs + 1) << "{\n";
                    to << indent(tabs + 2) << "array* xx_arr_clone = xx_clone_array(xx_field_arr);\n";
                    to << indent(tabs + 2) << "MVS_DetachPointer((uintptr_t)xx_field_arr);\n";
                    to << indent(tabs + 2) << ptr_var << "->" << field_arr_name << " = xx_arr_clone;\n";

                    if (elem_type == "string")
                    {
                        to << indent(tabs + 2) << "string* xx_new_s = xx_init_string(" << rhs_expr.str_expr << ");\n";
                        to << indent(tabs + 2) << "MVS_RegisterNew((uintptr_t)xx_new_s, sizeof(string), xx_free_string);\n";
                        to << indent(tabs + 2) << "xx_array_set(xx_arr_clone, " << field_idx << ", &xx_new_s);\n";
                    }
                    else if (elem_type == "uint" || elem_type == "int" || elem_type == "double" || elem_type == "char" || elem_type == "bool")
                    {
                        to << indent(tabs + 2) << "xx_array_set(xx_arr_clone, " << field_idx << ", "
                            << (rhs_expr.is_lvalue ? "&" + rhs_expr.str_expr : "&(" + elem_type + "){" + rhs_expr.str_expr + "}") << ");\n";
                    }
                    else
                    {
                        to << indent(tabs + 2) << "xx_array_set(xx_arr_clone, " << field_idx << ", &(" << rhs_expr.str_expr << "));\n";
                    }

                    to << indent(tabs + 1) << "}\n";
                    to << indent(tabs + 1) << "else\n";
                    to << indent(tabs + 1) << "{\n";

                    if (elem_type == "string")
                    {
                        to << indent(tabs + 2) << "string* xx_new_s = xx_init_string(" << rhs_expr.str_expr << ");\n";
                        to << indent(tabs + 2) << "MVS_RegisterNew((uintptr_t)xx_new_s, sizeof(string), xx_free_string);\n";
                        to << indent(tabs + 2) << "xx_array_set(xx_field_arr, " << field_idx << ", &xx_new_s);\n";
                    }
                    else if (elem_type == "uint" || elem_type == "int" || elem_type == "double" || elem_type == "char" || elem_type == "bool")
                    {
                        to << indent(tabs + 2) << "xx_array_set(xx_field_arr, " << field_idx << ", "
                            << (rhs_expr.is_lvalue ? "&" + rhs_expr.str_expr : "&(" + elem_type + "){" + rhs_expr.str_expr + "}") << ");\n";
                    }
                    else
                    {
                        to << indent(tabs + 2) << "xx_array_set(xx_field_arr, " << field_idx << ", &(" << rhs_expr.str_expr << "));\n";
                    }

                    to << indent(tabs + 1) << "}\n";
                    to << indent(tabs) << "}\n";
                }
            }
            // Case 3: caller is varname, field is varname
            else
            {
                std::string field_name = fc.field().view_VarName().name();
                std::string var_type = caller_type;

                to << indent(tabs) << "if (MVS_RefCount((uintptr_t)" << ptr_var << ") > 1)\n";
                to << indent(tabs) << "{\n";
                to << indent(tabs + 1) << var_type << "* xx_clone = xx_clone_" << var_type << "(" << ptr_var << ");\n";
                to << indent(tabs + 1) << "MVS_DetachPointer((uintptr_t)" << ptr_var << ");\n";

                if (lhs_expr.type == ExprType::Primitive || lhs_expr.type == ExprType::Enum)
                {
                    to << indent(tabs + 1) << "xx_clone->" << field_name << " = " << rhs_expr.str_expr << ";\n";
                }
                else if (lhs_expr.type == ExprType::Struct)
                {
                    to << indent(tabs + 1) << "MVS_DetachPointer((uintptr_t)xx_clone->" << field_name << ");\n";
                    to << indent(tabs + 1) << "xx_clone->" << field_name << " = " << rhs_expr.str_expr << ";\n";
                    to << indent(tabs + 1) << "MVS_RegisterNew((uintptr_t)xx_clone->" << field_name
                        << ", sizeof(" << lhs_expr.type_name << "), xx_free_" << lhs_expr.type_name << ");\n";
                }
                else if (lhs_expr.type == ExprType::Property)
                {
                    to << indent(tabs + 1) << "MVS_DetachPointer((uintptr_t)xx_clone->" << field_name << ");\n";
                    to << indent(tabs + 1) << "xx_clone->" << field_name << " = xx_init_" << lhs_expr.type_name
                        << "_" << rhs_expr.type_name << "(" << rhs_expr.str_expr << ");\n";
                    to << indent(tabs + 1) << "MVS_RegisterNew((uintptr_t)xx_clone->" << field_name
                        << ", sizeof(" << lhs_expr.type_name << "), xx_free_" << lhs_expr.type_name << ");\n";
                }
                else if (lhs_expr.type == ExprType::String)
                {
                    to << indent(tabs + 1) << "MVS_DetachPointer((uintptr_t)xx_clone->" << field_name << ");\n";
                    to << indent(tabs + 1) << "xx_clone->" << field_name << " = " << rhs_expr.str_expr << ";\n";
                    to << indent(tabs + 1) << "MVS_RegisterNew((uintptr_t)xx_clone->" << field_name
                        << ", sizeof(string), xx_free_string);\n";
                }
                else // array
                {
                    to << indent(tabs + 1) << "MVS_DetachPointer((uintptr_t)xx_clone->" << field_name << ");\n";
                    to << indent(tabs + 1) << "xx_clone->" << field_name << " = " << rhs_expr.str_expr << ";\n";
                    to << indent(tabs + 1) << "MVS_RegisterNew((uintptr_t)xx_clone->" << field_name
                        << ", sizeof(array), xx_free_array);\n";
                }

                to << indent(tabs + 1) << ptr_var << " = xx_clone;\n";
                to << indent(tabs) << "}\n";
                to << indent(tabs) << "else\n";
                to << indent(tabs) << "{\n";

                if (lhs_expr.type == ExprType::Primitive || lhs_expr.type == ExprType::Enum)
                {
                    to << indent(tabs + 1) << lhs_expr.str_expr << " = " << rhs_expr.str_expr << ";\n";
                }
                else if (lhs_expr.type == ExprType::Struct)
                {
                    to << indent(tabs + 1) << "MVS_DetachPointer((uintptr_t)" << lhs_expr.str_expr << ");\n";
                    to << indent(tabs + 1) << lhs_expr.str_expr << " = " << rhs_expr.str_expr << ";\n";
                    to << indent(tabs + 1) << "MVS_RegisterNew((uintptr_t)" << lhs_expr.str_expr
                        << ", sizeof(" << lhs_expr.type_name << "), xx_free_" << lhs_expr.type_name << ");\n";
                }
                else if (lhs_expr.type == ExprType::Property)
                {
                    to << indent(tabs + 1) << "MVS_DetachPointer((uintptr_t)" << lhs_expr.str_expr << ");\n";
                    to << indent(tabs + 1) << lhs_expr.str_expr << " = xx_init_" << lhs_expr.type_name
                        << "_" << rhs_expr.type_name << "(" << rhs_expr.str_expr << ");\n";
                    to << indent(tabs + 1) << "MVS_RegisterNew((uintptr_t)" << lhs_expr.str_expr
                        << ", sizeof(" << lhs_expr.type_name << "), xx_free_" << lhs_expr.type_name << ");\n";
                }
                else if (lhs_expr.type == ExprType::String)
                {
                    to << indent(tabs + 1) << "MVS_DetachPointer((uintptr_t)" << lhs_expr.str_expr << ");\n";
                    to << indent(tabs + 1) << lhs_expr.str_expr << " = " << rhs_expr.str_expr << ";\n";
                    to << indent(tabs + 1) << "MVS_RegisterNew((uintptr_t)" << lhs_expr.str_expr
                        << ", sizeof(string), xx_free_string);\n";
                }
                else // array
                {
                    to << indent(tabs + 1) << "MVS_DetachPointer((uintptr_t)" << lhs_expr.str_expr << ");\n";
                    to << indent(tabs + 1) << lhs_expr.str_expr << " = " << rhs_expr.str_expr << ";\n";
                    to << indent(tabs + 1) << "MVS_RegisterNew((uintptr_t)" << lhs_expr.str_expr
                        << ", sizeof(array), xx_free_array);\n";
                }

                to << indent(tabs) << "}\n";
            }
        }
        // Cases 4, 8, 9: caller is array_index
        else if (fc.caller().option_is_ArrayIndex())
        {
            std::string arr_name = fc.caller().view_ArrayIndex().array_expr().view_VarName().name();
            std::string arr_idx_str = EvalExpression(fc.caller().view_ArrayIndex().at()).str_expr;
            std::string caller_type = EvalExpression(fc.caller()).type_name;

            // Cases 8, 9: field is array_index
            if (fc.field().option_is_ArrayIndex())
            {
                std::string field_arr_name = fc.field().view_ArrayIndex().array_expr().view_VarName().name();
                std::string field_idx = EvalExpression(fc.field().view_ArrayIndex().at()).str_expr;
                auto elem_type_opt = _IsArray(caller_type, field_arr_name);
                std::string elem_type = elem_type_opt ? *elem_type_opt : "";

                // Case 9: field is array_index called with array_index (string elem char index)
                if (fc.field().view_ArrayIndex().array_expr().option_is_ArrayIndex())
                {
                    std::string outer_field = fc.field().view_ArrayIndex().array_expr()
                        .view_ArrayIndex().array_expr().view_VarName().name();
                    std::string outer_idx = EvalExpression(fc.field().view_ArrayIndex().array_expr()
                        .view_ArrayIndex().at()).str_expr;
                    std::string inner_idx = EvalExpression(fc.field().view_ArrayIndex().at()).str_expr;

                    to << indent(tabs) << "if (MVS_RefCount((uintptr_t)" << arr_name << ") > 1)\n";
                    to << indent(tabs) << "{\n";
                    to << indent(tabs + 1) << "array* xx_outer_clone = xx_clone_array(" << arr_name << ");\n";
                    to << indent(tabs + 1) << "MVS_DetachPointer((uintptr_t)" << arr_name << ");\n";
                    to << indent(tabs + 1) << caller_type << "* xx_struct = *(" << caller_type << "**)xx_array_get(xx_outer_clone, " << arr_idx_str << ");\n";
                    to << indent(tabs + 1) << "if (MVS_RefCount((uintptr_t)xx_struct) > 1)\n";
                    to << indent(tabs + 1) << "{\n";
                    to << indent(tabs + 2) << caller_type << "* xx_struct_clone = xx_clone_" << caller_type << "(xx_struct);\n";
                    to << indent(tabs + 2) << "MVS_DetachPointer((uintptr_t)xx_struct);\n";
                    to << indent(tabs + 2) << "xx_array_set(xx_outer_clone, " << arr_idx_str << ", &xx_struct_clone);\n";
                    to << indent(tabs + 2) << "xx_struct = xx_struct_clone;\n";
                    to << indent(tabs + 1) << "}\n";
                    to << indent(tabs + 1) << "array* xx_field_arr = xx_struct->" << outer_field << ";\n";
                    to << indent(tabs + 1) << "if (MVS_RefCount((uintptr_t)xx_field_arr) > 1)\n";
                    to << indent(tabs + 1) << "{\n";
                    to << indent(tabs + 2) << "array* xx_arr_clone = xx_clone_array(xx_field_arr);\n";
                    to << indent(tabs + 2) << "MVS_DetachPointer((uintptr_t)xx_field_arr);\n";
                    to << indent(tabs + 2) << "xx_struct->" << outer_field << " = xx_arr_clone;\n";
                    to << indent(tabs + 2) << "string* xx_elem = *(string**)xx_array_get(xx_arr_clone, " << outer_idx << ");\n";
                    to << indent(tabs + 2) << "if (MVS_RefCount((uintptr_t)xx_elem) > 1)\n";
                    to << indent(tabs + 2) << "{\n";
                    to << indent(tabs + 3) << "string* xx_str_clone = xx_clone_string(xx_elem);\n";
                    to << indent(tabs + 3) << "MVS_DetachPointer((uintptr_t)xx_elem);\n";
                    to << indent(tabs + 3) << "xx_str_clone->data[" << inner_idx << "] = " << rhs_expr.str_expr << ";\n";
                    to << indent(tabs + 3) << "xx_array_set(xx_arr_clone, " << outer_idx << ", &xx_str_clone);\n";
                    to << indent(tabs + 2) << "}\n";
                    to << indent(tabs + 2) << "else { xx_string_set(xx_elem, " << inner_idx << ", " << rhs_expr.str_expr << "); }\n";
                    to << indent(tabs + 1) << "}\n";
                    to << indent(tabs + 1) << "else\n";
                    to << indent(tabs + 1) << "{\n";
                    to << indent(tabs + 2) << "string* xx_elem = *(string**)xx_array_get(xx_field_arr, " << outer_idx << ");\n";
                    to << indent(tabs + 2) << "if (MVS_RefCount((uintptr_t)xx_elem) > 1)\n";
                    to << indent(tabs + 2) << "{\n";
                    to << indent(tabs + 3) << "string* xx_str_clone = xx_clone_string(xx_elem);\n";
                    to << indent(tabs + 3) << "MVS_DetachPointer((uintptr_t)xx_elem);\n";
                    to << indent(tabs + 3) << "xx_str_clone->data[" << inner_idx << "] = " << rhs_expr.str_expr << ";\n";
                    to << indent(tabs + 3) << "xx_array_set(xx_field_arr, " << outer_idx << ", &xx_str_clone);\n";
                    to << indent(tabs + 2) << "}\n";
                    to << indent(tabs + 2) << "else { xx_string_set(xx_elem, " << inner_idx << ", " << rhs_expr.str_expr << "); }\n";
                    to << indent(tabs + 1) << "}\n";
                    to << indent(tabs + 1) << arr_name << " = xx_outer_clone;\n";
                    to << indent(tabs) << "}\n";
                    to << indent(tabs) << "else\n";
                    to << indent(tabs) << "{\n";
                    to << indent(tabs + 1) << caller_type << "* xx_struct = *(" << caller_type << "**)xx_array_get(" << arr_name << ", " << arr_idx_str << ");\n";
                    to << indent(tabs + 1) << "if (MVS_RefCount((uintptr_t)xx_struct) > 1)\n";
                    to << indent(tabs + 1) << "{\n";
                    to << indent(tabs + 2) << caller_type << "* xx_struct_clone = xx_clone_" << caller_type << "(xx_struct);\n";
                    to << indent(tabs + 2) << "MVS_DetachPointer((uintptr_t)xx_struct);\n";
                    to << indent(tabs + 2) << "xx_array_set(" << arr_name << ", " << arr_idx_str << ", &xx_struct_clone);\n";
                    to << indent(tabs + 2) << "xx_struct = xx_struct_clone;\n";
                    to << indent(tabs + 1) << "}\n";
                    to << indent(tabs + 1) << "array* xx_field_arr = xx_struct->" << outer_field << ";\n";
                    to << indent(tabs + 1) << "if (MVS_RefCount((uintptr_t)xx_field_arr) > 1)\n";
                    to << indent(tabs + 1) << "{\n";
                    to << indent(tabs + 2) << "array* xx_arr_clone = xx_clone_array(xx_field_arr);\n";
                    to << indent(tabs + 2) << "MVS_DetachPointer((uintptr_t)xx_field_arr);\n";
                    to << indent(tabs + 2) << "xx_struct->" << outer_field << " = xx_arr_clone;\n";
                    to << indent(tabs + 2) << "string* xx_elem = *(string**)xx_array_get(xx_arr_clone, " << outer_idx << ");\n";
                    to << indent(tabs + 2) << "if (MVS_RefCount((uintptr_t)xx_elem) > 1)\n";
                    to << indent(tabs + 2) << "{\n";
                    to << indent(tabs + 3) << "string* xx_str_clone = xx_clone_string(xx_elem);\n";
                    to << indent(tabs + 3) << "MVS_DetachPointer((uintptr_t)xx_elem);\n";
                    to << indent(tabs + 3) << "xx_str_clone->data[" << inner_idx << "] = " << rhs_expr.str_expr << ";\n";
                    to << indent(tabs + 3) << "xx_array_set(xx_arr_clone, " << outer_idx << ", &xx_str_clone);\n";
                    to << indent(tabs + 2) << "}\n";
                    to << indent(tabs + 2) << "else { xx_string_set(xx_elem, " << inner_idx << ", " << rhs_expr.str_expr << "); }\n";
                    to << indent(tabs + 1) << "}\n";
                    to << indent(tabs + 1) << "else\n";
                    to << indent(tabs + 1) << "{\n";
                    to << indent(tabs + 2) << "string* xx_elem = *(string**)xx_array_get(xx_field_arr, " << outer_idx << ");\n";
                    to << indent(tabs + 2) << "if (MVS_RefCount((uintptr_t)xx_elem) > 1)\n";
                    to << indent(tabs + 2) << "{\n";
                    to << indent(tabs + 3) << "string* xx_str_clone = xx_clone_string(xx_elem);\n";
                    to << indent(tabs + 3) << "MVS_DetachPointer((uintptr_t)xx_elem);\n";
                    to << indent(tabs + 3) << "xx_str_clone->data[" << inner_idx << "] = " << rhs_expr.str_expr << ";\n";
                    to << indent(tabs + 3) << "xx_array_set(xx_field_arr, " << outer_idx << ", &xx_str_clone);\n";
                    to << indent(tabs + 2) << "}\n";
                    to << indent(tabs + 2) << "else { xx_string_set(xx_elem, " << inner_idx << ", " << rhs_expr.str_expr << "); }\n";
                    to << indent(tabs + 1) << "}\n";
                    to << indent(tabs) << "}\n";
                }
                // Case 8: caller is array_index, field is simple array_index
                else
                {
                    auto EmitArraySet = [&](const std::string& arr_ref, size_t t) {
                        if (elem_type == "string")
                        {
                            to << indent(t) << "string* xx_new_s = xx_init_string(" << rhs_expr.str_expr << ");\n";
                            to << indent(t) << "MVS_RegisterNew((uintptr_t)xx_new_s, sizeof(string), xx_free_string);\n";
                            to << indent(t) << "xx_array_set(" << arr_ref << ", " << field_idx << ", &xx_new_s);\n";
                        }
                        else if (elem_type == "uint" || elem_type == "int" || elem_type == "double" || elem_type == "char" || elem_type == "bool")
                        {
                            to << indent(t) << "xx_array_set(" << arr_ref << ", " << field_idx << ", "
                                << (rhs_expr.is_lvalue ? "&" + rhs_expr.str_expr : "&(" + elem_type + "){" + rhs_expr.str_expr + "}") << ");\n";
                        }
                        else if (!elem_type.empty() && std::holds_alternative<PropertyType>(type_table.at(elem_type)))
                        {
                            to << indent(t) << elem_type << "* xx_prop = xx_init_" << elem_type
                                << "_" << rhs_expr.type_name << "(" << rhs_expr.str_expr << ");\n";
                            to << indent(t) << "MVS_RegisterNew((uintptr_t)xx_prop, sizeof(" << elem_type << "), xx_free_" << elem_type << ");\n";
                            to << indent(t) << "xx_array_set(" << arr_ref << ", " << field_idx << ", &xx_prop);\n";
                        }
                        else
                        {
                            to << indent(t) << "xx_array_set(" << arr_ref << ", " << field_idx << ", &(" << rhs_expr.str_expr << "));\n";
                        }
                        };

                    to << indent(tabs) << "if (MVS_RefCount((uintptr_t)" << arr_name << ") > 1)\n";
                    to << indent(tabs) << "{\n";
                    to << indent(tabs + 1) << "array* xx_outer_clone = xx_clone_array(" << arr_name << ");\n";
                    to << indent(tabs + 1) << "MVS_DetachPointer((uintptr_t)" << arr_name << ");\n";
                    to << indent(tabs + 1) << caller_type << "* xx_struct = *(" << caller_type << "**)xx_array_get(xx_outer_clone, " << arr_idx_str << ");\n";
                    to << indent(tabs + 1) << "if (MVS_RefCount((uintptr_t)xx_struct) > 1)\n";
                    to << indent(tabs + 1) << "{\n";
                    to << indent(tabs + 2) << caller_type << "* xx_struct_clone = xx_clone_" << caller_type << "(xx_struct);\n";
                    to << indent(tabs + 2) << "MVS_DetachPointer((uintptr_t)xx_struct);\n";
                    to << indent(tabs + 2) << "xx_array_set(xx_outer_clone, " << arr_idx_str << ", &xx_struct_clone);\n";
                    to << indent(tabs + 2) << "xx_struct = xx_struct_clone;\n";
                    to << indent(tabs + 1) << "}\n";
                    to << indent(tabs + 1) << "array* xx_field_arr = xx_struct->" << field_arr_name << ";\n";
                    to << indent(tabs + 1) << "if (MVS_RefCount((uintptr_t)xx_field_arr) > 1)\n";
                    to << indent(tabs + 1) << "{\n";
                    to << indent(tabs + 2) << "array* xx_arr_clone = xx_clone_array(xx_field_arr);\n";
                    to << indent(tabs + 2) << "MVS_DetachPointer((uintptr_t)xx_field_arr);\n";
                    to << indent(tabs + 2) << "xx_struct->" << field_arr_name << " = xx_arr_clone;\n";
                    EmitArraySet("xx_arr_clone", tabs + 2);
                    to << indent(tabs + 1) << "}\n";
                    to << indent(tabs + 1) << "else\n";
                    to << indent(tabs + 1) << "{\n";
                    EmitArraySet("xx_field_arr", tabs + 2);
                    to << indent(tabs + 1) << "}\n";
                    to << indent(tabs + 1) << arr_name << " = xx_outer_clone;\n";
                    to << indent(tabs) << "}\n";
                    to << indent(tabs) << "else\n";
                    to << indent(tabs) << "{\n";
                    to << indent(tabs + 1) << caller_type << "* xx_struct = *(" << caller_type << "**)xx_array_get(" << arr_name << ", " << arr_idx_str << ");\n";
                    to << indent(tabs + 1) << "if (MVS_RefCount((uintptr_t)xx_struct) > 1)\n";
                    to << indent(tabs + 1) << "{\n";
                    to << indent(tabs + 2) << caller_type << "* xx_struct_clone = xx_clone_" << caller_type << "(xx_struct);\n";
                    to << indent(tabs + 2) << "MVS_DetachPointer((uintptr_t)xx_struct);\n";
                    to << indent(tabs + 2) << "xx_array_set(" << arr_name << ", " << arr_idx_str << ", &xx_struct_clone);\n";
                    to << indent(tabs + 2) << "xx_struct = xx_struct_clone;\n";
                    to << indent(tabs + 1) << "}\n";
                    to << indent(tabs + 1) << "array* xx_field_arr = xx_struct->" << field_arr_name << ";\n";
                    to << indent(tabs + 1) << "if (MVS_RefCount((uintptr_t)xx_field_arr) > 1)\n";
                    to << indent(tabs + 1) << "{\n";
                    to << indent(tabs + 2) << "array* xx_arr_clone = xx_clone_array(xx_field_arr);\n";
                    to << indent(tabs + 2) << "MVS_DetachPointer((uintptr_t)xx_field_arr);\n";
                    to << indent(tabs + 2) << "xx_struct->" << field_arr_name << " = xx_arr_clone;\n";
                    EmitArraySet("xx_arr_clone", tabs + 2);
                    to << indent(tabs + 1) << "}\n";
                    to << indent(tabs + 1) << "else\n";
                    to << indent(tabs + 1) << "{\n";
                    EmitArraySet("xx_field_arr", tabs + 2);
                    to << indent(tabs + 1) << "}\n";
                    to << indent(tabs) << "}\n";
                }
            }
            // Case 4: caller is array_index, field is varname
            else
            {
                std::string field_name = fc.field().view_VarName().name();

                auto EmitFieldSet = [&](const std::string& struct_ref, size_t t) {
                    if (lhs_expr.type == ExprType::Primitive || lhs_expr.type == ExprType::Enum)
                    {
                        to << indent(t) << struct_ref << "->" << field_name << " = " << rhs_expr.str_expr << ";\n";
                    }
                    else if (lhs_expr.type == ExprType::Struct)
                    {
                        to << indent(t) << "MVS_DetachPointer((uintptr_t)" << struct_ref << "->" << field_name << ");\n";
                        to << indent(t) << struct_ref << "->" << field_name << " = " << rhs_expr.str_expr << ";\n";
                        to << indent(t) << "MVS_RegisterNew((uintptr_t)" << struct_ref << "->" << field_name
                            << ", sizeof(" << lhs_expr.type_name << "), xx_free_" << lhs_expr.type_name << ");\n";
                    }
                    else if (lhs_expr.type == ExprType::Property)
                    {
                        to << indent(t) << "MVS_DetachPointer((uintptr_t)" << struct_ref << "->" << field_name << ");\n";
                        to << indent(t) << struct_ref << "->" << field_name << " = xx_init_" << lhs_expr.type_name
                            << "_" << rhs_expr.type_name << "(" << rhs_expr.str_expr << ");\n";
                        to << indent(t) << "MVS_RegisterNew((uintptr_t)" << struct_ref << "->" << field_name
                            << ", sizeof(" << lhs_expr.type_name << "), xx_free_" << lhs_expr.type_name << ");\n";
                    }
                    else if (lhs_expr.type == ExprType::String)
                    {
                        to << indent(t) << "MVS_DetachPointer((uintptr_t)" << struct_ref << "->" << field_name << ");\n";
                        to << indent(t) << struct_ref << "->" << field_name << " = " << rhs_expr.str_expr << ";\n";
                        to << indent(t) << "MVS_RegisterNew((uintptr_t)" << struct_ref << "->" << field_name
                            << ", sizeof(string), xx_free_string);\n";
                    }
                    else // array
                    {
                        to << indent(t) << "MVS_DetachPointer((uintptr_t)" << struct_ref << "->" << field_name << ");\n";
                        to << indent(t) << struct_ref << "->" << field_name << " = " << rhs_expr.str_expr << ";\n";
                        to << indent(t) << "MVS_RegisterNew((uintptr_t)" << struct_ref << "->" << field_name
                            << ", sizeof(array), xx_free_array);\n";
                    }
                    };

                to << indent(tabs) << "if (MVS_RefCount((uintptr_t)" << arr_name << ") > 1)\n";
                to << indent(tabs) << "{\n";
                to << indent(tabs + 1) << "array* xx_outer_clone = xx_clone_array(" << arr_name << ");\n";
                to << indent(tabs + 1) << "MVS_DetachPointer((uintptr_t)" << arr_name << ");\n";
                to << indent(tabs + 1) << caller_type << "* xx_struct = *(" << caller_type << "**)xx_array_get(xx_outer_clone, " << arr_idx_str << ");\n";
                to << indent(tabs + 1) << "if (MVS_RefCount((uintptr_t)xx_struct) > 1)\n";
                to << indent(tabs + 1) << "{\n";
                to << indent(tabs + 2) << caller_type << "* xx_struct_clone = xx_clone_" << caller_type << "(xx_struct);\n";
                to << indent(tabs + 2) << "MVS_DetachPointer((uintptr_t)xx_struct);\n";
                to << indent(tabs + 2) << "xx_array_set(xx_outer_clone, " << arr_idx_str << ", &xx_struct_clone);\n";
                to << indent(tabs + 2) << "xx_struct = xx_struct_clone;\n";
                to << indent(tabs + 1) << "}\n";
                EmitFieldSet("xx_struct", tabs + 1);
                to << indent(tabs + 1) << arr_name << " = xx_outer_clone;\n";
                to << indent(tabs) << "}\n";
                to << indent(tabs) << "else\n";
                to << indent(tabs) << "{\n";
                to << indent(tabs + 1) << caller_type << "* xx_struct = *(" << caller_type << "**)xx_array_get(" << arr_name << ", " << arr_idx_str << ");\n";
                to << indent(tabs + 1) << "if (MVS_RefCount((uintptr_t)xx_struct) > 1)\n";
                to << indent(tabs + 1) << "{\n";
                to << indent(tabs + 2) << caller_type << "* xx_struct_clone = xx_clone_" << caller_type << "(xx_struct);\n";
                to << indent(tabs + 2) << "MVS_DetachPointer((uintptr_t)xx_struct);\n";
                to << indent(tabs + 2) << "xx_array_set(" << arr_name << ", " << arr_idx_str << ", &xx_struct_clone);\n";
                to << indent(tabs + 2) << "xx_struct = xx_struct_clone;\n";
                to << indent(tabs + 1) << "}\n";
                EmitFieldSet("xx_struct", tabs + 1);
                to << indent(tabs) << "}\n";
            }
        }
    }
}

std::string val::CGenerator::GetTypeName(const VariableKind& v) noexcept
{
    return std::holds_alternative <ObjectKind>(v) ? std::get <ObjectKind>(v).type_name : std::get <ArrayKind>(v).of_kind.type_name;
}

ExprRet val::CGenerator::EvalFieldCall(const Expression& field_call_expr) noexcept
{
    const auto& view = field_call_expr.view_FieldCall();

    ExprRet ret;
    auto caller_ret = EvalExpression(view.caller());
    
    ret.is_lvalue = caller_ret.is_lvalue;
    ret.str_expr = caller_ret.str_expr;

    std::string field_name;
    if (view.field().option_is_VarName())
    {
        field_name = view.field().view_VarName().name();

        if (std::holds_alternative <StructType>(type_table.at(caller_ret.type_name)))
        {
            ret.type_name = GetTypeName(std::get <StructType>(type_table.at(caller_ret.type_name)).fields.at(field_name));
            ret.str_expr += "->" + field_name;
        }
        else {
            ret.type_name = GetTypeName(std::get <StructType>(type_table.at(cur_option_name)).fields.at(field_name));
            ret.str_expr = caller_ret.str_expr + "->opts." + cur_option_name + "_opt->" + field_name;
            caller_ret.type_name = cur_option_name;
        }
        if (_IsPrimitive(caller_ret.type_name, field_name))
        {
            ret.type = ExprType::Primitive;
        }
        else if (_IsEnumType(caller_ret.type_name, field_name))
        {
            ret.type = ExprType::Enum;
        }
        else if (_IsStructType(caller_ret.type_name, field_name))
        {
            ret.type = ExprType::Struct;
        }
        else if (_IsProperty(caller_ret.type_name, field_name))
        {
            ret.type = ExprType::Property;
        }
        else if (_IsString(caller_ret.type_name, field_name))
        {
            ret.type = ExprType::String;
        }
        else {
            if (_IsString(caller_ret.type_name, field_name)) { ret.type = ExprType::String; }
            else {
                auto t = _IsArray(caller_ret.type_name, field_name);

                if (*t == "string") { ret.type = ExprType::ArrayOfString; }
                else if (*t == "uint" || *t == "int" || *t == "double" || *t == "char" || *t == "bool") { ret.type = ExprType::ArrayOfPrim; }
                else if (std::holds_alternative <EnumType>(type_table.at(*t))) { ret.type = ExprType::ArrayOfEnum; }
                else if (std::holds_alternative <StructType>(type_table.at(*t))) { ret.type = ExprType::ArrayOfStruct; }
                else if (std::holds_alternative <PropertyType>(type_table.at(*t))) { ret.type = ExprType::ArrayOfProp; }
            }
        }
    }
    else {
        field_name = view.field().view_ArrayIndex().array_expr().view_VarName().name();
        ret.type_name = GetTypeName(std::get <StructType>(type_table.at(caller_ret.type_name)).fields.at(field_name));

        auto at_expr = EvalExpression(view.field().view_ArrayIndex().at());
        
		if (_IsString(caller_ret.type_name, field_name)) { 
            ret.str_expr += "->" + field_name + "->data[" + at_expr.str_expr + "]";
            ret.type = ExprType::Primitive; 
        }
        else {
            auto t = _IsArray(caller_ret.type_name, field_name);

            if (*t == "string") { 
                ret.type = ExprType::String; 
                ret.str_expr = "(*(string**)xx_array_get(" + caller_ret.str_expr + "->" + field_name + ", " + at_expr.str_expr + "))";
            }
            else if (*t == "uint" || *t == "int" || *t == "double" || *t == "char" || *t == "bool") { 
                ret.type = ExprType::Primitive; 
                ret.str_expr = "(*(" + *t + "*)xx_array_get(" + caller_ret.str_expr + "->" + field_name + ", " + at_expr.str_expr + "))";
            }
            else if (std::holds_alternative <EnumType>(type_table.at(*t))) { 
                ret.str_expr = "(*(" + *t + "*)xx_array_get(" + caller_ret.str_expr + "->" + field_name + ", " + at_expr.str_expr + "))";
                ret.type = ExprType::Enum; 
            }
            else if (std::holds_alternative <StructType>(type_table.at(*t))) { 
                ret.str_expr = "(*(" + *t + "**)xx_array_get(" + caller_ret.str_expr + "->" + field_name + ", " + at_expr.str_expr + "))";
                ret.type = ExprType::Struct; 
            }
        }
    }
    
    ret.type_name = GetTypeName(std::get <StructType>(type_table.at(caller_ret.type_name)).fields.at(field_name));
    return ret;
}

ExprRet val::CGenerator::EvalArrIndex(const Expression& arr_index_expr) noexcept
{
    auto ret = EvalExpression(arr_index_expr.view_ArrayIndex().array_expr());
    auto at = EvalExpression(arr_index_expr.view_ArrayIndex().at());

    if (ret.type == ExprType::ArrayOfPrim)
    {
        ret.str_expr = "(*(" + ret.type_name + "*)xx_array_get(" + ret.str_expr + ", " + at.str_expr + "))";
        ret.type = ExprType::Primitive;
    }
    else if (ret.type == ExprType::ArrayOfEnum)
    {
        ret.str_expr = "(*(" + ret.type_name + "*)xx_array_get(" + ret.str_expr + ", " + at.str_expr + "))";
        ret.type = ExprType::Enum;
    }
    else if (ret.type == ExprType::ArrayOfString)
    {
        ret.str_expr = "(*(string**)xx_array_get(" + ret.str_expr + ", " + at.str_expr + "))";
        ret.type = ExprType::String;
    }
    else if (ret.type == ExprType::ArrayOfProp)
    {
        ret.str_expr = "(*(" + ret.type_name + "**)xx_array_get(" + ret.str_expr + ", " + at.str_expr + "))";
        ret.type = ExprType::Property;
    }
    else if (ret.type == ExprType::ArrayOfStruct)
    {
        ret.str_expr = "(*(" + ret.type_name + "**)xx_array_get(" + ret.str_expr + ", " + at.str_expr + "))";
        ret.type = ExprType::Struct;
    }
    return ret;
}

ExprRet val::CGenerator::EvalVarName(const Expression& var_name_expr) noexcept
{
    ExprRet ret;

    ret.str_expr = ToString(var_name_expr);
    ret.is_lvalue = true;
    
    const std::string& name = var_name_expr.view_VarName().name();

    if (_IsPrimitive(name))
    {
        ret.type = ExprType::Primitive;
    }
    else if (_IsEnumType(name))
    {
        ret.type = ExprType::Enum;
    }
    else if (_IsString(name))
    {
        ret.type = ExprType::String;
    }
    else if (_IsStructType(name))
    {
        ret.type = ExprType::Struct;
    }
    else if (_IsProperty(name))
    {
        ret.type = ExprType::Property;
    }
    else {
        auto t = _IsArray(name);

        if (*t == "string") { ret.type = ExprType::ArrayOfString; }
        else if (*t == "uint" || *t == "int" || *t == "double" || *t == "char" || *t == "bool") { ret.type = ExprType::ArrayOfPrim; }
        else if (std::holds_alternative <EnumType>(type_table.at(*t))) { ret.type = ExprType::ArrayOfEnum; }
        else if (std::holds_alternative <StructType>(type_table.at(*t))) { ret.type = ExprType::ArrayOfStruct; }
        else if (std::holds_alternative <PropertyType>(type_table.at(*t))) { ret.type = ExprType::ArrayOfProp; }
    }

    ret.type_name = GetTypeName(symbol_table.at(name));
    return ret;
}

ExprRet val::CGenerator::EvalLiteral(const Expression& literal_expr) noexcept
{
    std::string str_expr = ToString(literal_expr);
    switch (literal_expr.sel())
    {
    case selector::StringLiteralExpr:
        return { ExprType::String, "string", str_expr, false};
    case selector::IntLiteralExpr:
        return { ExprType::Primitive, "int", str_expr, false };
    case selector::DoubleLiteralExpr:
        return { ExprType::Primitive, "double", str_expr, false };
    case selector::CharLiteralExpr:
        return { ExprType::Primitive, "char", str_expr, false };
    case selector::BoolLiteralExpr:
        return { ExprType::Primitive, "bool", str_expr, false };
    case selector::EmptyExpr:
        return { ExprType::Null, "void", str_expr, false };
    }
}

ExprRet val::CGenerator::EvalFnCallExpr(const Expression& fn_call_expr) noexcept
{
    ExprRet ret;

    ret.is_lvalue = false;
    ret.str_expr = ToString(fn_call_expr);
    
    if (fn_call_expr.view_FnCall().fn_name() == "printf" 
        || fn_call_expr.view_FnCall().fn_name() == "scanf" 
        || fn_call_expr.view_FnCall().fn_name() == "len")
    {
        ret.type = ExprType::Primitive;
        ret.type_name = "int";
        return ret;
    }

    const auto& ret_kind = fn_table.at(fn_call_expr.view_FnCall().fn_name()).ret_kind;

    if (std::holds_alternative <ObjectKind>(ret_kind))
    {
        std::string type_name = std::get <ObjectKind>(ret_kind).type_name;
        if (type_name == "string")
        {
            ret.type = ExprType::String;
        }
        else if (type_name == "uint" || type_name == "int" ||
            type_name == "double" || type_name == "char" ||
            type_name == "bool")
        {
            ret.type = ExprType::Primitive;
        }
        else if (std::holds_alternative <StructType>(type_table.at(type_name)))
        {
            ret.type = ExprType::Struct;
        }
        else if (std::holds_alternative <PropertyType>(type_table.at(type_name)))
        {
            ret.type = ExprType::Property;
        }
        else if (std::holds_alternative <EnumType>(type_table.at(type_name)))
        {
            ret.type = ExprType::Enum;
        }

        ret.type_name = type_name;
    }
    else {
        std::string type_name = std::get <ArrayKind>(ret_kind).of_kind.type_name;

        if (type_name == "string")
        {
            ret.type = ExprType::ArrayOfString;
        }
        else if (type_name == "uint" || type_name == "int" ||
            type_name == "double" || type_name == "char" ||
            type_name == "bool")
        {
            ret.type = ExprType::ArrayOfPrim;
        }
        else if (std::holds_alternative <StructType>(type_table.at(type_name)))
        {
            ret.type = ExprType::ArrayOfStruct;
        }
        else if (std::holds_alternative <PropertyType>(type_table.at(type_name)))
        {
            ret.type = ExprType::ArrayOfProp;
        }
        else if (std::holds_alternative <EnumType>(type_table.at(type_name)))
        {
            ret.type = ExprType::ArrayOfEnum;
        }

        ret.type_name = type_name;
    }

    return ret;
}

ExprRet val::CGenerator::EvalBinaryExpr(const Expression& bin_expr) noexcept
{
    ExprRet lret = EvalExpression(bin_expr.view_Binary().lhs());
    ExprRet rret = EvalExpression(bin_expr.view_Binary().rhs());

    ExprRet ret;

    ret.is_lvalue = false;
    ret.str_expr = ToString(bin_expr);

    auto IsAnyArray = [this](ExprType type) -> bool {
        return type == ExprType::ArrayOfPrim || type == ExprType::ArrayOfEnum || type == ExprType::ArrayOfStruct ||
            type == ExprType::ArrayOfProp || type == ExprType::ArrayOfString;
    };

    if (lret.type == ExprType::Primitive && IsAnyArray(rret.type))
    {
        ret.type_name = rret.type_name;
        ret.type = rret.type;
    }
    else if (rret.type == ExprType::Primitive && IsAnyArray(lret.type))
    {
        ret.type_name = lret.type_name;
        ret.type = lret.type;
    }
    else if ((rret.type == ExprType::Primitive && lret.type == ExprType::String) ||
        (lret.type == ExprType::Primitive && rret.type == ExprType::String))
    {
        ret.type_name = "string";
        ret.type = ExprType::String;
    }
    else {
        ret.type_name = lret.type_name;
        ret.type = lret.type;
    }

    return ret;
}

ExprRet val::CGenerator::EvalUnaryExpr(const Expression& unary_expr) noexcept
{
    ExprRet ret;
    
    ret.is_lvalue = false;
    ret.str_expr = ToString(unary_expr);
    ret.type = ExprType::Primitive;
    ret.type_name = EvalExpression(unary_expr.view_Unary().expr()).type_name;

    return ret;
}

ExprRet val::CGenerator::EvalInitListExpr(const Expression& init_list_expr) noexcept
{
    ExprRet ret;
    
    ret.is_lvalue = false;

    if (init_list_expr.view_InitList().size() == 0)
    {
        ret.type = ExprType::Null;
    }

    for (size_t i = 0; i < init_list_expr.view_InitList().size(); i++)
    {
        auto fret = EvalExpression(init_list_expr.view_InitList().exprs(i));

        if (fret.type == ExprType::Null) { ret.type = ExprType::Null; continue; }

        ret.type_name = fret.type_name;
        if (fret.type == ExprType::Primitive)
        {
            ret.type = ExprType::ArrayOfPrim;
            ret.str_expr = "xx_init_array((" + ret.type_name + "[]) {";
            break;
        }
        else if (fret.type == ExprType::String)
        {
            ret.type = ExprType::ArrayOfString;
            ret.str_expr = "xx_init_array((string*[]) {";
            break;
        }
        else if (fret.type == ExprType::Enum)
        {
            ret.type = ExprType::ArrayOfEnum;
            ret.str_expr = "xx_init_array((" + ret.type_name + "[]) {";
            break;
        }
        else if (fret.type == ExprType::Struct)
        {
            ret.type = ExprType::ArrayOfStruct;
            ret.str_expr = "xx_init_array((" + ret.type_name + "*[]) {";
            break;
        }
        else if (fret.type == ExprType::Property)
        {
            ret.str_expr = "xx_init_array((" + opt_to_prop.at(ret.type_name) + "*[]) {";
            ret.type = ExprType::ArrayOfProp;
            break;
        }
    }

    if (ret.type == ExprType::Null)
    {
        ret.str_expr = "NULL";
        return ret;
    }

    for (size_t i = 0; i < init_list_expr.view_InitList().size(); i++)
    {
        auto fret = EvalExpression(init_list_expr.view_InitList().exprs(i));
        ret.type_name = fret.type_name;
        
        if (fret.type == ExprType::Null) 
        {
            ret.str_expr += "NULL, ";
        }
        else if (fret.type == ExprType::Property)
        {
            ret.str_expr += "xx_init_" + opt_to_prop.at(fret.type_name) + "_" + fret.type_name + "(" + fret.str_expr + "), ";
        }
        else if (fret.type == ExprType::String)
        {
            ret.str_expr += "xx_init_string(" + fret.str_expr + "), ";
        }
        else
        {
            ret.str_expr += fret.str_expr + ", ";
        }
    }

    ret.str_expr += "}, " + std::to_string(init_list_expr.view_InitList().size()) + ", sizeof(";

    if (ret.type == ExprType::ArrayOfPrim || ret.type == ExprType::ArrayOfEnum)
    {
        ret.str_expr += ret.type_name + "), 0, NULL)";
    }
    else if (ret.type == ExprType::ArrayOfString)
    {
        ret.str_expr += "string*), 1, xx_free_string)";
    }
    else if (ret.type == ExprType::ArrayOfStruct || ret.type == ExprType::ArrayOfProp)
    {
        ret.str_expr += ret.type_name + "*), 1, xx_free_" + ret.type_name + ')';
    }

    return ret;
}

ExprRet val::CGenerator::EvalStructInitExpr(const Expression& struct_init_expr) noexcept
{
    ExprRet ret;
    ret.is_lvalue = false;
    ret.str_expr = ToString(struct_init_expr);
    
    std::string struct_name = struct_init_expr.view_StructInit().struct_name();
    ret.type_name = struct_name;
    if (opt_to_prop.contains(struct_name))
    {
        ret.type = ExprType::Property;
    }
    else 
    {
        ret.type = ExprType::Struct;
    }
    return ret;
}
