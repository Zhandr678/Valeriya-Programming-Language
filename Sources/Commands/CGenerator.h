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

		void PushExpression(const Expression& expr, std::ofstream& to) noexcept;

		void PushInitialization(const Statement& var_init, std::ofstream& to, size_t num_tabs) noexcept;
		void PushAssignment(const Statement& assignment, std::ofstream& to, size_t num_tabs) noexcept;
		void PushExprCall(const Statement& expr_call, std::ofstream& to, size_t num_tabs) noexcept;
		void PushBlock(const Statement& block, std::ofstream& to, size_t num_tabs) noexcept;
		void PushCondition(const Statement& condition, std::ofstream& to, size_t num_tabs) noexcept;
		void PushForLoop(const Statement& for_loop, std::ofstream& to, size_t num_tabs) noexcept;
		void PushWhileLoop(const Statement& while_loop, std::ofstream& to, size_t num_tabs) noexcept;
		void PushMakeStruct(const Statement& make_struct, std::ofstream& to, size_t num_tabs) noexcept;
		void PushMakeEnum(const Statement& make_enum, std::ofstream& to, size_t num_tabs) noexcept;
		void PushCaseClause(const Statement& case_clause, std::ofstream& to, size_t num_tabs) noexcept;
		void PushMakeProperty(const Statement& make_property, std::ofstream& to, size_t num_tabs) noexcept;
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