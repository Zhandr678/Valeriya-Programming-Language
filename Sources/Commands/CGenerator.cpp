#include "CGenerator.h"

namespace val
{
	void CGenerator::PushMakeFunction(const Statement& make_fn, std::ofstream& to) noexcept
	{
		const auto& view = make_fn.view_MakeFunction();
	}

	void CGenerator::PushReturn(const Statement& return_stmt, std::ofstream& to) noexcept
	{
		const auto& view = return_stmt.view_Return();
		to << "return;\n";

		if (not view.return_expr().option_is_EmptyLiteral())
		{
			// PushExpression(view.return_expr(), to);
		}
	}

	void CGenerator::PushBreak(const Statement& break_stmt, std::ofstream& to) noexcept
	{
		to << "break;\n";
	}

	void CGenerator::PushContinue(const Statement& continue_stmt, std::ofstream& to) noexcept
	{
		to << "continue;\n";
	}

	void CGenerator::PushBlock(const Statement& block, std::ofstream& to) noexcept
	{
		const auto& view = block.view_Block();
		for (size_t i = 0; i < view.size(); i++)
		{
			switch (view.statements(i).sel())
			{
			case selector::ConditionStmt:
				PushCondition(view.statements(i), to);
				break;
			case selector::WhileLoopStmt:
				PushWhileLoop(view.statements(i), to);
				break;
			case selector::ReturnStmt:
				PushReturn(view.statements(i), to);
				break;
			case selector::BreakStmt:
				PushBreak(view.statements(i), to);
				break;
			case selector::ContinueStmt:
				PushContinue(view.statements(i), to);
				break;
			default:
				std::cout << "Skipping\n";
				break;
			}
		}
	}

	void CGenerator::PushCondition(const Statement& condition, std::ofstream& to) noexcept
	{
		const auto& view = condition.view_Condition();

		to << "if (";
		PushExpression(view.if_cond(), to);
		to << ")\n";

		PushBlock(view.if_body(), to);

		for (size_t i = 0; i < view.size(); i++)
		{
			to << "else if (";
			PushExpression(view.elif_stmt(i).view_ElifCondition().elif_cond(), to);
			to << ")\n";

			PushBlock(view.elif_stmt(i).view_ElifCondition().elif_body(), to);
		}

		if (not view.else_body().option_is_Empty())
		{
			to << "else\n";
			PushBlock(view.else_body(), to);
		}
	}

	void CGenerator::PushWhileLoop(const Statement& while_loop, std::ofstream& to) noexcept
	{
		const auto& view = while_loop.view_WhileLoop();
		
		to << "while (";
		PushExpression(view.cond(), to);
		to << ")\n";

		PushBlock(view.whileloop_body(), to);
	}

	void CGenerator::PushExpression(const Expression& expr, std::ofstream& to) noexcept
	{
		to << "expr";
		// I am lazy, will implement this later
		// when I am left with no time to implement the rest of the codegen
		// so stupid of me, I know... 
		// I will implement this later, I promise
		// I will implement this later, I promise
		// I will implement this later, I promise
		// I will implement this later, I promise
	}

}