#pragma once

#include "Statements/Statement.h"
#include <unordered_map>

#include "Types.h"

namespace val
{
	class Semantics
	{
	public:
		std::string filename;

		std::unordered_map <std::string, VariableKind> symbol_table;
		std::unordered_map <std::string, TypeClass> type_table;
		std::unordered_map <std::string, FnTable> fn_table;
		std::unordered_map <std::string, bool> active;
		std::unordered_map <std::string, std::string> option_to_prop;

		std::vector <std::string> block_allocated;

#pragma region("Type Table Helpers")
		StructType ConstructStructType(const Statement& make_struct_stmt) const noexcept;
		PropertyType ConstructPropertyType(const Statement& make_property_stmt) const noexcept;
		EnumType ConstructEnumType(const Statement& make_enum_stmt) const noexcept;
		FnTable ConstructFn(const Statement& make_fn_stmt) const noexcept;

		bool NameExists(const std::string& name) const noexcept;
		bool TypeExists(const std::string& name) const noexcept;

		bool CanBeAssigned(const VariableKind& to, const VariableKind& from) noexcept;
#pragma endregion

#pragma region("Symbol Table Helpers")
		void AddVariable(const Statement& var_init) noexcept;
		void AddType(const std::string& type_name, TypeClass&& type) noexcept;
		void AddFn(const std::string& fn_name, FnTable&& ftable) noexcept;

		void Activate(const std::string& name) noexcept;
		void Deactivate(const std::string& name) noexcept;
#pragma endregion

#pragma region("Semantics")
		VariableKind AnalyzeVarNameExpr(const Expression& varname_expr, size_t line);
		VariableKind AnalyzeArrayCallExpr(const Expression& array_call, size_t line);
		VariableKind AnalyzeLiterals(const Expression& any_literal_expr, size_t line);
		VariableKind AnalyzeStructFieldCallExpr(const Expression& field_call, size_t line);
		VariableKind AnalyzePropertyFieldCallExpr(const Expression& field_call, const std::string& active_option, size_t line);
		VariableKind AnalyzeFnCallExpr(const Expression& fn_call, size_t line);
		VariableKind AnalyzeBinopExpr(const Expression& binop_expr, size_t line);
		VariableKind AnalyzeUnaryExpr(const Expression& unary_expr, size_t line);
		VariableKind AnalyzeInitListExpr(const Expression& init_list_expr, size_t line);
		VariableKind AnalyzeStructInitExpr(const Expression& struct_init_expr, size_t line);
		VariableKind AnalyzeExpression(const Expression& expr, size_t line);
		
		void AnalyzeExprCall(const Statement& expr_call_stmt);

		void AnalyzeVarInit(const Statement& var_init_stmt);
		void AnalyzeArrayInit(const Statement& array_init_stmt);

		bool AnalyzeLoopBody(const Statement& block_stmt, bool should_return, const VariableKind& ret);
		void AnalyzeWhileLoop(const Statement& while_loop_stmt, bool should_return, const VariableKind& ret);

		void AnalyzeFinalForLoopStmt(const Statement& final_for_stmt);
		void AnalyzeForLoop(const Statement& for_stmt, bool should_return, const VariableKind& ret);

		void AnalyzeMakeFunction(const Statement& make_fn_stmt);

		void AnalyzeMakeStruct(const Statement& make_struct_stmt);

		void AnalyzePropertyOption(const Statement& prop_opt_stmt, const std::string& prop_name);
		void AnalyzeMakeProperty(const Statement& make_prop_stmt);

		void AnalyzeMakeEnum(const Statement& make_enum_stmt);

		bool AnalyzeMatch(const Statement& match_stmt, bool should_return, const VariableKind& ret);

		void AnalyzeAssignment(const Statement& assign_stmt);

		bool AnalyzeElif(const Statement& elif_stmt, bool should_return, const VariableKind& ret);
		bool AnalyzeCondition(const Statement& cond_stmt, bool should_return, const VariableKind& ret);

		bool AnalyzeBlock(const Statement& block_stmt, bool should_return, const VariableKind& ret);
		
#pragma endregion

	public:
		Semantics(const Semantics&) = delete;
		Semantics& operator=(const Semantics&) = delete;
		Semantics(Semantics&&) noexcept = delete;
		Semantics& operator=(Semantics&&) noexcept = delete;

		Semantics(const std::string& filename);

		void Analysis(const Statement& AST);
	};

}