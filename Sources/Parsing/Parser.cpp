#include "Parser.h"

#include "ParserException.h"

#pragma region("Helpers")


static bool CanBeTypeName(val::TokenLabel lbl)
{
	return lbl == val::TokenLabel::KW_INT || lbl == val::TokenLabel::KW_BOOL || lbl == val::TokenLabel::KW_STRING ||
		lbl == val::TokenLabel::KW_CHAR || lbl == val::TokenLabel::KW_DOUBLE || lbl == val::TokenLabel::IDENTIFIER;
}


#pragma endregion

namespace val 
{

	Parser::Parser(Lexer&& lexer) 
		: lexer(std::move(lexer))
	{
	}

	Parser::Parser(Parser&& parser) noexcept 
		: lexer(std::move(parser.lexer))
	{
	}

	Parser& Parser::operator=(Parser&& parser) noexcept
	{
		this->lexer = std::move(parser.lexer);
		return *this;
	}

	Statement Parser::ConstructAST(TokenLabel flag = TokenLabel::_EOF_)
	{
		std::vector <Statement> stms;

		Token next_token = GetNextPeeked();
		while (next_token.label != flag)
		{
			if (auto st = AnalyzeExpressionStatement()) { 
				stms.push_back(*st); 
			}
			else if (auto st = AnalyzeInitalizationStatement()) {
				stms.push_back(*st);
			}
			else if (auto st = AnalyzeConditionStatement()) {
				stms.push_back(*st);
			}
			else if (auto st = AnalyzeForLoopStatement()) {
				stms.push_back(*st);
			}
			else if (auto st = AnalyzeWhileLoopStatement()) {
				stms.push_back(*st);
			}
			else if (auto st = AnalyzeMakeEnumStatement()) {
				stms.push_back(*st);
			}
			else if (auto st = AnalyzeMakeFunctionStatement()) {
				stms.push_back(*st);
			}
			else if (auto st = AnalyzeMakeStructStatement()) {
				stms.push_back(*st);
			}
			else if (auto st = AnalyzeMatchStatement()) {
				stms.push_back(*st);
			}
			else {
				throw ParserException("Unknown Statement", GetFileName(), EmptyExpr, GetLine());
			}
		}

		return Statement(BlockOfStmt, stms.begin(), stms.end());
	}

	std::string Parser::GetFileName() const
	{
		return lexer.GetFileName();
	}

	size_t Parser::GetLine() const
	{
		return lexer.GetLine();
	}

	size_t Parser::GetColumn() const
	{
		return lexer.GetColumn();
	}

	Token Parser::GetNextImportantToken()
	{
		Token next = lexer.ReadAndClassifyNext();

		while (next.label == TokenLabel::COM_BLOCK || next.label == TokenLabel::COM_LINE || next.label == TokenLabel::SYM_SEMICOLON)
		{
			next = lexer.ReadAndClassifyNext();
		}

		return next;
	}

	Token Parser::GetNextPeeked()
	{
		if (next >= peeked.size())
		{
			peeked.push_back(GetNextImportantToken());
		}

		return peeked[next++];
	}

	void Parser::ResetLookAhead()
	{
		peeked.clear();
		next = 0;
	}

	std::optional<Statement> Parser::AnalyzeInitalizationStatement()
	{
		Token type_token = GetNextPeeked();
		std::vector <Statement> var_inits;

		if (CanBeTypeName(type_token.label))
		{
			next = 0;
			return std::nullopt;
		}

		Token next_token = GetNextPeeked();
		while (next_token.label != TokenLabel::SYM_SEMICOLON)
		{
			Token varname_token = std::move(next_token);

			if (varname_token.label != TokenLabel::IDENTIFIER)
			{
				throw; // initialization of variable with forbidden name;
			}

			next_token = GetNextPeeked();

			if (next_token.label == TokenLabel::SYM_COMMA)
			{
				var_inits.push_back(Statement(VarInitStmt, Expression(EmptyExpr), varname_token.attr, type_token.attr));
				next_token = GetNextPeeked();
				ResetLookAhead();
				continue;
			}

			if (next_token.label == TokenLabel::SYM_EQUAL)
			{
				auto expr = AnalyzeExpressionStatement();

				if (not expr.has_value()) 
				{ 
					throw; // no expression after equal symbol
				}
				
				var_inits.push_back(Statement(VarInitStmt, expr->view_ExprCall().expr(), varname_token.attr, type_token.attr));
				ResetLookAhead();
				
				next_token = GetNextPeeked();
				if (next_token.label == TokenLabel::SYM_SEMICOLON) { break; }
				else if (next_token.label == TokenLabel::SYM_COMMA) { next_token = GetNextPeeked(); }
				else { throw; }
			}
		}

		ResetLookAhead();
		return Statement(BlockOfStmt, var_inits.begin(), var_inits.end());
	}

