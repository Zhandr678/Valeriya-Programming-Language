#pragma once

#include <variant>
#include <fstream>
#include <filesystem>

#include "Statements/Statement.h"
#include "SemanticAnalysis/Types.h"
#include "Commands/CompileInfo.h"

namespace val
{

	class CGenerator
	{
	private:
		CompileInfo info;

		void PushExpression(std::ostream& to) noexcept;

		void PushVarInitialization(const Statement& var_init, std::ostream& to, size_t num_tabs, std::vector <std::string>& alloc_ptrs) noexcept;
		void PushArrInitialization(const Statement& arr_init, std::ostream& to, size_t num_tabs, std::vector <std::string>& alloc_ptrs) noexcept;

		void PushCleanup(const std::vector <std::string>& alloc_ptrs, std::ostream& to, size_t num_tabs) noexcept;

		std::vector <std::string> field_order;
		std::string initial_assign_expr_str;
		bool is_prop{false};
		std::string cur_option;
		ExpressionInfo cur_lhs_info, cur_rhs_info;
		void PushFieldsVector(const std::string& field_call_str) noexcept;
		std::string PushFieldRecurse(const Expression& caller, std::ostream& to, size_t num_tabs, size_t i) noexcept;
		
		void PushAssignment(const Statement& assignment, std::ostream& to, size_t num_tabs) noexcept;
		void PushExprCall(const Statement& expr_call, std::ostream& to, size_t num_tabs) noexcept;
		void PushBlock(const Statement& block, std::ostream& to, size_t num_tabs, std::vector <std::string> allocated = {}) noexcept;
		void PushCondition(const Statement& condition, std::ostream& to, size_t num_tabs) noexcept;
		void PushForLoop(const Statement& for_loop, std::ostream& to, size_t num_tabs) noexcept;
		void PushWhileLoop(const Statement& while_loop, std::ostream& to, size_t num_tabs) noexcept;

		void PushStructDecl(const Statement& make_struct, std::ostream& to, size_t num_tabs) noexcept;
		void PushStructDeleter(const Statement& make_struct, std::ostream& to, size_t num_tabs) noexcept;
		void PushStructInit(const Statement& make_struct, std::ostream& to, size_t num_tabs) noexcept;
		void PushStructClone(const Statement& make_struct, std::ostream& to, size_t num_tabs) noexcept;

		void PushMakeStruct(const Statement& make_struct, std::ostream& to, size_t num_tabs) noexcept;
		
		void PushMakeEnum(const Statement& make_enum, std::ostream& to, size_t num_tabs) noexcept;

		void PushForwardDeclProperty(const std::string& prop_name, std::ostream& to, size_t num_tabs) noexcept;
		void PushTagEnumProperty(const Statement& make_property, std::ostream& to, size_t num_tabs) noexcept;
		void PushOptionStructProperty(const Statement& make_property, std::ostream& to, size_t num_tabs) noexcept;
		void PushPropertyDecl(const Statement& make_property, std::ostream& to, size_t num_tabs) noexcept;
		void PushPropertyDeleter(const Statement& make_property, std::ostream& to, size_t num_tabs) noexcept;
		void PushPropertyInit(const std::string& prop_name, const std::string& opt_name, std::ostream& to, size_t num_tabs) noexcept;
		void PushPropertyNULLInit(const std::string& prop_name, const std::string& opt_name, std::ostream& to, size_t num_tabs) noexcept;
		void PushPropertyClone(const Statement& make_property, std::ostream& to, size_t num_tabs) noexcept;

		void PushMakeProperty(const Statement& make_property, std::ostream& to, size_t num_tabs) noexcept;

		void PushPropertyCaseClause(const std::string& prop_name, const std::string& prop_type, const Statement& case_clause, std::ostream& to, size_t num_tabs, size_t order) noexcept;
		void PushEnumCaseClause(const std::string& enum_expr, const Statement& case_clause, std::ostream& to, size_t num_tabs, size_t order) noexcept;
		void PushMatch(const Statement& match, std::ostream& to, size_t num_tabs) noexcept;

		void PushMakeFunction(const Statement& make_fn, std::ostream& to, size_t num_tabs) noexcept;
		void PushReturn(const Statement& return_stmt, std::ostream& to, size_t num_tabs) noexcept;
		void PushBreak(const Statement& break_stmt, std::ostream& to, size_t num_tabs) noexcept;
		void PushContinue(const Statement& continue_stmt, std::ostream& to, size_t num_tabs) noexcept;


		void AddHeaders(std::ostream& to) noexcept;

	public:
		CGenerator(CompileInfo&& info);

		std::filesystem::path GenerateC_IR(const Statement& valid_AST, std::string_view val_source_path);
	
	};

}