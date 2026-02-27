#include "CGenerator.h"

#define __IS_OPTION_CHECK_FN_NAME(prop, opt) "xx_" + prop + "_option_is_" + opt
#define __FREE_FN_NAME(type) "xx_free_" + type
#define __DELETER_POINTER(type) "(" + type + "*)address"
#define __AS_UINTPTR_T(type) "(uintptr_t)" + type

namespace val
{
	void CGenerator::PushMakeFunction(const Statement& make_fn, std::ofstream& to, size_t num_tabs) noexcept
	{
		const auto& view = make_fn.view_MakeFunction();
	}

	void CGenerator::PushReturn(const Statement& return_stmt, std::ofstream& to, size_t num_tabs) noexcept
	{
		const auto& view = return_stmt.view_Return();
		to << std::string(num_tabs, '\t') << "return\n";

		if (not view.return_expr().option_is_EmptyLiteral())
		{
			to << " ";
			PushExpression(view.return_expr(), to);
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

_Bool MVS_DetachPointer(uintptr_t address)
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

_Bool MVS_SameLoc(uintptr_t a, uintptr_t b)
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
			case selector::ArrayInitStmt:
				PushInitialization(view.statements(i), out, 1);
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
			default:
				std::cout << "Skipping For Now\n";
				break;
			}
		}

		out << "\tMVS_Destroy();\n";
		out << "}\n";

		out.close();

		std::cout << "Generated C IR at: " << c_ir_path.string() << "\n";

