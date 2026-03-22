#pragma once

#include <string>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <variant>
#include <functional>

#include "Statement.h"
#include "Expression.h"
#include "Semantics.h"

enum class ExprType
{
	Null, Primitive, Enum, Struct, Property, String,
	ArrayOfPrim, ArrayOfEnum, ArrayOfStruct, ArrayOfProp, ArrayOfString
};

struct ExprRet
{
	ExprType type{ ExprType::Null };
	std::string type_name;
	std::string str_expr;
	bool is_lvalue{false};
};

namespace val
{
	class CGenerator
	{
	private:
		std::unordered_map <std::string, VariableKind> symbol_table;
		std::unordered_map <std::string, TypeClass> type_table;
		std::unordered_map <std::string, FnTable> fn_table;

		std::unordered_map <std::string, std::string> opt_to_prop;

		bool in_match{false};
		std::string cur_prop_name;
		std::string cur_option_name;
		std::string cur_match_expr;

		bool TypeIsPrimitive(const std::string& type_name) const noexcept;
		bool TypeIsStruct(const std::string& type_name) const noexcept;
		bool TypeIsProperty(const std::string& type_name) const noexcept;
		bool TypeIsEnum(const std::string& type_name) const noexcept;
		bool TypeIsString(const std::string& type_name) const noexcept;

		bool _IsPrimitive(const std::string& var_name) const noexcept;
		bool _IsPrimitive(const std::string& struct_name, const std::string& field_name) const noexcept;

		bool _IsString(const std::string& var_name) const noexcept;
		bool _IsString(const std::string& struct_name, const std::string& field_name) const noexcept;

		bool _IsStructType(const std::string& var_name) const noexcept;
		bool _IsStructType(const std::string& struct_name, const std::string& field_name) const noexcept;

		bool _IsEnumType(const std::string& var_name) const noexcept;
		bool _IsEnumType(const std::string& struct_name, const std::string& field_name) const noexcept;

		bool _IsProperty(const std::string& var_name) const noexcept;
		bool _IsProperty(const std::string& struct_name, const std::string& field_name) const noexcept;

		// std::string is type in the array
		std::optional <std::string> _IsArray(const std::string& var_name) const noexcept;
		std::optional <std::string> _IsArray(const std::string& struct_name, const std::string& field_name) const noexcept;

		bool IsExprTypeArray(const ExprType& expr);

		std::string indent(size_t tabs) const noexcept;

		void AddHeaders(std::ostream& to) noexcept;

		std::string ToString(const Expression& expr) noexcept;

		std::string GetTypeName(const VariableKind& v) noexcept;

		ExprRet EvalFieldCall(const Expression& field_call_expr) noexcept;
		ExprRet EvalArrIndex(const Expression& arr_index_expr) noexcept;
		ExprRet EvalVarName(const Expression& var_name_expr) noexcept;
		ExprRet EvalLiteral(const Expression& literal_expr) noexcept;
		ExprRet EvalFnCallExpr(const Expression& fn_call_expr) noexcept;
		ExprRet EvalBinaryExpr(const Expression& bin_expr) noexcept;
		ExprRet EvalUnaryExpr(const Expression& unary_expr) noexcept;
		ExprRet EvalInitListExpr(const Expression& init_list_expr) noexcept;
		ExprRet EvalStructInitExpr(const Expression& struct_init_expr) noexcept;
		ExprRet EvalExpression(const Expression& expr) noexcept;

		void GenExprCall(const Statement& expr_call_stmt, std::ostream& to, size_t tabs) noexcept;

		void GenAssign(const Statement& assign_stmt, std::ostream& to, size_t tabs) noexcept;

		void GenVarInit(const Statement& var_init_stmt, std::ostream& to, size_t tabs, std::vector <std::string>& allocs) noexcept;
		void GenArrInit(const Statement& arr_init_stmt, std::ostream& to, size_t tabs, std::vector <std::string>& allocs) noexcept;

		void GenReturn(const Statement& return_stmt, std::ostream& to, size_t tabs) noexcept;

		void GenMakeFn(const Statement& make_fn, std::ostream& to, size_t tabs) noexcept;

		void GenStructDecl(const Statement& struct_stmt, std::ostream& to, size_t tabs) noexcept;
		void GenStructDel(const Statement& struct_stmt, std::ostream& to, size_t tabs) noexcept;
		void GenStructInit(const Statement& struct_stmt, std::ostream& to, size_t tabs) noexcept;
		void GenStructClone(const Statement& struct_stmt, std::ostream& to, size_t tabs) noexcept;
		void GenMakeStruct(const Statement& struct_stmt, std::ostream& to, size_t tabs) noexcept;

		void GenFwDeclProp(const Statement& prop_stmt, std::ostream& to, size_t tabs) noexcept;
		void GenTagEnumProp(const Statement& prop_stmt, std::ostream& to, size_t tabs) noexcept;
		void GenOptionProp(const Statement& prop_stmt, std::ostream& to, size_t tabs) noexcept;
		void GenPropDecl(const Statement& prop_stmt, std::ostream& to, size_t tabs) noexcept;
		void GenPropDel(const Statement& prop_stmt, std::ostream& to, size_t tabs) noexcept;
		void GenPropInit(const std::string& prop_name, const std::string& opt_name, std::ostream& to, size_t tabs) noexcept;
		void GenPropNullInit(const std::string& prop_name, const std::string& opt_name, std::ostream& to, size_t tabs) noexcept;
		void GenPropClone(const Statement& prop_stmt, std::ostream& to, size_t tabs) noexcept;

		void GenMakeProperty(const Statement& property_stmt, std::ostream& to, size_t tabs) noexcept;

		void GenMakeEnum(const Statement& enum_stmt, std::ostream& to, size_t tabs) noexcept;

		void GenCaseProperty(const Statement& case_stmt, const std::string& prop_type, const std::string& prop_expr_name, size_t order, std::ostream& to, size_t tabs) noexcept;
		void GenCaseEnum(const Statement& case_stmt, const std::string& enum_expr, size_t order, std::ostream& to, size_t tabs) noexcept;
		void GenMatch(const Statement& match_stmt, std::ostream& to, size_t tabs) noexcept;
		void GenFor(const Statement& for_stmt, std::ostream& to, size_t tabs) noexcept;
		void GenWhile(const Statement& while_stmt, std::ostream& to, size_t tabs) noexcept;
		void GenCond(const Statement& cond_stmt, std::ostream& to, size_t tabs) noexcept;

		void GenBlock(const Statement& block_stmt, std::ostream& to, size_t tabs, std::vector <std::string> allocs = {}) noexcept;

		void GenCleanup(std::vector <std::string>& allocs, std::ostream& to, size_t tabs) noexcept;
		
	public:
		explicit CGenerator(
			std::unordered_map <std::string, VariableKind>&& symbol_table,
			std::unordered_map <std::string, TypeClass>&& type_table,
			std::unordered_map <std::string, FnTable>&& fn_table
		);

		std::filesystem::path GenerateC_IR(const Statement& valid_AST, const std::string& val_source_path);
	};
}