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

		void PushExpression(std::ofstream& to) noexcept;

		void PushVarInitialization(const Statement& var_init, std::ofstream& to, size_t num_tabs) noexcept;
		void PushArrInitialization(const Statement& arr_init, std::ofstream& to, size_t num_tabs) noexcept;
		void PushAssignment(const Statement& assignment, std::ofstream& to, size_t num_tabs) noexcept;
		void PushExprCall(const Statement& expr_call, std::ofstream& to, size_t num_tabs) noexcept;
		void PushBlock(const Statement& block, std::ofstream& to, size_t num_tabs) noexcept;
		void PushCondition(const Statement& condition, std::ofstream& to, size_t num_tabs) noexcept;
		void PushForLoop(const Statement& for_loop, std::ofstream& to, size_t num_tabs) noexcept;
		void PushWhileLoop(const Statement& while_loop, std::ofstream& to, size_t num_tabs) noexcept;

		void PushStructDecl(const Statement& make_struct, std::ofstream& to, size_t num_tabs) noexcept;
		void PushStructDeleter(const Statement& make_struct, std::ofstream& to, size_t num_tabs) noexcept;
		void PushStructInit(const Statement& make_struct, std::ofstream& to, size_t num_tabs) noexcept;
		void PushStructAssign(const Statement& make_struct, std::ofstream& to, size_t num_tabs) noexcept;

		void PushMakeStruct(const Statement& make_struct, std::ofstream& to, size_t num_tabs) noexcept;
		
		void PushMakeEnum(const Statement& make_enum, std::ofstream& to, size_t num_tabs) noexcept;

		void PushForwardDeclProperty(const std::string& prop_name, std::ofstream& to, size_t num_tabs) noexcept;
		void PushTagEnumProperty(const Statement& make_property, std::ofstream& to, size_t num_tabs) noexcept;
		void PushOptionStructProperty(const Statement& make_property, std::ofstream& to, size_t num_tabs) noexcept;
		void PushPropertyDecl(const Statement& make_property, std::ofstream& to, size_t num_tabs) noexcept;
		void PushPropertyDeleter(const Statement& make_property, std::ofstream& to, size_t num_tabs) noexcept;
		void PushPropertyInit(const std::string& prop_name, const std::string& opt_name, std::ofstream& to, size_t num_tabs) noexcept;
		void PushPropertyNULLInit(const std::string& prop_name, const std::string& opt_name, std::ofstream& to, size_t num_tabs) noexcept;

		void PushMakeProperty(const Statement& make_property, std::ofstream& to, size_t num_tabs) noexcept;

		void PushPropertyCaseClause(const std::string& prop_name, const std::string& prop_type, const Statement& case_clause, std::ofstream& to, size_t num_tabs) noexcept;
		void PushEnumCaseClause(const std::string& enum_expr, const Statement& case_clause, std::ofstream& to, size_t num_tabs) noexcept;
		void PushMatch(const Statement& match, std::ofstream& to, size_t num_tabs) noexcept;

		void PushMakeFunction(const Statement& make_fn, std::ofstream& to, size_t num_tabs) noexcept;
		void PushReturn(const Statement& return_stmt, std::ofstream& to, size_t num_tabs) noexcept;
		void PushBreak(const Statement& break_stmt, std::ofstream& to, size_t num_tabs) noexcept;
		void PushContinue(const Statement& continue_stmt, std::ofstream& to, size_t num_tabs) noexcept;


		void AddHeaders(std::ofstream& to) noexcept;

	public:
		CGenerator(CompileInfo&& info);

		std::filesystem::path GenerateC_IR(const Statement& valid_AST, std::string_view val_source_path);
	
	};

}