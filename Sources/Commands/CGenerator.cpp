#include "CGenerator.h"

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
	void CGenerator::PushPropertyCaseClause(const std::string& prop_name, const std::string& prop_type, const Statement& case_clause, std::ofstream& to, size_t num_tabs) noexcept
	{
		const auto& view = case_clause.view_CaseClause();

		if (view.is_wildcard())
		{
			PushBlock(view.case_body(), to, num_tabs);
		}
		else {
			to << std::string(num_tabs, '\t') << "if (" 
				<< __IS_OPTION_CHECK_FN_NAME(prop_name, view.case_expr().view_VarName().name()) 
				<< "(" << prop_type << "))\n";
			PushBlock(view.case_body(), to, num_tabs);
		}
	}

	void CGenerator::PushEnumCaseClause(const std::string& enum_expr, const Statement& case_clause, std::ofstream& to, size_t num_tabs) noexcept
	{
		const auto& view = case_clause.view_CaseClause();

		if (view.is_wildcard())
		{
			PushBlock(view.case_body(), to, num_tabs);
		}
		else {
			to << std::string(num_tabs, '\t') << "if (" << enum_expr << " == " << view.case_expr().view_VarName().name() << ")\n";
			PushBlock(view.case_body(), to, num_tabs);
		}
	}

	void CGenerator::PushMatch(const Statement& match, std::ofstream& to, size_t num_tabs) noexcept
	{
		const auto& view = match.view_Match();
		auto match_expr_info = info.GetNextExpr();
		info.PopExpr();

		for (size_t i = 0; i < view.size(); i++)
		{
			if (match_expr_info.second.typeclass == FieldType::Type::Property) {
				PushPropertyCaseClause(match_expr_info.second.name, match_expr_info.first, view.cases(i), to, num_tabs);
			}
			else {
				PushEnumCaseClause(match_expr_info.first, view.cases(i), to, num_tabs);
			}
		}
	}

	void CGenerator::PushMakeFunction(const Statement& make_fn, std::ofstream& to, size_t num_tabs) noexcept
	{
		const auto& view = make_fn.view_MakeFunction();
		std::cout << "Skipping function\n";
	}

	void CGenerator::PushReturn(const Statement& return_stmt, std::ofstream& to, size_t num_tabs) noexcept
	{
		const auto& view = return_stmt.view_Return();
		to << std::string(num_tabs, '\t') << "return\n";

		if (not view.return_expr().option_is_EmptyLiteral())
		{
			to << " ";
			PushExpression(to);
		}

		to << ";\n";
	}

	void CGenerator::PushBreak(const Statement& break_stmt, std::ofstream& to, size_t num_tabs) noexcept
	{
		to << std::string(num_tabs, '\t') << "break;\n";
	}

	void CGenerator::PushContinue(const Statement& continue_stmt, std::ofstream& to, size_t num_tabs) noexcept
	{
		to << std::string(num_tabs, '\t') << "continue;\n";
	}

	void CGenerator::AddHeaders(std::ofstream& to) noexcept
	{
		to << R"(#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#define bool _Bool

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

bool MVS_DetachPointer(uintptr_t address)
{
    size_t index = _hash_address(address, mu.capacity);

    MVSNode* current = mu.buckets[index];
    MVSNode* prev = NULL;

    while (current)
    {
        if (current->key == address)
        {
            if (current->value.ref_count > 1)
            {
                current->value.ref_count--;
                return 0;
            }
            if (current->value.ref_count == 0)
            {
                return 0;
            }

            current->value.deleter(address);

            if (prev) { prev->next = current->next; }
            else { mu.buckets[index] = current->next; }

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
		out << "\tMVS_Init();\n\n";

		for (size_t i = 0; i < view.size(); i++)
		{
			switch (view.statements(i).sel())
			{
			case selector::VarInitStmt:
				PushVarInitialization(view.statements(i), out, 1);
				break;
			case selector::ArrayInitStmt:
				PushArrInitialization(view.statements(i), out, 1);
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

		out << "\tMVS_Destroy();\n";
		out << "}\n";

		out.close();

		std::cout << "Generated C IR at: " << c_ir_path.string() << "\n";

		return c_ir_path;
	}

	void CGenerator::PushAssignment(const Statement& assignment, std::ofstream& to, size_t num_tabs) noexcept
	{
		std::cout << "Skipping assignment\n";
	}

	void CGenerator::PushExprCall(const Statement& expr_call, std::ofstream& to, size_t num_tabs) noexcept
	{
		if (not expr_call.option_is_Empty()) 
		{
			to << std::string(num_tabs, '\t');
			PushExpression(to);
			to << ";\n";
		}
	}

	void CGenerator::PushBlock(const Statement& block, std::ofstream& to, size_t num_tabs) noexcept
	{
		const auto& view = block.view_Block();
		to << std::string(num_tabs, '\t') << "{\n";

		for (size_t i = 0; i < view.size(); i++)
		{
			switch (view.statements(i).sel())
			{
			case selector::VarInitStmt:
				PushVarInitialization(view.statements(i), to, num_tabs + 1);
				break;
			case selector::ArrayInitStmt:
				PushArrInitialization(view.statements(i), to, num_tabs + 1);
				break;
			case selector::ConditionStmt:
				PushCondition(view.statements(i), to, num_tabs + 1);
				break;
			case selector::WhileLoopStmt:
				PushWhileLoop(view.statements(i), to, num_tabs + 1);
				break;
			case selector::ReturnStmt:
				PushReturn(view.statements(i), to, num_tabs + 1);
				break;
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

		to << std::string(num_tabs, '\t') << "}\n";
	}

	void CGenerator::PushCondition(const Statement& condition, std::ofstream& to, size_t num_tabs) noexcept
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

	void CGenerator::PushForLoop(const Statement& for_loop, std::ofstream& to, size_t num_tabs) noexcept
	{
		std::cout << "Skipping For loop\n";
	}

	void CGenerator::PushWhileLoop(const Statement& while_loop, std::ofstream& to, size_t num_tabs) noexcept
	{
		const auto& view = while_loop.view_WhileLoop();
		
		to << std::string(num_tabs, '\t') << "while (";
		PushExpression(to);
		to << ")\n";

		PushBlock(view.whileloop_body(), to, num_tabs);
	}

	void CGenerator::PushStructDecl(const Statement& make_struct, std::ofstream& to, size_t num_tabs) noexcept
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
				if (info.GetADTFieldInfo(view.struct_name()).at(view.inits(i).view_VarInit().var_name()).typeclass == FieldType::Type::Primitive)
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
				else {
					std::cout << "Skipping for now\n";
				}
			}
		}

		to << std::string(num_tabs, '\t') << "} " << view.struct_name() << ";\n\n";
	}

	void CGenerator::PushStructDeleter(const Statement& make_struct, std::ofstream & to, size_t num_tabs) noexcept
	{
		const auto& view = make_struct.view_MakeStruct();

		to << std::string(num_tabs, '\t') << "void " << __FREE_FN_NAME(view.struct_name()) << "(uintptr_t address)\n";
		to << std::string(num_tabs, '\t') << "{\n";

		std::string deleter_pointer = "(" + (std::string)__DELETER_POINTER(view.struct_name()) + ")";

		for (size_t i = 0; i < view.size(); i++)
		{
			if (view.inits(i).option_is_VarInit())
			{
				if (info.GetADTFieldInfo(view.struct_name()).at(view.inits(i).view_VarInit().var_name()).typeclass == FieldType::Type::Struct)
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

	void CGenerator::PushStructInit(const Statement& make_struct, std::ofstream & to, size_t num_tabs) noexcept
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

			if (type_info.typeclass == FieldType::Type::Primitive)
			{
				to << type_info.name << " " << var_name;
			}
			else if (type_info.typeclass == FieldType::Type::Struct || type_info.typeclass == FieldType::Type::Property)
			{
				to << type_info.name << "* " << var_name;
			}
			else {
				// handle arrays and strings
			}

			i++;
		}

		to << ") \n";
		to << std::string(num_tabs, '\t') << "{\n";

		to << std::string(num_tabs + 1, '\t') << view.struct_name() << " *xx_" << view.struct_name()
			<< "_init_ptr = malloc(sizeof(" << view.struct_name() << "));\n";

		for (auto& [var_name, type_info] : info.GetADTFieldInfo(view.struct_name()))
		{
			if (type_info.typeclass == FieldType::Type::Primitive)
			{
				to << std::string(num_tabs + 1, '\t') << "xx_" << view.struct_name() << "_init_ptr->" <<
					var_name << " = " << var_name << ";\n";
			}
			else if (type_info.typeclass == FieldType::Type::Struct || type_info.typeclass == FieldType::Type::Property)
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

	void CGenerator::PushStructAssign(const Statement& make_struct, std::ofstream & to, size_t num_tabs) noexcept
	{
		const auto& view = make_struct.view_MakeStruct();

		to << std::string(num_tabs, '\t') << "void xx_assign_" << view.struct_name() << "(" << view.struct_name() << "** dest, " <<
			view.struct_name() << "* source)\n";
		to << std::string(num_tabs, '\t') << "{\n";
		to << std::string(num_tabs + 1, '\t') << "if (MVS_SameLoc((uintptr_t)(*dest), (uintptr_t)source)) { return; }\n";
		to << std::string(num_tabs + 1, '\t') << "MVS_DetachPointer((uintptr_t)(*dest));\n";
		to << std::string(num_tabs + 1, '\t') << "(*dest) = source;\n";
		to << std::string(num_tabs, '\t') << "}\n\n";
	}

	void CGenerator::PushMakeStruct(const Statement& make_struct, std::ofstream& to, size_t num_tabs) noexcept
	{
		const auto& view = make_struct.view_MakeStruct();

		PushStructDecl(make_struct, to, num_tabs);
		PushStructDeleter(make_struct, to, num_tabs);
		PushStructInit(make_struct, to, num_tabs);
		// PushStructAssign(make_struct, to, num_tabs); do we need it?
	}

	void CGenerator::PushMakeEnum(const Statement & make_enum, std::ofstream & to, size_t num_tabs) noexcept
	{
		const auto& view = make_enum.view_MakeEnum();

		to << std::string(num_tabs, '\t') << "typedef enum " << view.enum_name() << " {\n";

		for (size_t i = 0; i < view.size(); i++)
		{
			to << std::string(num_tabs + 1, '\t') << view.enum_variants(i) << ", ";
		}
		to << '\n' << std::string(num_tabs, '\t') << "} " << view.enum_name() << ";\n";
	}

	void CGenerator::PushForwardDeclProperty(const std::string& prop_name, std::ofstream& to, size_t num_tabs) noexcept
	{
		to << std::string(num_tabs, '\t') << "typedef struct " << prop_name << " " << prop_name << ";\n";
		to << std::string(num_tabs, '\t') << "void " << __FREE_FN_NAME(prop_name) << "(uintptr_t address);\n\n";
	}

	void CGenerator::PushTagEnumProperty(const Statement& make_property, std::ofstream& to, size_t num_tabs) noexcept
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

	void CGenerator::PushOptionStructProperty(const Statement& make_property, std::ofstream& to, size_t num_tabs) noexcept
	{
		const auto& view = make_property.view_MakeProperty();
		to << std::string(num_tabs, '\t') << "typedef union " << view.prop_name() << "_Opts {\n";

		for (size_t i = 0; i < view.size(); i++)
		{
			to << std::string(num_tabs + 1, '\t') << view.opts(i).view_MakeStruct().struct_name() << "* " << view.opts(i).view_MakeStruct().struct_name() << "_opt;\n";
		}

		to << std::string(num_tabs, '\t') << "} " << view.prop_name() << "_Opts;\n\n";
	}

	void CGenerator::PushPropertyDecl(const Statement& make_property, std::ofstream& to, size_t num_tabs) noexcept
	{
		const auto& view = make_property.view_MakeProperty();
		to << std::string(num_tabs, '\t') << "typedef struct " << view.prop_name() << " {\n";
		to << std::string(num_tabs + 1, '\t') << view.prop_name() << "_Tag tag;\n";
		to << std::string(num_tabs + 1, '\t') << view.prop_name() << "_Opts opts;\n";

		to << std::string(num_tabs, '\t') << "} " << view.prop_name() << ";\n\n";
	}

	void CGenerator::PushPropertyDeleter(const Statement& make_property, std::ofstream& to, size_t num_tabs) noexcept
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

	void CGenerator::PushPropertyInit(const std::string& prop_name, const std::string& opt_name, std::ofstream& to, size_t num_tabs) noexcept
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

	void CGenerator::PushPropertyNULLInit(const std::string& prop_name, const std::string& opt_name, std::ofstream& to, size_t num_tabs) noexcept
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

	void CGenerator::PushMakeProperty(const Statement & make_property, std::ofstream & to, size_t num_tabs) noexcept
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
	}

	void CGenerator::PushExpression(std::ofstream& to) noexcept
	{
		to << info.GetNextExpr().first;
		info.PopExpr();
	}

	void CGenerator::PushVarInitialization(const Statement& var_init, std::ofstream& to, size_t num_tabs) noexcept
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
			to << "/* HERE SHOULD BE STRING INIT */\n";
		}
	}

	void CGenerator::PushArrInitialization(const Statement& arr_init, std::ofstream& to, size_t num_tabs) noexcept
	{
		std::cout << "Skipping arr init\n";
	}
}