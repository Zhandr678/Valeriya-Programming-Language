#include "CGenerator.h"
#include <sstream>

#define __IS_OPTION_CHECK_FN_NAME(prop, opt) "xx_" + prop + "_option_is_" + opt
#define __FREE_FN_NAME(type) "xx_free_" + type
#define __INIT_FN_NAME(type) "xx_init_" + type
#define __AS_TAG(opt_name) opt_name + "_tag"
#define __AS_OPT(name) name + "_opt"
#define __INIT_PROP_OPT(type, opt_name) "xx_init_" + type + '_' + opt_name
#define __DELETER_POINTER(type) "(" + type + "*)address"
#define __AS_UINTPTR_T(type) "(uintptr_t)" + type

namespace val
{
	void CGenerator::PushPropertyCaseClause(const std::string& prop_name, const std::string& prop_type, const Statement& case_clause, std::ostream& to, size_t num_tabs, size_t order) noexcept
	{
		const auto& view = case_clause.view_CaseClause();

		if (order == 0)
		{
			to << std::string(num_tabs, '\t') << "if (";
		}
		else if (order > 0)
		{
			to << std::string(num_tabs, '\t') << "else if (";
		}

		if (view.is_wildcard())
		{
			if (order > 0) {
				to << "1)\n";
			}
		}
		else {
			is_prop = true;
			cur_option = view.case_expr().view_VarName().name();
			to << __IS_OPTION_CHECK_FN_NAME(prop_name, view.case_expr().view_VarName().name()) 
			<< "(" << prop_type << "))\n";	
		}
		PushBlock(view.case_body(), to, num_tabs);
	}