	std::optional<Statement> Parser::AnalyzeAssignmentStatement()
	{
		return std::optional<Statement>();
	}

	std::optional<Statement> Parser::AnalyzeConditionStatement()
	{
		if (GetNextPeeked().label != TokenLabel::KW_IF)
		{
			next = 0;
			return std::nullopt;
		}

		if (GetNextPeeked().label != TokenLabel::SYM_LPAR)
		{
			throw; // no ( after if
		}

		auto expr = AnalyzeExpressionStatement();
		
		if (not expr.has_value())
		{
			throw; // no expression inside if
		}

		if (GetNextPeeked().label != TokenLabel::SYM_RPAR)
		{
			throw; // no ) after if (expr
		}

		if (GetNextPeeked().label != TokenLabel::SYM_LBRACE)
		{
			throw; // no { after if (expr)
		}

		// Statement if_then = ConstructNextStatement();

		if (GetNextPeeked().label != TokenLabel::SYM_RBRACE)
		{
			throw; // no } after if (expr) { stmts 
		}
		return std::optional<Statement>();
	}

	std::optional<Statement> Parser::AnalyzeForLoopStatement()
	{
		return std::optional<Statement>();
	}

	std::optional<Statement> Parser::AnalyzeWhileLoopStatement()
	{
		return std::optional<Statement>();
	}

	std::optional<Statement> Parser::AnalyzeMakeFunctionStatement()
	{
		if (GetNextPeeked().label != TokenLabel::KW_FUNC)
		{
			next = 0;
			return std::nullopt;
		}

		Token fn_name_token = GetNextPeeked();

		if (fn_name_token.label != TokenLabel::IDENTIFIER)
		{
			throw; // Forbidden name for function
		}

		if (GetNextPeeked().label != TokenLabel::SYM_LPAR)
		{
			throw; // need ( after function name
		}

		Token next_token = GetNextPeeked();
		std::vector <Statement> args;

		while (next_token.label != TokenLabel::SYM_RPAR)
		{
			Statement arg(FnArgsStmt, Expression(EmptyExpr), "", false, "");

			if (next_token.label == TokenLabel::KW_INOUT)
			{
				arg.view_FnArgs().update_is_inout(true);
				next_token = GetNextPeeked();
			}

			if (not CanBeTypeName(next_token.label))
			{
				throw; // Not a Type name in function's param list
			}

			arg.view_FnArgs().update_type_name(std::move(next_token.attr));

			next_token = GetNextPeeked();

			if (next_token.label != TokenLabel::IDENTIFIER)
			{
				throw; // Expected name after type
			}

			arg.view_FnArgs().update_var_name(std::move(next_token.attr));

			if (GetNextPeeked().label == TokenLabel::SYM_EQUAL)
			{
				auto expr = AnalyzeExpressionStatement();
				if (not expr.has_value())
				{
					throw; // expected expression after =
				}

				arg.view_FnArgs().update_default_expr(expr->view_ExprCall().expr());
			}

			args.push_back(std::move(arg));

			next_token = GetNextPeeked();
			if (next_token.label == TokenLabel::SYM_COMMA) { continue; }
			else if (next_token.label == TokenLabel::SYM_RPAR) { break; }
			else {
				throw; // expected next arg or ), not next_token.label
			}
		}

		peeked.clear();
		next = 0;

		if (GetNextPeeked().label != TokenLabel::SYM_ARROW)
		{
			throw; // expected -> return_type
		}

		Token ret_type_token = GetNextPeeked();

		if (not CanBeTypeName(ret_type_token.label))
		{
			throw; // expected valid return_type
		}

		if (GetNextPeeked().label != TokenLabel::SYM_LBRACE)
		{
			throw; // expected {
		}

		Statement fn_body = ConstructAST();

		if (GetNextPeeked().label != TokenLabel::SYM_RBRACE)
		{
			throw; // expected }
		}

		return Statement(MakeFunctionStmt, fn_body, args.begin(), args.end(), fn_name_token.attr, ret_type_token.attr);
	}

	std::optional<Statement> Parser::AnalyzeMakeStructStatement()
	{
		return std::optional<Statement>();
	}

	std::optional<Statement> Parser::AnalyzeMakePropertyStatement()
	{
		return std::optional<Statement>();
	}

	std::optional<Statement> Parser::AnalyzeMakeEnumStatement()
	{
		return std::optional<Statement>();
	}

	std::optional<Statement> Parser::AnalyzeMatchStatement()
	{
		return std::optional<Statement>();
	}

	std::optional<Statement> Parser::AnalyzeExpressionStatement()
	{
		return std::optional<Statement>();
	}

}