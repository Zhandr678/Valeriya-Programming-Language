#pragma once

#include "Statements/Statement.h"
#include "Commands/Command.h"

#include <unordered_map>
#include <set>

#include "Types.h"
#include "ExprAnalysis.h"

namespace val
{
	class Semantics
	{
	private:
		Statement AST;

		std::unordered_map <std::string, VariableKind> syntax_table;
		std::unordered_map <std::string, CustomTypeClass> type_table;
		std::unordered_map <std::string, std::pair <VariableKind, std::unordered_map <std::string, VariableKind>>> fn_table;

#pragma region("Type Table Helpers")
		StructType ConstructStructType(const Statement& make_struct_stmt) noexcept;
		PropertyType ConstructPropertyType(const Statement& make_property_stmt) noexcept;
		EnumType ConstructEnumType(const Statement& make_enum_stmt) noexcept;

		void RemoveStructType(const std::string& type_name);
		void RemovePropertyType(const std::string& type_name);
		void RemoveEnumType(const std::string& type_name);

		TypeClass IdentifyType(const std::string& var_name);
		TypeClass IdentifyType(const Expression& expr);

		bool TypeExists(const std::string& type_name);
#pragma endregion

#pragma region("Symbol Table Helpers")
		void AddVariable(const Statement& var_init);
		void RemoveVariable(const std::string& var_name);

		bool VariableExists(const std::string& var_name);
		bool FnNameExists(const std::string& fn_name);
#pragma endregion

		bool IdentifierNameExists(const std::string& name);
		bool VerifyExpression(const Expression& expr);
		bool InoutUniqueness(const Statement& make_fn_stmt);
		std::string ExprToStr(const Expression& expr);

#pragma region("Semantics")

		ExpressionAnalysisResult AnalyzeVarNameExpr(const Expression& varname_expr);
		ExpressionAnalysisResult AnalyzeArrayCallExpr(const Expression& array_call);
		// Do we need to analyze literals?
		ExpressionAnalysisResult AnalyzeFieldCallExpr(const Expression& field_call);
		ExpressionAnalysisResult AnalyzeFnCallExpr(const Expression& fn_call);
		ExpressionAnalysisResult AnalyzeBinopExpr(const Expression& binop_expr);
		ExpressionAnalysisResult AnalyzeUnaryExpr(const Expression& unary_expr);

		ExpressionAnalysisResult AnalyzeExpression(const Expression& expr);
		
		Command AnalyzeVarInit(const Statement& var_init_stmt);
		Command AnalyzeArrayInit(const Statement& array_init_stmt);
		Command AnalyzeWhileLoop(const Statement& while_loop_stmt);
		Command AnalyzeForLoop(const Statement& for_stmt);
		Command AnalyzeMakeFunction(const Statement& make_fn_stmt);
		Command AnalyzeMakeStruct(const Statement& make_struct_stmt);
		Command AnalyzeMakeProperty(const Statement& make_prop_stmt);
		Command AnalyzeMakeEnum(const Statement& make_enum_stmt);
		Command AnalyzeMatch(const Statement& match_stmt);
		Command AnalyzeAssignment(const Statement& assign_stmt);
		Command AnalyzeCondition(const Statement& cond_stmt);
		Command AnalyzeBlock(const Statement& block_stmt); 
		
#pragma endregion

	public:
		Semantics() = delete;
		Semantics(const Semantics&) = delete;
		Semantics& operator=(const Semantics&) = delete;
		Semantics(Semantics&&) noexcept = delete;
		Semantics& operator=(Semantics&&) noexcept = delete;

		explicit Semantics(Statement&& AST);

		std::vector <Command> AnalyzePrepareCommands();
	};

}