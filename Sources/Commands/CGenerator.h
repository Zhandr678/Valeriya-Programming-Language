#pragma once

#include <variant>

#include "Statements/Statement.h"

namespace val
{

	class CGenerator
	{
	private:
		static void PushInitialization(const Statement& var_init, std::string& to);
		static void PushAssignment(const Statement& assignment, std::string& to);
		static void PushExprCall(const Statement& expr_call, std::string& to);
		static void PushBlock(const Statement& block, std::string& to);
		static void PushCondition(const Statement& condition, std::string& to);
		static void PushForLoop(const Statement& for_loop, std::string& to);
		static void PushWhileLoop(const Statement& while_loop, std::string& to);
		static void PushMakeStruct(const Statement& make_struct, std::string& to);
		static void PushMakeEnum(const Statement& make_enum, std::string& to);
		static void PushCaseClause(const Statement& case_clause, std::string& to);
		static void PushMakeProperty(const Statement& make_property, std::string& to);
		static void PushMatch(const Statement& match, std::string& to);
		static void PushMakeFunction(const Statement& make_fn, std::string& to);
		static void PushReturn(const Statement& return_stmt, std::string& to);
		static void PushBreak(const Statement& break_stmt, std::string& to);
		static void PushContinue(const Statement& continue_stmt, std::string& to);

	public:

		static std::filesystem::path GenerateC_IR(const Statement & valid_AST, std::string_view val_source_path)
		{
			std::string source_code;

			for (size_t i = 0; i < valid_AST.view_Block().size(); i++)
			{
				switch (valid_AST.view_Block().statements(i).sel())
				{
					
				}
			}

			std::filesystem::path c_ir_path = std::filesystem::path(val_source_path).replace_extension(".c");
			std::ifstream out(c_ir_path);

			if (not out.is_open())
			{
				throw std::runtime_error("Failed to Open File " + c_ir_path.string());
			}

			/* HOW TO WRITE TO IFSTREAM CREATED FILE */



			return c_ir_path;
		}
	};

}