	void CGenerator::PushEnumCaseClause(const std::string& enum_expr, const Statement& case_clause, std::ostream& to, size_t num_tabs, size_t order) noexcept
	{
		const auto& view = case_clause.view_CaseClause();

		if (order == 0)
		{
			to << std::string(num_tabs, '\t') << "if (";
		}
		else if (order > 0)
		{
			to << std::string(num_tabs, '\t') << "else if (";
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
		PushBlock(view.case_body(), to, num_tabs);
	}

	void CGenerator::PushMatch(const Statement& match, std::ostream& to, size_t num_tabs) noexcept
	{
		const auto& view = match.view_Match();
		auto match_expr_info = info.GetNextExpr();
		info.PopExpr();

		for (size_t i = 0; i < view.size(); i++)
		{
			if (match_expr_info.expr_type.typeclass == FieldType::Type::Property) {
				PushPropertyCaseClause(match_expr_info.expr_type.name, match_expr_info.valid_expr, view.cases(i), to, num_tabs, i);
			}
			else {
				PushEnumCaseClause(match_expr_info.valid_expr, view.cases(i), to, num_tabs, i);
			}
		}
	}

	void CGenerator::PushMakeFunction(const Statement& make_fn, std::ostream& to, size_t num_tabs) noexcept
	{
		const auto& view = make_fn.view_MakeFunction();
		
		if (IsPrimitive(view.ret_type_name()) && view.ret_type_name() != "string")
		{
			to << std::string(num_tabs, '\t') << view.ret_type_name() << ' ' << view.fn_name() << '(';
		}
		else {
			to << std::string(num_tabs, '\t') << view.ret_type_name() << "* " << view.fn_name() << '(';
		}

		for (size_t i = 0; i < view.size(); i++)
		{
			const auto& param = view.params(i).view_FnArgs();

			if (i > 0) { to << ", "; }

			if (param.is_array())
			{
				to << "/* Skipping for now */";
			}
			else if ((IsPrimitive(param.type_name()) && param.type_name() != "string") 
				|| info.GetSymbolType(param.var_name()).typeclass == FieldType::Type::Enum)
			{
				to << param.type_name() << ' ' << param.var_name();
			}
			else {
				to << param.type_name() << "* " << param.var_name();
			}
		}

		to << ")\n";
		PushBlock(view.fn_body(), to, num_tabs);
	}

	void CGenerator::PushReturn(const Statement& return_stmt, std::ostream& to, size_t num_tabs) noexcept
	{
		const auto& view = return_stmt.view_Return();
		to << std::string(num_tabs, '\t') << "return";

		if (not view.return_expr().option_is_EmptyLiteral())
		{
			to << " ";
			PushExpression(to);
		}

		to << ";\n";
	}

	void CGenerator::PushBreak(const Statement& break_stmt, std::ostream& to, size_t num_tabs) noexcept
	{
		to << std::string(num_tabs, '\t') << "break;\n";
	}

	void CGenerator::PushContinue(const Statement& continue_stmt, std::ostream& to, size_t num_tabs) noexcept
	{
		to << std::string(num_tabs, '\t') << "continue;\n";
	}

	void CGenerator::AddHeaders(std::ostream& to) noexcept
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
            MVS_RegisterNew((uintptr_t)new_ptr, 0, NULL);
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
                MVS_RegisterNew((uintptr_t)ptrs[i], 0, NULL);
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
            MVS_RegisterNew((uintptr_t)ptr, 0, NULL);
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

	CGenerator::CGenerator(CompileInfo&& info) : info(std::move(info))
	{}

	std::filesystem::path CGenerator::GenerateC_IR(const Statement& valid_AST, std::string_view val_source_path)
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
				PushMakeFunction(view.statements(i), out, 0);
				break;
			case selector::MakeStructStmt:
				PushMakeStruct(view.statements(i), out, 0);
				break;
			case selector::MakePropertyStmt:
				PushMakeProperty(view.statements(i), out, 0);
				break;
			case selector::MakeEnumStmt:
				PushMakeEnum(view.statements(i), out, 0);
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
				PushVarInitialization(view.statements(i), out, 1, alloc_ptrs);
				break;
			case selector::ArrayInitStmt:
				PushArrInitialization(view.statements(i), out, 1, alloc_ptrs);
				break;
			case selector::ConditionStmt:
				PushCondition(view.statements(i), out, 1);
				break;
			case selector::WhileLoopStmt:
				PushWhileLoop(view.statements(i), out, 1);
				break;
			case selector::ReturnStmt:
				PushReturn(view.statements(i), out, 1);
				break;
			case selector::BreakStmt:
				PushBreak(view.statements(i), out, 1);
				break;
			case selector::ContinueStmt:
				PushContinue(view.statements(i), out, 1);
				break;
			case selector::MatchStmt:
				PushMatch(view.statements(i), out, 1);
				break;
			case selector::ForLoopStmt:
				PushForLoop(view.statements(i), out, 1);
				break;
			case selector::AssignmentStmt:
				PushAssignment(view.statements(i), out, 1);
				break;
			case selector::BlockOfStmt:
				PushBlock(view.statements(i), out, 1);
				break;
			case selector::ExprCallStmt:
				PushExprCall(view.statements(i), out, 1);
				break;
			}
		}

		PushCleanup(alloc_ptrs, out, 1);
		out << "\tMVS_Destroy();\n";
		out << "}\n";

		out.close();

		std::cout << "Generated C IR at: " << c_ir_path.string() << "\n";

		return c_ir_path;
	}

	void CGenerator::PushAssignment(const Statement& assignment, std::ostream& to, size_t num_tabs) noexcept
	{
		const auto& view = assignment.view_Assignment();
		
		auto lhs_info = info.GetNextExpr();
		cur_lhs_info = lhs_info;
		info.PopExpr();
		auto rhs_info = info.GetNextExpr();
		cur_rhs_info = rhs_info;
		info.PopExpr();

		if (lhs_info.valid_expr.back() == ']' && lhs_info.expr_type.name == "char")
		{
			std::string index_expr = [](const std::string& str) -> std::string {
				int depth = 1;
				for (int i = (int)str.size() - 2; i >= 0; i--)
				{
					if (str[i] == ']') { depth++; }
					else if (str[i] == '[') { depth--; }
					if (depth == 0) { return str.substr(i + 1, str.size() - i - 2); }
				}
				return "null";
			}(lhs_info.valid_expr);
			std::string array_expr = lhs_info.valid_expr.substr(0, lhs_info.valid_expr.find_last_of('['));
			to << std::string(num_tabs, '\t') << "if (MVS_RefCount((uintptr_t)" << array_expr << ") > 1)\n";
			to << std::string(num_tabs, '\t') << "{\n";
			to << std::string(num_tabs + 1, '\t') << "string* clone = xx_clone_string(" << array_expr << ");\n";
			to << std::string(num_tabs + 1, '\t') << "MVS_DetachPointer((uintptr_t)" << array_expr << ");\n";
			to << std::string(num_tabs + 1, '\t') << array_expr << " = clone;\n";
			to << std::string(num_tabs, '\t') << "}\n";
			to << std::string(num_tabs, '\t') << "xx_string_set(" << array_expr << ", " << index_expr << ", " << rhs_info.valid_expr << ");\n";
		}
		else if (lhs_info.valid_expr.back() == ']')
		{

		}
		else if (lhs_info.expr_type.typeclass == FieldType::Type::Primitive || lhs_info.expr_type.typeclass == FieldType::Type::Enum)
		{
			to << std::string(num_tabs, '\t') << lhs_info.valid_expr << " = " << rhs_info.valid_expr << ";\n";
		}
		else if (not view.dest().option_is_FieldCall()) 
		{
			to << std::string(num_tabs, '\t') << "MVS_DetachPointer((uintptr_t)" << rhs_info.valid_expr << ");\n";
			if (lhs_info.expr_type.typeclass == FieldType::Type::Property && rhs_info.expr_type.typeclass == FieldType::Type::Struct)
			{
				to << std::string(num_tabs, '\t') << lhs_info.valid_expr << " = xx_init_" <<
					lhs_info.expr_type.name << '_' << rhs_info.expr_type.name << '(' << rhs_info.valid_expr << "); \n";
			}
			else {
				to << std::string(num_tabs, '\t') << lhs_info.valid_expr << " = " << rhs_info.valid_expr << ";\n";
			}
			to << std::string(num_tabs, '\t') << "MVS_RegisterNew((uintptr_t)" << rhs_info.valid_expr << ", sizeof(" << rhs_info.expr_type.name << "), " << __FREE_FN_NAME(rhs_info.expr_type.name) << ");\n";
		}
		else {
			PushFieldsVector(lhs_info.valid_expr);
			initial_assign_expr_str = rhs_info.valid_expr;
			PushFieldRecurse(assignment.view_Assignment().dest(), to, num_tabs, field_order.size() - 2);
			initial_assign_expr_str = "";
			field_order.clear();

			to << std::string(num_tabs, '\t') << "else { " << lhs_info.valid_expr << " = " << rhs_info.valid_expr << "; }\n";
		}
	}

	void CGenerator::PushExprCall(const Statement& expr_call, std::ostream& to, size_t num_tabs) noexcept
	{
		if (expr_call.view_ExprCall().expr().option_is_FnCall()) 
		{
			to << std::string(num_tabs, '\t');
			PushExpression(to);
			to << ";\n";
		}
	}

	void CGenerator::PushBlock(const Statement& block, std::ostream& to, size_t num_tabs, std::vector <std::string> allocated) noexcept
	{
		const auto& view = block.view_Block();
		to << std::string(num_tabs, '\t') << "{\n";

		std::vector <std::string> alloc_ptrs;
		alloc_ptrs.reserve(allocated.size());
		for (size_t i = 0; i < allocated.size(); i++)
		{
			alloc_ptrs.push_back(allocated[i]);
		}

		for (size_t i = 0; i < view.size(); i++)
		{
			switch (view.statements(i).sel())
			{
			case selector::VarInitStmt:
				PushVarInitialization(view.statements(i), to, num_tabs + 1, alloc_ptrs);
				break;
			case selector::ArrayInitStmt:
				PushArrInitialization(view.statements(i), to, num_tabs + 1, alloc_ptrs);
				break;
			case selector::ConditionStmt:
				PushCondition(view.statements(i), to, num_tabs + 1);
				break;
			case selector::WhileLoopStmt:
				PushWhileLoop(view.statements(i), to, num_tabs + 1);
				break;
			case selector::ReturnStmt:
			{
				if (info.GetNextExpr().is_lvalue) {}
				PushCleanup(alloc_ptrs, to, num_tabs + 1);
				PushReturn(view.statements(i), to, num_tabs + 1);
				to << std::string(num_tabs, '\t') << "}\n";
				return;
			}
			case selector::BreakStmt:
				PushBreak(view.statements(i), to, num_tabs + 1);
				break;
			case selector::ContinueStmt:
				PushContinue(view.statements(i), to, num_tabs + 1);
				break;
			case selector::MatchStmt:
				PushMatch(view.statements(i), to, num_tabs + 1);
				break;
			case selector::ForLoopStmt:
				PushForLoop(view.statements(i), to, num_tabs + 1);
				break;
			case selector::AssignmentStmt:
				PushAssignment(view.statements(i), to, num_tabs + 1);
				break;
			case selector::BlockOfStmt:
				PushBlock(view.statements(i), to, num_tabs + 1);
				break;
			case selector::ExprCallStmt:
				PushExprCall(view.statements(i), to, num_tabs + 1);
				break;
			}
		}

		PushCleanup(alloc_ptrs, to, num_tabs + 1);
		to << std::string(num_tabs, '\t') << "}\n";
	}

	void CGenerator::PushCondition(const Statement& condition, std::ostream& to, size_t num_tabs) noexcept
	{
		const auto& view = condition.view_Condition();

		to << std::string(num_tabs, '\t') << "if (";
		PushExpression(to);
		to << ")\n";

		PushBlock(view.if_body(), to, num_tabs);

		for (size_t i = 0; i < view.size(); i++)
		{
			to << std::string(num_tabs, '\t') << "else if (";
			PushExpression(to);
			to << ")\n";

			PushBlock(view.elif_stmt(i).view_ElifCondition().elif_body(), to, num_tabs);
		}

		if (not view.else_body().option_is_Empty())
		{
			to << std::string(num_tabs, '\t') << "else\n";
			PushBlock(view.else_body(), to, num_tabs);
		}
	}

	void CGenerator::PushForLoop(const Statement& for_loop, std::ostream& to, size_t num_tabs) noexcept
	{
		const auto& view = for_loop.view_ForLoop();

		to << std::string(num_tabs, '\t') << "{\n";

		std::vector <std::string> allocated;

		for (size_t i = 0; i < view.init_part().view_Block().size(); i++)
		{
			if (view.init_part().view_Block().statements(i).option_is_ArrayInit())
			{
				PushArrInitialization(view.init_part().view_Block().statements(i), to, num_tabs + 1, allocated);
			}
			else
			{
				PushVarInitialization(view.init_part().view_Block().statements(i), to, num_tabs + 1, allocated);
			}
		}

		to << std::string(num_tabs + 1, '\t') << "while(1)\n";
		to << std::string(num_tabs + 1, '\t') << "{\n";

		to << std::string(num_tabs + 2, '\t') << "if (!(";
		PushExpression(to);
		to << ")) { break; }\n";

		std::ostringstream increment_block;
		PushBlock(view.final_expr(), increment_block, num_tabs + 2);

		PushBlock(view.forloop_body(), to, num_tabs + 2, allocated);
		to << increment_block.str();

		to << std::string(num_tabs + 1, '\t') << "}\n";
		to << std::string(num_tabs, '\t') << "}\n";
	}

	void CGenerator::PushWhileLoop(const Statement& while_loop, std::ostream& to, size_t num_tabs) noexcept
	{
		const auto& view = while_loop.view_WhileLoop();
		
		to << std::string(num_tabs, '\t') << "while (";
		PushExpression(to);
		to << ")\n";

		PushBlock(view.whileloop_body(), to, num_tabs);
	}

	void CGenerator::PushStructDecl(const Statement& make_struct, std::ostream& to, size_t num_tabs) noexcept
	{
		const auto& view = make_struct.view_MakeStruct();
		to << std::string(num_tabs, '\t') << "typedef struct " << view.struct_name();

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
				if (info.GetADTFieldInfo(view.struct_name()).at(view.inits(i).view_VarInit().var_name()).typeclass == FieldType::Type::Primitive ||
					info.GetADTFieldInfo(view.struct_name()).at(view.inits(i).view_VarInit().var_name()).typeclass == FieldType::Type::Enum)
				{
					to << std::string(num_tabs + 1, '\t') << view.inits(i).view_VarInit().type_name() << " " << view.inits(i).view_VarInit().var_name() << ";\n";
				}
				else if (info.GetADTFieldInfo(view.struct_name()).at(view.inits(i).view_VarInit().var_name()).typeclass == FieldType::Type::Struct
					|| info.GetADTFieldInfo(view.struct_name()).at(view.inits(i).view_VarInit().var_name()).typeclass == FieldType::Type::Property)
				{
					if (view.inits(i).view_VarInit().type_name() == view.struct_name())
					{
						to << std::string(num_tabs + 1, '\t') << "struct " << view.struct_name() << "* " << view.inits(i).view_VarInit().var_name() << ";\n";
					}
					else {
						to << std::string(num_tabs + 1, '\t') << view.inits(i).view_VarInit().type_name() << "* " << view.inits(i).view_VarInit().var_name() << ";\n";
					}
				}
				else
				{
					to << std::string(num_tabs + 1, '\t') << "string* " << view.inits(i).view_VarInit().var_name() << ";\n";
				}
			}
		}

		to << std::string(num_tabs, '\t') << "} " << view.struct_name() << ";\n\n";
	}

	void CGenerator::PushStructDeleter(const Statement& make_struct, std::ostream & to, size_t num_tabs) noexcept
	{
		const auto& view = make_struct.view_MakeStruct();

		to << std::string(num_tabs, '\t') << "void " << __FREE_FN_NAME(view.struct_name()) << "(uintptr_t address)\n";
		to << std::string(num_tabs, '\t') << "{\n";

		std::string deleter_pointer = "(" + (std::string)__DELETER_POINTER(view.struct_name()) + ")";

		for (size_t i = 0; i < view.size(); i++)
		{
			if (view.inits(i).option_is_VarInit())
			{
				if (info.GetADTFieldInfo(view.struct_name()).at(view.inits(i).view_VarInit().var_name()).typeclass == FieldType::Type::Struct
					|| info.GetADTFieldInfo(view.struct_name()).at(view.inits(i).view_VarInit().var_name()).typeclass == FieldType::Type::Property
					|| info.GetADTFieldInfo(view.struct_name()).at(view.inits(i).view_VarInit().var_name()).typeclass == FieldType::Type::String)
				{
					to << std::string(num_tabs + 1, '\t') << "if (" << deleter_pointer << "->" << view.inits(i).view_VarInit().var_name() << " != NULL)\n";
					to << std::string(num_tabs + 1, '\t') << "{\n";
					to << std::string(num_tabs + 2, '\t') << "MVS_DetachPointer(" << __AS_UINTPTR_T(deleter_pointer) << "->" << view.inits(i).view_VarInit().var_name() << ");\n";
					to << std::string(num_tabs + 1, '\t') << "}\n";
				}
			}
		}

		if (view.size() != 0) {
			to << std::string(num_tabs + 1, '\t') << "free(" << deleter_pointer << ");\n";
		}
		to << std::string(num_tabs, '\t') << "}\n\n";
	}

	void CGenerator::PushStructInit(const Statement& make_struct, std::ostream & to, size_t num_tabs) noexcept
	{
		const auto& view = make_struct.view_MakeStruct();
		to << std::string(num_tabs, '\t') << view.struct_name() << "* " << __INIT_FN_NAME(view.struct_name()) << "(";

		if (view.size() == 0)
		{
			to << ")\n" << std::string(num_tabs, '\t') << "{\n";
			to << std::string(num_tabs + 1, '\t') << "return NULL;\n";
			to << std::string(num_tabs, '\t') << "}\n\n";
			return;
		}

		int i = 0;
		for (auto& [var_name, type_info] : info.GetADTFieldInfo(view.struct_name()))
		{
			if (i > 0)
			{
				to << ", ";
			}

			if (type_info.typeclass == FieldType::Type::Primitive || type_info.typeclass == FieldType::Type::Enum)
			{
				to << type_info.name << " " << var_name;
			}
			else if (type_info.typeclass == FieldType::Type::Struct || type_info.typeclass == FieldType::Type::Property)
			{
				to << type_info.name << "* " << var_name;
			}
			else {
				to << "string* " << var_name;
			}

			i++;
		}

		to << ") \n";
		to << std::string(num_tabs, '\t') << "{\n";

		to << std::string(num_tabs + 1, '\t') << view.struct_name() << " *xx_" << view.struct_name()
			<< "_init_ptr = malloc(sizeof(" << view.struct_name() << "));\n";

		for (auto& [var_name, type_info] : info.GetADTFieldInfo(view.struct_name()))
		{
			if (type_info.typeclass == FieldType::Type::Primitive || type_info.typeclass == FieldType::Type::Enum)
			{
				to << std::string(num_tabs + 1, '\t') << "xx_" << view.struct_name() << "_init_ptr->" <<
					var_name << " = " << var_name << ";\n";
			}
			else
			{
				to << std::string(num_tabs + 1, '\t') << "xx_" << view.struct_name() << "_init_ptr->" <<
					var_name << " = " << var_name << ";\n";
				to << std::string(num_tabs + 1, '\t') << "if (" << var_name << " != NULL) {\n";
				to << std::string(num_tabs + 2, '\t') << "MVS_RegisterNew((uintptr_t)"
					<< var_name << ", sizeof(" << type_info.name << "), "
					<< __FREE_FN_NAME(type_info.name) << ");\n";
				to << std::string(num_tabs + 1, '\t') << "}\n";
			}
		}

		to << std::string(num_tabs + 1, '\t') << "return xx_" << view.struct_name() << "_init_ptr;\n";
		to << std::string(num_tabs, '\t') << "}\n\n";
	}

	void CGenerator::PushStructClone(const Statement& make_struct, std::ostream& to, size_t num_tabs) noexcept
	{
		const auto& view = make_struct.view_MakeStruct();
		to << std::string(num_tabs, '\t') << view.struct_name() << "* " << "xx_clone_" << view.struct_name() << "(" << view.struct_name() << "* ptr) \n";
		to << std::string(num_tabs, '\t') << "{\n";

		to << std::string(num_tabs + 1, '\t') << "if (ptr == NULL) { return NULL; }\n";
		to << std::string(num_tabs + 1, '\t') << view.struct_name() << "* clone = xx_init_" << view.struct_name() << "(";

		for (size_t i = 0; i < view.size(); i++)
		{
			if (i > 0) { to << ", "; }
			std::string param_name = view.inits(i).option_is_VarInit() ? 
				view.inits(i).view_VarInit().var_name() : view.inits(i).view_ArrayInit().type_info().view_VarInit().var_name();

			if (info.GetADTFieldInfo(view.struct_name()).at(param_name).typeclass == FieldType::Type::Struct ||
				info.GetADTFieldInfo(view.struct_name()).at(param_name).typeclass == FieldType::Type::Property)
			{
				to << "xx_clone_" << info.GetADTFieldInfo(view.struct_name()).at(param_name).name << "(ptr->" << param_name << ")";
			}
			else if (info.GetADTFieldInfo(view.struct_name()).at(param_name).typeclass == FieldType::Type::String)
			{
				to << "xx_clone_string(ptr->" << param_name << ")";
			}
			else {
				to << "ptr->" << param_name;
			}
		}
		to << ");\n";
		to << std::string(num_tabs + 1, '\t') << "MVS_RegisterNew((uintptr_t)clone, sizeof(" << view.struct_name() << "), " << __FREE_FN_NAME(view.struct_name()) << ");\n";
		to << std::string(num_tabs + 1, '\t') << "return clone;\n";
		to << std::string(num_tabs, '\t') << "}\n\n";
	}

	void CGenerator::PushMakeStruct(const Statement& make_struct, std::ostream& to, size_t num_tabs) noexcept
	{
		const auto& view = make_struct.view_MakeStruct();

		PushStructDecl(make_struct, to, num_tabs);
		PushStructDeleter(make_struct, to, num_tabs);
		PushStructInit(make_struct, to, num_tabs);
		PushStructClone(make_struct, to, num_tabs);
	}

	void CGenerator::PushMakeEnum(const Statement & make_enum, std::ostream & to, size_t num_tabs) noexcept
	{
		const auto& view = make_enum.view_MakeEnum();

		to << std::string(num_tabs, '\t') << "typedef enum " << view.enum_name() << " {\n";

		for (size_t i = 0; i < view.size(); i++)
		{
			to << std::string(num_tabs + 1, '\t') << view.enum_variants(i) << ", ";
		}
		to << '\n' << std::string(num_tabs, '\t') << "} " << view.enum_name() << ";\n";
	}

	void CGenerator::PushForwardDeclProperty(const std::string& prop_name, std::ostream& to, size_t num_tabs) noexcept
	{
		to << std::string(num_tabs, '\t') << "typedef struct " << prop_name << " " << prop_name << ";\n";
		to << std::string(num_tabs, '\t') << "void " << __FREE_FN_NAME(prop_name) << "(uintptr_t address);\n\n";
	}

	void CGenerator::PushTagEnumProperty(const Statement& make_property, std::ostream& to, size_t num_tabs) noexcept
	{
		const auto& view = make_property.view_MakeProperty();
		to << std::string(num_tabs, '\t') << "typedef enum " << view.prop_name() << "_Tag {\n";
		to << std::string(num_tabs + 1, '\t');
		for (size_t i = 0; i < view.size(); i++)
		{
			to << view.opts(i).view_MakeStruct().struct_name() << "_tag, ";
		}
		to << '\n' << std::string(num_tabs, '\t') << "} " << view.prop_name() << "_Tag;\n\n";
	}

	void CGenerator::PushOptionStructProperty(const Statement& make_property, std::ostream& to, size_t num_tabs) noexcept
	{
		const auto& view = make_property.view_MakeProperty();
		to << std::string(num_tabs, '\t') << "typedef union " << view.prop_name() << "_Opts {\n";

		for (size_t i = 0; i < view.size(); i++)
		{
			to << std::string(num_tabs + 1, '\t') << view.opts(i).view_MakeStruct().struct_name() << "* " << view.opts(i).view_MakeStruct().struct_name() << "_opt;\n";
		}

		to << std::string(num_tabs, '\t') << "} " << view.prop_name() << "_Opts;\n\n";
	}

	void CGenerator::PushPropertyDecl(const Statement& make_property, std::ostream& to, size_t num_tabs) noexcept
	{
		const auto& view = make_property.view_MakeProperty();
		to << std::string(num_tabs, '\t') << "typedef struct " << view.prop_name() << " {\n";
		to << std::string(num_tabs + 1, '\t') << view.prop_name() << "_Tag tag;\n";
		to << std::string(num_tabs + 1, '\t') << view.prop_name() << "_Opts opts;\n";

		to << std::string(num_tabs, '\t') << "} " << view.prop_name() << ";\n\n";
	}

	void CGenerator::PushPropertyDeleter(const Statement& make_property, std::ostream& to, size_t num_tabs) noexcept
	{
		const auto& view = make_property.view_MakeProperty();
		to << std::string(num_tabs, '\t') << "void " << __FREE_FN_NAME(view.prop_name()) << "(uintptr_t address)\n";
		to << std::string(num_tabs, '\t') << "{\n";

		std::string deleter_pointer = "(" + (std::string)__DELETER_POINTER(view.prop_name()) + ")";

		for (size_t i = 0; i < view.size(); i++)
		{
			to << std::string(num_tabs + 1, '\t') <<
				"if ((" << deleter_pointer << "->tag == " << __AS_TAG(view.opts(i).view_MakeStruct().struct_name()) << ") && (" <<
				deleter_pointer << "->opts." << view.opts(i).view_MakeStruct().struct_name() << "_opt != NULL)) \n";
			to << std::string(num_tabs + 1, '\t') << "{\n";
			to << std::string(num_tabs + 2, '\t') << "MVS_DetachPointer(" << __AS_UINTPTR_T(deleter_pointer) << "->opts." << view.opts(i).view_MakeStruct().struct_name() << "_opt);\n";
			to << std::string(num_tabs + 1, '\t') << "}\n";
		}
		to << std::string(num_tabs + 1, '\t') << "free(" << deleter_pointer << ");\n";
		to << std::string(num_tabs, '\t') << "}\n\n";
	}

	void CGenerator::PushPropertyInit(const std::string& prop_name, const std::string& opt_name, std::ostream& to, size_t num_tabs) noexcept
	{
		to << std::string(num_tabs, '\t') << prop_name << "* " << __INIT_PROP_OPT(prop_name, opt_name) <<
			'(' << opt_name << "* " << __AS_OPT(opt_name) << ")\n";
		to << std::string(num_tabs, '\t') << "{\n";

		to << std::string(num_tabs + 1, '\t') << prop_name << "* xx_init_" << prop_name << "_ptr = malloc(sizeof(" << prop_name << "));\n";
		to << std::string(num_tabs + 1, '\t') << "xx_init_" << prop_name << "_ptr->tag = " << __AS_TAG(opt_name) << ";\n";
		to << std::string(num_tabs + 1, '\t') << "xx_init_" << prop_name << "_ptr->opts." << __AS_OPT(opt_name) << " = " << __AS_OPT(opt_name) << ";\n";

		to << std::string(num_tabs + 1, '\t') << "if (" << __AS_OPT(opt_name) << " != NULL)\n";
		to << std::string(num_tabs + 1, '\t') << "{\n";
		to << std::string(num_tabs + 2, '\t') << "MVS_RegisterNew((uintptr_t)"
			<< __AS_OPT(opt_name) << ", sizeof(" << opt_name << "), " << __FREE_FN_NAME(opt_name) << ");\n";
		to << std::string(num_tabs + 1, '\t') << "}\n";
		to << std::string(num_tabs + 1, '\t') << "return xx_init_" << prop_name << "_ptr;\n";
		to << std::string(num_tabs, '\t') << "}\n\n";
	}

	void CGenerator::PushPropertyNULLInit(const std::string& prop_name, const std::string& opt_name, std::ostream& to, size_t num_tabs) noexcept
	{
		to << std::string(num_tabs, '\t') << prop_name << "* " << __INIT_PROP_OPT(prop_name, opt_name) <<
			'(' << opt_name << "* " << __AS_OPT(opt_name) << ")\n";
		to << std::string(num_tabs, '\t') << "{\n";

		to << std::string(num_tabs + 1, '\t') << prop_name << "* xx_init_" << prop_name << "_ptr = malloc(sizeof(" << prop_name << "));\n";
		to << std::string(num_tabs + 1, '\t') << "xx_init_" << prop_name << "_ptr->tag = " << __AS_TAG(opt_name) << ";\n";
		to << std::string(num_tabs + 1, '\t') << "xx_init_" << prop_name << "_ptr->opts." << __AS_OPT(opt_name) << " = " << __AS_OPT(opt_name) << ";\n";

		to << std::string(num_tabs + 1, '\t') << "return xx_init_" << prop_name << "_ptr;\n";
		to << std::string(num_tabs, '\t') << "}\n\n";
	}

	void CGenerator::PushPropertyClone(const Statement& make_property, std::ostream& to, size_t num_tabs) noexcept
	{
		const auto& view = make_property.view_MakeProperty();

		to << std::string(num_tabs, '\t') << view.prop_name() << "* " << "xx_clone_" << view.prop_name() << "(" << view.prop_name() << "* ptr) \n";
		to << std::string(num_tabs, '\t') << "{\n";
		to << std::string(num_tabs + 1, '\t') << "if (ptr == NULL) { return NULL; }\n";
		to << std::string(num_tabs + 1, '\t') << view.prop_name() << "* clone = malloc(sizeof(" << view.prop_name() << "));\n";
		to << std::string(num_tabs + 1, '\t') << "clone->tag = ptr->tag;\n";
		to << std::string(num_tabs + 1, '\t') << "switch (ptr->tag)\n";
		to << std::string(num_tabs + 1, '\t') << "{\n";
		
		for (size_t i = 0; i < view.size(); i++)
		{
			std::string opt_name = view.opts(i).view_MakeStruct().struct_name();
			to << std::string(num_tabs + 1, '\t') << "case " << __AS_TAG(opt_name) << ":\n";
			to << std::string(num_tabs + 2, '\t') << "clone->opts." << __AS_OPT(opt_name) << " = xx_clone_" << opt_name
				<< "(ptr->opts." << __AS_OPT(opt_name) << ");\n";
			to << std::string(num_tabs + 2, '\t') << "break;\n";
		}
		
		to << std::string(num_tabs + 1, '\t') << "}\n";
		to << std::string(num_tabs + 1, '\t') << "MVS_RegisterNew((uintptr_t)clone, sizeof(" << view.prop_name() << "), " <<
			__FREE_FN_NAME(view.prop_name()) << ");\n";
		to << std::string(num_tabs + 1, '\t') << "return clone;\n";
		to << std::string(num_tabs, '\t') << "}\n\n";
	}

	void CGenerator::PushMakeProperty(const Statement & make_property, std::ostream & to, size_t num_tabs) noexcept
	{
		const auto& view = make_property.view_MakeProperty();

		PushForwardDeclProperty(view.prop_name(), to, num_tabs);

		for (size_t i = 0; i < view.size(); i++)
		{
			PushStructDecl(view.opts(i), to, num_tabs);
		}

		PushTagEnumProperty(make_property, to, num_tabs);
		PushOptionStructProperty(make_property, to, num_tabs);
		PushPropertyDecl(make_property, to, num_tabs);

		auto OptionCheckCreate = [&to, &view, &num_tabs](const std::string& IR_opt_name, size_t i) {
			to << std::string(num_tabs, '\t') << "bool " << __IS_OPTION_CHECK_FN_NAME(view.prop_name(), IR_opt_name) << "(const " << view.prop_name() << "* prop)\n";
			to << std::string(num_tabs, '\t') << "{\n";
			to << std::string(num_tabs + 1, '\t') << "return prop->tag == " << view.opts(i).view_MakeStruct().struct_name() << "_tag;\n";
			to << std::string(num_tabs, '\t') << "}\n\n";
		};

		for (size_t i = 0; i < view.size(); i++)
		{
			OptionCheckCreate(view.opts(i).view_MakeStruct().struct_name(), i);
		}

		PushPropertyDeleter(make_property, to, num_tabs);

		for (size_t i = 0; i < view.size(); i++)
		{
			PushStructDeleter(view.opts(i), to, num_tabs);
			PushStructInit(view.opts(i), to, num_tabs);
			PushStructClone(view.opts(i), to, num_tabs);
		}

		for (size_t i = 0; i < view.size(); i++)
		{
			if (view.opts(i).view_MakeStruct().size() == 0)
			{
				PushPropertyNULLInit(view.prop_name(), view.opts(i).view_MakeStruct().struct_name(), to, num_tabs);
			}
			else {
				PushPropertyInit(view.prop_name(), view.opts(i).view_MakeStruct().struct_name(), to, num_tabs);
			}
		}

		PushPropertyClone(make_property, to, num_tabs);
	}

	void CGenerator::PushExpression(std::ostream& to) noexcept
	{
		to << info.GetNextExpr().valid_expr;
		info.PopExpr();
	}

	void CGenerator::PushVarInitialization(const Statement& var_init, std::ostream& to, size_t num_tabs, std::vector <std::string>& alloc_ptrs) noexcept
	{
		const auto& view = var_init.view_VarInit();

		if (info.GetSymbolType(view.var_name()).typeclass == FieldType::Type::Primitive)
		{
			to << std::string(num_tabs, '\t') << view.type_name() << ' ' << view.var_name();
			if (not view.init_expr().option_is_EmptyLiteral())
			{
				to << " = ";
				PushExpression(to);
			}
			to << ";\n";
		}
		else if (info.GetSymbolType(view.var_name()).typeclass == FieldType::Type::Enum)
		{
			to << std::string(num_tabs, '\t') << view.type_name() << " " << view.var_name();
			if (not view.init_expr().option_is_EmptyLiteral())
			{
				to << " = ";
				PushExpression(to);
			}
			to << ";\n";
		}
		else if (info.GetSymbolType(view.var_name()).typeclass == FieldType::Type::Struct) 
		{
			alloc_ptrs.push_back(view.var_name());
			to << std::string(num_tabs, '\t') << view.type_name() << "* " << view.var_name() << " = ";
			if (not view.init_expr().option_is_EmptyLiteral()) 
			{
				PushExpression(to);
				to << ";\n";
				to << std::string(num_tabs, '\t') 
					<< "MVS_RegisterNew((uintptr_t)" << view.var_name()
					<< ", sizeof(" << view.type_name() << "), "
					<< __FREE_FN_NAME(view.type_name()) << ");\n";
			}
			else 
			{
				to << "NULL;\n";
			}
		}
		else if (info.GetSymbolType(view.var_name()).typeclass == FieldType::Type::Property) 
		{
			alloc_ptrs.push_back(view.var_name());
			to << std::string(num_tabs, '\t') << view.type_name() << "* " << view.var_name() << " = ";
			if (view.init_expr().option_is_StructInit())
			{
				to << "xx_init_" << view.type_name() << "_" << view.init_expr().view_StructInit().struct_name() << '(';
				PushExpression(to);
				to << ");\n";
				to << std::string(num_tabs, '\t')
					<< "MVS_RegisterNew((uintptr_t)" << view.var_name()
					<< ", sizeof(" << view.type_name() << "), "
					<< __FREE_FN_NAME(view.type_name()) << ");\n";
			}
			else if (not view.init_expr().option_is_EmptyLiteral())
			{
				PushExpression(to);
				to << ";\n";
				to << std::string(num_tabs, '\t')
					<< "MVS_RegisterNew((uintptr_t)" << view.var_name()
					<< ", sizeof(" << view.type_name() << "), "
					<< __FREE_FN_NAME(view.type_name()) << ");\n";
			}
			else {
				to << "NULL;\n";
			}
		}
		else {
			alloc_ptrs.push_back(view.var_name());
			to << std::string(num_tabs, '\t') << "string* " << view.var_name() << " = ";
			if (not view.init_expr().option_is_EmptyLiteral()) 
			{
				PushExpression(to);
				to << ";\n";
			}
			else {
				to << "xx_init_string(\"\");\n";
			}
			to << std::string(num_tabs, '\t') << "MVS_RegisterNew((uintptr_t)" << view.var_name()
				<< ", sizeof(string), xx_free_string);\n";
		}
	}

	void CGenerator::PushArrInitialization(const Statement& arr_init, std::ostream& to, size_t num_tabs, std::vector <std::string>& alloc_ptrs) noexcept
	{
		const auto& view = arr_init.view_ArrayInit();
		alloc_ptrs.push_back(view.type_info().view_VarInit().var_name());
		
		if (info.GetSymbolType(view.type_info().view_VarInit().var_name()).typeclass == FieldType::Type::ArrayPrimitive)
		{
			std::string of_type = info.GetSymbolType(view.type_info().view_VarInit().var_name()).name;
			to << std::string(num_tabs, '\t') << "array* " << view.type_info().view_VarInit().var_name() << " = " <<
				"xx_init_array((" << of_type << "[])" << info.GetNextExpr().valid_expr;
			std::string expr = info.GetNextExpr().valid_expr;
			int size = std::count(expr.begin(), expr.end(), ',');
			to << ", " << size << ", sizeof(" << of_type << "), 0, NULL);\n";
			to << std::string(num_tabs, '\t') << "MVS_RegisterNew((uintptr_t)" << view.type_info().view_VarInit().var_name()
				<< ", sizeof(array), xx_free_array);\n";
			info.PopExpr();
		}
		else if (info.GetSymbolType(view.type_info().view_VarInit().var_name()).typeclass == FieldType::Type::ArrayEnum)
		{
			std::string of_type = info.GetSymbolType(view.type_info().view_VarInit().var_name()).name;
			to << std::string(num_tabs, '\t') << "array* " << view.type_info().view_VarInit().var_name() << " = " <<
				"xx_init_array((" << of_type << "[])" << info.GetNextExpr().valid_expr;
			std::string expr = info.GetNextExpr().valid_expr;
			int size = std::count(expr.begin(), expr.end(), ',');
			to << ", " << size << ", sizeof(" << of_type << "), 0);\n";
			to << std::string(num_tabs, '\t') << "MVS_RegisterNew((uintptr_t)" << view.type_info().view_VarInit().var_name()
				<< ", sizeof(array), xx_free_array);\n";
			info.PopExpr();
		}
		else
		{
			to << std::string(num_tabs, '\t') << "array* " << view.type_info().view_VarInit().var_name() << " = " <<
				"xx_init_array((" << info.GetNextExpr().expr_type.name << "* [])" << info.GetNextExpr().valid_expr;
			std::string expr = info.GetNextExpr().valid_expr;
			int size = std::count(expr.begin(), expr.end(), ',');
			to << ", " << size << ", sizeof(uintptr_t), 1, xx_free_" << info.GetSymbolType(view.type_info().view_VarInit().var_name()).name << "); \n";
			to << std::string(num_tabs, '\t') << "MVS_RegisterNew((uintptr_t)" << view.type_info().view_VarInit().var_name()
				<< ", sizeof(array), xx_free_array);\n";
			info.PopExpr();
		}
	}

	void CGenerator::PushCleanup(const std::vector<std::string>& alloc_ptrs, std::ostream& to, size_t num_tabs) noexcept
	{
		for (const auto& ptr_name : alloc_ptrs)
		{
			to << std::string(num_tabs, '\t') << "MVS_DetachPointer((uintptr_t)" << ptr_name << ");\n";
		}
	}

	void CGenerator::PushFieldsVector(const std::string& field_call_str) noexcept
	{
		auto is_open_bracket = [](char c) {
			return c == '(' || c == '[' || c == '{';
		};
		auto is_close_bracket = [](char c) {
			return c == ')' || c == ']' || c == '}';
		};
		auto matching_open = [](char c) {
			switch (c) {
			case ')': return '(';
			case ']': return '[';
			case '}': return '{';
			default: return '\0';
			}
		};


		std::string current;
		std::vector<char> stack; // keep track of open brackets

		for (size_t i = 0; i < field_call_str.size(); ++i) {
			if (is_open_bracket(field_call_str[i])) {
				stack.push_back(field_call_str[i]);
				current += field_call_str[i];
			}
			else if (is_close_bracket(field_call_str[i])) {
				if (!stack.empty() && stack.back() == matching_open(field_call_str[i])) {
					stack.pop_back();
				}
				current += field_call_str[i];
			}
			else if (field_call_str[i] == '-' && i + 1 < field_call_str.size() && field_call_str[i + 1] == '>' && stack.empty()) {
				// found '->' outside brackets
				field_order.push_back(current);
				current.clear();
				++i; // skip '>'
			}
			else {
				current += field_call_str[i];
			}
		}

		if (!current.empty()) {
			field_order.push_back(current);
		}
	}

	std::string CGenerator::PushFieldRecurse(const Expression& caller, std::ostream& to, size_t num_tabs, size_t i) noexcept
	{
		if (caller.option_is_VarName()) 
		{ 
			return info.GetSymbolType(caller.view_VarName().name()).name; 
		}
		if (caller.option_is_ArrayIndex())
		{
			return PushFieldRecurse(caller.view_ArrayIndex().array_expr(), to, num_tabs, i);
		}

		auto AssignTo = [this](size_t i) -> std::string {
			std::string field_call_str = "xx_clone";
			if (is_prop)
			{
				field_call_str += "->opts." + __AS_OPT(cur_option);
			}
			for (size_t j = i + 1; j < field_order.size(); j++)
			{
				field_call_str += "->" + field_order[j];
			}
			return field_call_str;
		};
		auto ParentPtr = [this](size_t i) -> std::string {
			std::string parent_ptr_str = "";
			for (size_t j = 0; j <= i; j++)
			{
				if (j > 0) { parent_ptr_str += "->"; }
				parent_ptr_str += field_order[j];
			}
			return parent_ptr_str;
		};

		std::string type_name = PushFieldRecurse(caller.view_FieldCall().caller(), to, num_tabs, i - 1);

		if (caller.view_FieldCall().caller().option_is_VarName())
		{
			to << std::string(num_tabs, '\t') << "if (MVS_RefCount((uintptr_t)" 
				<< caller.view_FieldCall().caller().view_VarName().name() << ") > 1)\n";
			to << std::string(num_tabs, '\t') << "{\n";
			to << std::string(num_tabs + 1, '\t') << type_name << "* xx_clone = xx_clone_" << type_name << "(" << caller.view_FieldCall().caller().view_VarName().name() << ");\n";
			to << std::string(num_tabs + 1, '\t') << "MVS_DetachPointer((uintptr_t)" << caller.view_FieldCall().caller().view_VarName().name() << ");\n";

			to << std::string(num_tabs + 1, '\t') << AssignTo(i) << " = " << initial_assign_expr_str << ";\n";
			to << std::string(num_tabs + 1, '\t') << caller.view_FieldCall().caller().view_VarName().name() << " = xx_clone;\n";
			to << std::string(num_tabs, '\t') << "}\n";
		}
		else if (caller.view_FieldCall().caller().option_is_ArrayIndex())
		{
			to << std::string(num_tabs, '\t') << "if (MVS_RefCount((uintptr_t)"
				<< caller.view_FieldCall().caller().view_ArrayIndex().array_expr().view_VarName().name() << ") > 1)\n";
			to << std::string(num_tabs, '\t') << "{\n";
			to << std::string(num_tabs + 1, '\t') << type_name << "* xx_clone = xx_clone_" << type_name << "(" << caller.view_FieldCall().caller().view_ArrayIndex().array_expr().view_VarName().name() << ");\n";
			to << std::string(num_tabs + 1, '\t') << "MVS_DetachPointer((uintptr_t)" << caller.view_FieldCall().caller().view_ArrayIndex().array_expr().view_VarName().name() << ");\n";
			to << std::string(num_tabs + 1, '\t') << AssignTo(i) << " = " << initial_assign_expr_str << ";\n";
			to << std::string(num_tabs + 1, '\t') << caller.view_FieldCall().caller().view_ArrayIndex().array_expr().view_VarName().name() 
				<< " = xx_clone;\n";
			to << std::string(num_tabs, '\t') << "}\n";
		}
		else if (caller.view_FieldCall().caller().option_is_FieldCall())
		{
			std::string parent_ptr_str = ParentPtr(i);
			to << std::string(num_tabs, '\t') << "if (MVS_RefCount((uintptr_t)"
				<< parent_ptr_str << ") > 1)\n";
			to << std::string(num_tabs, '\t') << "{\n";
			to << std::string(num_tabs + 1, '\t') << type_name << "* xx_clone = xx_clone_" << type_name << "(" << parent_ptr_str << ");\n";
			to << std::string(num_tabs + 1, '\t') << "MVS_DetachPointer((uintptr_t)" << parent_ptr_str << ");\n";
			to << std::string(num_tabs + 1, '\t') << AssignTo(i) << " = " << initial_assign_expr_str << ";\n";
			to << std::string(num_tabs + 1, '\t') << parent_ptr_str << " = xx_clone;\n";
			to << std::string(num_tabs, '\t') << "}\n";
		}

		if (is_prop) { type_name = cur_option; }
		is_prop = false;

		if (caller.view_FieldCall().field().option_is_ArrayIndex())
		{
			if (info.GetADTFieldInfo(type_name).at(caller.view_FieldCall().field().view_ArrayIndex().array_expr().view_VarName().name()).typeclass == FieldType::Type::String)
			{
				std::string index_expr = [](const std::string& str) -> std::string {
					int depth = 1;
					for (int i = (int)str.size() - 2; i >= 0; i--)
					{
						if (str[i] == ']') { depth++; }
						else if (str[i] == '[') { depth--; }
						if (depth == 0) { return str.substr(i + 1, str.size() - i - 2); }
					}
					return "null";
					}(cur_lhs_info.valid_expr);
				std::string array_expr = cur_lhs_info.valid_expr.substr(0, cur_lhs_info.valid_expr.find_last_of('['));
				to << std::string(num_tabs, '\t') << "if (MVS_RefCount((uintptr_t)" << array_expr << ") > 1)\n";
				to << std::string(num_tabs, '\t') << "{\n";
				to << std::string(num_tabs + 1, '\t') << "string* clone = xx_clone_string(" << array_expr << ");\n";
				to << std::string(num_tabs + 1, '\t') << "MVS_DetachPointer((uintptr_t)" << array_expr << ");\n";
				to << std::string(num_tabs + 1, '\t') << array_expr << " = clone;\n";
				to << std::string(num_tabs, '\t') << "}\n";
				to << std::string(num_tabs, '\t') << "xx_string_set(" << array_expr << ", " << index_expr << ", " << cur_rhs_info.valid_expr << ");\n";
				return "char";
			}
			return info.GetADTFieldInfo(type_name).at(caller.view_FieldCall().field().view_ArrayIndex().array_expr().view_VarName().name()).name;
		}

		return info.GetADTFieldInfo(type_name).at(caller.view_FieldCall().field().view_VarName().name()).name;
	}
}