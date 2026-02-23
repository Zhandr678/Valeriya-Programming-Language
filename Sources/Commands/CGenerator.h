#pragma once

#include <variant>
#include <fstream>
#include <filesystem>

#include "Statements/Statement.h"

namespace val
{

	class CGenerator
	{
	private:
		static void PushVarName(const Expression& var_name, std::ofstream& to) noexcept;
		static void PushArrayIndex(const Expression& array_index, std::ofstream& to) noexcept;
		static void PushFieldCall(const Expression& field_call, std::ofstream& to) noexcept;
		static void PushLiteral(const Expression& literal, std::ofstream& to) noexcept;
		static void PushStructInit(const Expression& struct_init, std::ofstream& to) noexcept;
		static void PushInitList(const Expression& init_list, std::ofstream& to) noexcept;
		static void PushEmptyLiteral(const Expression& empty_literal, std::ofstream& to) noexcept;

		static void PushExpression(const Expression& expr, std::ofstream& to) noexcept;

		static void PushInitialization(const Statement& var_init, std::ofstream& to) noexcept;
		static void PushAssignment(const Statement& assignment, std::ofstream& to) noexcept;
		static void PushExprCall(const Statement& expr_call, std::ofstream& to) noexcept;
		static void PushBlock(const Statement& block, std::ofstream& to) noexcept;
		static void PushCondition(const Statement& condition, std::ofstream& to) noexcept;
		static void PushForLoop(const Statement& for_loop, std::ofstream& to) noexcept;
		static void PushWhileLoop(const Statement& while_loop, std::ofstream& to) noexcept;
		static void PushMakeStruct(const Statement& make_struct, std::ofstream& to) noexcept;
		static void PushMakeEnum(const Statement& make_enum, std::ofstream& to) noexcept;
		static void PushCaseClause(const Statement& case_clause, std::ofstream& to) noexcept;
		static void PushMakeProperty(const Statement& make_property, std::ofstream& to) noexcept;
		static void PushMatch(const Statement& match, std::ofstream& to) noexcept;
		static void PushMakeFunction(const Statement& make_fn, std::ofstream& to) noexcept;
		static void PushReturn(const Statement& return_stmt, std::ofstream& to) noexcept;
		static void PushBreak(const Statement& break_stmt, std::ofstream& to) noexcept;
		static void PushContinue(const Statement& continue_stmt, std::ofstream& to) noexcept;

	public:

		static std::filesystem::path GenerateC_IR(const Statement & valid_AST, std::string_view val_source_path)
		{
			std::filesystem::path c_ir_path = std::filesystem::path(val_source_path).replace_extension(".c");
			std::ofstream out(c_ir_path);

			if (not out.is_open())
			{
				throw std::runtime_error("Failed to Open File " + c_ir_path.string());
			}

			PushBlock(valid_AST, out);

			out.close();

			std::cout << "Generated C IR at: " << c_ir_path.string() << "\n";

			return c_ir_path;
		}
	};

}