		return c_ir_path;
	}

	void CGenerator::PushBlock(const Statement& block, std::ofstream& to, size_t num_tabs) noexcept
	{
		const auto& view = block.view_Block();
		to << std::string(num_tabs, '\t') << "{\n";

		for (size_t i = 0; i < view.size(); i++)
		{
			switch (view.statements(i).sel())
			{
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
			default:
				std::cout << "Skipping\n";
				break;
			}
		}

		to << std::string(num_tabs, '\t') << "}\n";
	}

	void CGenerator::PushCondition(const Statement& condition, std::ofstream& to, size_t num_tabs) noexcept
	{
		const auto& view = condition.view_Condition();

		to << std::string(num_tabs, '\t') << "if (";
		PushExpression(view.if_cond(), to);
		to << ")\n";

		PushBlock(view.if_body(), to, num_tabs);

		for (size_t i = 0; i < view.size(); i++)
		{
			to << std::string(num_tabs, '\t') << "else if (";
			PushExpression(view.elif_stmt(i).view_ElifCondition().elif_cond(), to);
			to << ")\n";

			PushBlock(view.elif_stmt(i).view_ElifCondition().elif_body(), to, num_tabs);
		}

		if (not view.else_body().option_is_Empty())
		{
			to << std::string(num_tabs, '\t') << "else\n";
			PushBlock(view.else_body(), to, num_tabs);
		}
	}

	void CGenerator::PushWhileLoop(const Statement& while_loop, std::ofstream& to, size_t num_tabs) noexcept
	{
		const auto& view = while_loop.view_WhileLoop();
		
		to << std::string(num_tabs, '\t') << "while (";
		PushExpression(view.cond(), to);
		to << ")\n";

		PushBlock(view.whileloop_body(), to, num_tabs);
	}

	void CGenerator::PushMakeStruct(const Statement& make_struct, std::ofstream& to, size_t num_tabs) noexcept
	{
		const auto& view = make_struct.view_MakeStruct();

		to << std::string(num_tabs, '\t') << "typedef struct " << view.struct_name();

		if (view.size() == 0)
		{
			to << ' ' << view.struct_name() << ";\n\n";

			to << "void " << __FREE_FN_NAME(view.struct_name()) << "(uintptr_t address) {}\n\n";

			return;
		}

		to << " {\n";

		for (size_t i = 0; i < view.size(); i++)
		{
			if (view.inits(i).option_is_VarInit())
			{
				if (info.GetADTFieldInfo(view.struct_name(), view.inits(i).view_VarInit().var_name()) == FieldType::Primitive)
				{
					to << std::string(num_tabs + 1, '\t') << view.inits(i).view_VarInit().type_name() << " " << view.inits(i).view_VarInit().var_name() << ";\n";
				}
				else if (info.GetADTFieldInfo(view.struct_name(), view.inits(i).view_VarInit().var_name()) == FieldType::ADT)
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

		to << std::string(num_tabs, '\t') << "void " << __FREE_FN_NAME(view.struct_name()) << "(uintptr_t address)\n";
		to << std::string(num_tabs, '\t') << "{\n";

		std::string deleter_pointer = "(" + (std::string)__DELETER_POINTER(view.struct_name()) + ")";

		for (size_t i = 0; i < view.size(); i++)
		{
			if (view.inits(i).option_is_VarInit())
			{
				if (info.GetADTFieldInfo(view.struct_name(), view.inits(i).view_VarInit().var_name()) == FieldType::ADT)
				{
					to << std::string(num_tabs + 1, '\t') << "if (" << deleter_pointer << "->" << view.inits(i).view_VarInit().var_name() << " != NULL)\n";
					to << std::string(num_tabs + 1, '\t') << "{\n";
					to << std::string(num_tabs + 2, '\t') << __FREE_FN_NAME(view.inits(i).view_VarInit().type_name()) << "(" << __AS_UINTPTR_T(deleter_pointer) << "->" << view.inits(i).view_VarInit().var_name()  << ");\n";
					to << std::string(num_tabs + 1, '\t') << "}\n";
				}
			}
		}
		to << std::string(num_tabs + 1, '\t') << "free(" << deleter_pointer << ");\n";
		to << std::string(num_tabs, '\t') << "}\n\n";
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

	void CGenerator::PushMakeProperty(const Statement & make_property, std::ofstream & to, size_t num_tabs) noexcept
	{
		const auto& view = make_property.view_MakeProperty();

		for (size_t i = 0; i < view.size(); i++)
		{
			PushMakeStruct(view.opts(i), to, num_tabs);
		}

		to << std::string(num_tabs, '\t') << "typedef enum " << view.prop_name() << "_Tag {\n";
		to << std::string(num_tabs + 1, '\t');
		for (size_t i = 0; i < view.size(); i++)
		{
			to << view.opts(i).view_MakeStruct().struct_name() << "_tag, ";
		}
		to << '\n' << std::string(num_tabs, '\t') << "} " << view.prop_name() << "_Tag;\n\n";

		to << std::string(num_tabs, '\t') << "typedef union " << view.prop_name() << "_Opts {\n";

		for (size_t i = 0; i < view.size(); i++)
		{
			to << std::string(num_tabs + 1, '\t') << view.opts(i).view_MakeStruct().struct_name() << "* " << view.opts(i).view_MakeStruct().struct_name() << "_opt;\n";
		}

		to << std::string(num_tabs, '\t') << "} " << view.prop_name() << "_Opts;\n\n";

		to << std::string(num_tabs, '\t') << "typedef struct " << view.prop_name() << " {\n";
		to << std::string(num_tabs + 1, '\t') << view.prop_name() << "_Tag tag;\n";
		to << std::string(num_tabs + 1, '\t') << view.prop_name() << "_Opts* opts;\n";

		to << std::string(num_tabs, '\t') << "} " << view.prop_name() << ";\n\n";

		auto OptionCheckCreate = [&to, &view, &num_tabs](const std::string& IR_opt_name) {
			to << std::string(num_tabs, '\t') << "_Bool " << __IS_OPTION_CHECK_FN_NAME(view.prop_name(), IR_opt_name) << "(const " << view.prop_name() << "* prop)\n";
			to << std::string(num_tabs, '\t') << "{\n";
			to << std::string(num_tabs + 1, '\t') << "return prop->tag == " << view.opts(0).view_MakeStruct().struct_name() << "_tag;\n";
			to << std::string(num_tabs, '\t') << "}\n\n";
		};

		for (size_t i = 0; i < view.size(); i++)
		{
			OptionCheckCreate(view.opts(i).view_MakeStruct().struct_name());
		}

		to << std::string(num_tabs, '\t') << "void " << __FREE_FN_NAME(view.prop_name()) << "(uintptr_t address)\n";
		to << std::string(num_tabs, '\t') << "{\n";

		std::string deleter_pointer = "(" + (std::string)__DELETER_POINTER(view.prop_name()) + ")";

		to << std::string(num_tabs + 1, '\t') << "if (" << deleter_pointer << "->opts != NULL)\n";
		to << std::string(num_tabs + 1, '\t') << "{\n";
		for (size_t i = 0; i < view.size(); i++)
		{
			to << std::string(num_tabs + 2, '\t') << 
				"if ((" << deleter_pointer << "->tag == " << view.opts(i).view_MakeStruct().struct_name() << "_tag) && (" <<
				deleter_pointer << "->opts->" << view.opts(i).view_MakeStruct().struct_name() << "_opt != NULL)) \n";
			to << std::string(num_tabs + 2, '\t') << "{\n";
			to << std::string(num_tabs + 3, '\t') << __FREE_FN_NAME(view.opts(i).view_MakeStruct().struct_name()) << "(" << __AS_UINTPTR_T(deleter_pointer) << "->opts->" << view.opts(i).view_MakeStruct().struct_name() << "_opt);\n";
			to << std::string(num_tabs + 2, '\t') << "}\n";
		}
		to << std::string(num_tabs + 2, '\t') << "free(" << deleter_pointer << "->opts);\n";

		to << std::string(num_tabs + 1, '\t') << "}\n";
		to << std::string(num_tabs + 1, '\t') << "free(" << deleter_pointer << ");\n";
		to << std::string(num_tabs, '\t') << "}\n\n";
	}

	void CGenerator::PushExpression(const Expression& expr, std::ofstream& to) noexcept
	{
		to << info.GetNextExpr();
		info.PopExpr();
	}

	void CGenerator::PushInitialization(const Statement& var_init, std::ofstream& to, size_t num_tabs) noexcept
	{
		if (var_init.option_is_VarInit())
		{
			const auto& view = var_init.view_VarInit();

			if (info.GetSymbolType(view.var_name()) == FieldType::Primitive)
			{
				to << std::string(num_tabs, '\t') << view.type_name() << ' ' << view.var_name();
				if (not view.init_expr().option_is_EmptyLiteral())
				{
					to << " = ";
					PushExpression(view.init_expr(), to);
				}
				to << ";\n";
			}

			else if (info.GetSymbolType(view.var_name()) == FieldType::ADT) {
				to << std::string(num_tabs, '\t') << view.type_name() << "* " << view.var_name() 
					<< " = malloc(sizeof(" << view.type_name() << "));" << "\n";
				if (view.init_expr().option_is_EmptyLiteral())
				{
					to << std::string(num_tabs, '\t') << view.var_name() << " = NULL;\n";
				}
				else {
					to << std::string(num_tabs, '\t') << '*' << view.var_name() << " = " << info.GetNextExpr() << ";\n";
				}
				to << std::string(num_tabs, '\t') << "MVS_RegisterNew((uintptr_t)" << view.var_name() << ", sizeof(" << view.type_name() << "), " << __FREE_FN_NAME(view.type_name()) << ");\n\n";
			}
		}
		else { // option_is_ArrayInit

		}
	}
}