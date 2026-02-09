#pragma once

#include "Statements/Statement.h"
#include "Commands/Command.h"
#include "DSU.h"

#include <unordered_map>

#include "Types.h"
using Command = std::variant<std::monostate>;
namespace val
{
	class Semantics
	{
	public:
		DSU dsu_COW;

		std::unordered_map <std::string, VariableKind> symbol_table;
		std::unordered_map <std::string, TypeClass> type_table;
		std::unordered_map <std::string, FnTable> fn_table;
		std::unordered_map <std::string, bool> active;

#pragma region("Type Table Helpers")
		StructType ConstructStructType(const Statement& make_struct_stmt) const noexcept;
		PropertyType ConstructPropertyType(const Statement& make_property_stmt) const noexcept;
		EnumType ConstructEnumType(const Statement& make_enum_stmt) const noexcept;
		FnTable ConstructFn(const Statement& make_fn_stmt) const noexcept;

		bool NameExists(const std::string& name) const;
#pragma endregion

#pragma region("Symbol Table Helpers")
		void AddVariable(const Statement& var_init) noexcept;
		void AddType(const std::string& type_name, TypeClass&& type) noexcept;
		void AddFn(const std::string& fn_name, FnTable&& ftable) noexcept;

		void Deactivate(const std::string& name) noexcept;
#pragma endregion
		void VerifyInoutUniqueness(const Expression& fn_call_expr) const;

#pragma region("Semantics")
		

		VariableKind AnalyzeVarNameExpr(const Expression& varname_expr);
		VariableKind AnalyzeArrayCallExpr(const Expression& array_call);
		VariableKind AnalyzeLiterals(const Expression& any_literal_expr);
		VariableKind AnalyzeStructFieldCallExpr(const Expression& field_call);
		VariableKind AnalyzePropertyFieldCallExpr(const Expression& field_call, const std::string& active_option);
		VariableKind AnalyzeFnCallExpr(const Expression& fn_call);
		VariableKind AnalyzeBinopExpr(const Expression& binop_expr);
		VariableKind AnalyzeUnaryExpr(const Expression& unary_expr);
		
		VariableKind AnalyzeInitListExpr(const Expression& init_list_expr);
		VariableKind AnalyzeStructInitExpr(const Expression& struct_init_expr);

		VariableKind AnalyzeExpression(const Expression& expr);
		
		std::vector <Command> AnalyzeVarInit(const Statement& var_init_stmt);
		std::vector <Command> AnalyzeArrayInit(const Statement& array_init_stmt);
		std::vector <Command> AnalyzeWhileLoop(const Statement& while_loop_stmt);
		std::vector <Command> AnalyzeForLoop(const Statement& for_stmt);
		std::vector <Command> AnalyzeMakeFunction(const Statement& make_fn_stmt);
		std::vector <Command> AnalyzeMakeStruct(const Statement& make_struct_stmt);
		std::vector <Command> AnalyzeMakeProperty(const Statement& make_prop_stmt);
		std::vector <Command> AnalyzeMakeEnum(const Statement& make_enum_stmt);
		std::vector <Command> AnalyzeMatch(const Statement& match_stmt);
		std::vector <Command> AnalyzeAssignment(const Statement& assign_stmt);
		std::vector <Command> AnalyzeCondition(const Statement& cond_stmt);
		std::vector <Command> AnalyzeBlock(const Statement& block_stmt);
		
#pragma endregion

	public:
		Semantics(const Semantics&) = delete;
		Semantics& operator=(const Semantics&) = delete;
		Semantics(Semantics&&) noexcept = delete;
		Semantics& operator=(Semantics&&) noexcept = delete;

		Semantics() = default;

		std::vector <Command> AnalyzePrepareCommands(const Statement& AST);
	};

}