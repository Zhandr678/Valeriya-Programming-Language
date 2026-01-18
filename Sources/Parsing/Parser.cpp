#include "Parser.h"

#include "ParserException.h"

#pragma region("Helpers")

#define _Dupl(x) x, x

static bool CanBeTypeName(val::TokenLabel lbl)
{
	return lbl == val::TokenLabel::KW_INT || lbl == val::TokenLabel::KW_BOOL || lbl == val::TokenLabel::KW_STRING ||
		lbl == val::TokenLabel::KW_CHAR || lbl == val::TokenLabel::KW_DOUBLE || lbl == val::TokenLabel::IDENTIFIER;
}

static bool IsBinaryOperator(val::TokenLabel lbl)
{
	return lbl == val::TokenLabel::SYM_PLUS || lbl == val::TokenLabel::SYM_MINUS || lbl == val::TokenLabel::SYM_TIMES ||
		lbl == val::TokenLabel::SYM_DIV || lbl == val::TokenLabel::SYM_MOD || lbl == val::TokenLabel::SYM_OR ||
		lbl == val::TokenLabel::SYM_AND || lbl == val::TokenLabel::SYM_LOR || lbl == val::TokenLabel::SYM_LAND ||
		lbl == val::TokenLabel::SYM_XOR || lbl == val::TokenLabel::SYM_POW || lbl == val::TokenLabel::SYM_EQUAL ||
		lbl == val::TokenLabel::SYM_NEQ || lbl == val::TokenLabel::SYM_LESS || lbl == val::TokenLabel::SYM_GREATER ||
		lbl == val::TokenLabel::SYM_LEQ || lbl == val::TokenLabel::SYM_GEQ;
}

static int OpOrder(const val::TokenLabel& lbl)
{
	if (lbl == val::TokenLabel::SYM_EQUAL || lbl == val::TokenLabel::SYM_NEQ || lbl == val::TokenLabel::SYM_LESS ||
		lbl == val::TokenLabel::SYM_GREATER || lbl == val::TokenLabel::SYM_LEQ || lbl == val::TokenLabel::SYM_GEQ ||
		lbl == val::TokenLabel::SYM_LAND || lbl == val::TokenLabel::SYM_LOR)
	{
		return 0;
	}

	if (lbl == val::TokenLabel::SYM_PLUS || lbl == val::TokenLabel::SYM_MINUS || lbl == val::TokenLabel::SYM_AND ||
		lbl == val::TokenLabel::SYM_OR || lbl == val::TokenLabel::SYM_XOR)
	{
		return 1;
	}

	if (lbl == val::TokenLabel::SYM_TIMES || lbl == val::TokenLabel::SYM_DIV || lbl == val::TokenLabel::SYM_MOD)
	{
		return 2;
	}

	if (lbl == val::TokenLabel::SYM_POW)
	{
		return 3;
	}

	return 5;
}

static val::Expression GenerateExpression(const std::vector <val::Expression>& exprs, const std::vector <val::Token>& ops, size_t left, size_t right)
{
	if (left < 0 || right >= exprs.size()) { throw std::out_of_range("Index beyond vector of Expressions"); }
	if (left >= right) { return exprs[left]; }
	
	size_t main_op_index = 0, main_op_weight = 5;

	for (size_t i = left; i < right; i++)
	{
		int w = OpOrder(ops[i].label);
		if (w == 0)
		{
			auto left_expr = GenerateExpression(exprs, ops, left, i);
			auto right_expr = GenerateExpression(exprs, ops, i + 1, right);
			return val::Expression(val::BinaryExpr, left_expr, right_expr, ops[i].attr);
		}

		else if (w < main_op_weight)
		{
			main_op_index = i;
			main_op_weight = w;
		}
	}

	auto left_expr = GenerateExpression(exprs, ops, left, main_op_index);
	auto right_expr = GenerateExpression(exprs, ops, main_op_index + 1, right);
	return val::Expression(val::BinaryExpr, left_expr, right_expr, ops[main_op_index].attr);
}

static val::Expression FieldCallFromCallVector(const std::vector <std::string>& call_order, int i = 0)
{
	if (call_order.empty()) { return val::Expression(val::EmptyExpr); }
	if (i == call_order.size() - 1)
	{
		return val::Expression(val::VarNameExpr, call_order[i]);
	}

	return val::Expression(val::FieldCallExpr, val::Expression(val::VarNameExpr, call_order[i]), FieldCallFromCallVector(call_order, i + 1));
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

	Statement Parser::ConstructAST(TokenLabel flag)
	{
		std::vector <Statement> stms;

		Token next_token = GetNextPeeked();
		while (next_token.label != flag)
		{
			next = 0;
			if (auto st = AnalyzeAssignmentStatement()) {
				stms.push_back(*st);
			}
			else if (auto st = AnalyzeInitalizationStatement()) {
				stms.push_back(*st);
			}
			else if (auto st = AnalyzeReturnStatement()) {
				stms.push_back(*st);
			}
			else if (auto st = AnalyzeLoopCommands()) {
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
			else if (auto st = AnalyzeMakePropertyStatement()) {
				stms.push_back(*st);
			}
			else if (auto st = AnalyzeMatchStatement()) {
				stms.push_back(*st);
			}
			else {
				auto expr = ParseExpression(_Dupl(TokenLabel::SYM_SEMICOLON));

				if (not expr.has_value())
				{
					throw ParserException("Invalid Statement", GetFileName(), EmptyStmt, GetLine());
				}

				if (GetNextPeeked().label != TokenLabel::SYM_SEMICOLON)
				{
					throw ParserException("Expected Semicolon", GetFileName(), ExprCallStmt, GetLine());
				}

				ResetLookAhead();
				stms.emplace_back(ExprCallStmt, *expr);
			}
			next_token = GetNextPeeked();
		}
		next--;

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

		while (next.label == TokenLabel::COM_BLOCK || next.label == TokenLabel::COM_LINE)
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

	Expression Parser::ParseFieldCall()
	{
		Token called_field = GetNextPeeked();
		Token next_token = GetNextPeeked();

		if (called_field.label == TokenLabel::IDENTIFIER and next_token.label != TokenLabel::SYM_DOT)
		{
			next--;
			return Expression(VarNameExpr, called_field.attr);
		}

		else if (called_field.label == TokenLabel::IDENTIFIER and next_token.label == TokenLabel::SYM_DOT)
		{
			return Expression(FieldCallExpr, Expression(VarNameExpr, called_field.attr), ParseFieldCall());
		}

		else {
			throw ParserException(called_field.attr + " is not a Field Name", GetFileName(), ExprCallStmt, GetLine());
		}
	}

	std::optional<Statement> Parser::ParseForLoopFinalStatement()
	{
		Token next_token = GetNextPeeked();
		std::vector <Statement> assignments;
		while (next_token.label != TokenLabel::SYM_RPAR)
		{
			if (next_token.label != TokenLabel::IDENTIFIER)
			{
				throw ParserException("Expected Identifier", GetFileName(), ForLoopStmt, GetLine());
			}

			if (GetNextPeeked().label != TokenLabel::SYM_ASSIGN)
			{
				throw ParserException("Expected Assignment Statement", GetFileName(), ForLoopStmt, GetLine());
			}

			auto expr = ParseExpression(TokenLabel::SYM_COMMA, TokenLabel::SYM_RPAR);

			if (!expr.has_value())
			{
				throw ParserException("Expected Expression After '='", GetFileName(), ForLoopStmt, GetLine());
			}

			assignments.emplace_back(AssignmentStmt, *expr, Expression(VarNameExpr, next_token.attr));

			next_token = GetNextPeeked();
			if (next_token.label == TokenLabel::SYM_COMMA)
			{
				next_token = GetNextPeeked();
				continue;
			}
			else if (next_token.label == TokenLabel::SYM_RPAR)
			{
				next--;
				break;
			}
			else {
				throw ParserException("Expected Closing ')'", GetFileName(), ForLoopStmt, GetLine());
			}
		}

		if (assignments.empty()) { return std::nullopt; }
		ResetLookAhead();
		return Statement(BlockOfStmt, assignments.begin(), assignments.end());
	}

	std::optional<Statement> Parser::AnalyzeReturnStatement()
	{
		if (GetNextPeeked().label != TokenLabel::KW_RET)
		{
			next = 0;
			return std::nullopt;
		}

		if (GetNextPeeked().label == TokenLabel::SYM_SEMICOLON)
		{
			ResetLookAhead();
			return Statement(ReturnStmt, Expression(EmptyExpr));
		}

		next--;
		auto ret_expr = ParseExpression(_Dupl(TokenLabel::SYM_SEMICOLON));

		if (not ret_expr.has_value())
		{
			throw ParserException("Expected Expression", GetFileName(), ReturnStmt, GetLine());
		}

		if (GetNextPeeked().label != TokenLabel::SYM_SEMICOLON)
		{
			throw ParserException("Expected Semicolon", GetFileName(), ReturnStmt, GetLine());
		}

		ResetLookAhead();
		return Statement(ReturnStmt, *ret_expr);
	}

	std::optional<Statement> Parser::AnalyzeLoopCommands()
	{
		if (GetNextPeeked().label == TokenLabel::KW_CONTINUE)
		{
			if (GetNextPeeked().label != TokenLabel::SYM_SEMICOLON)
			{
				throw ParserException("Expected Semicolon", GetFileName(), ContinueStmt, GetLine());
			}
			ResetLookAhead();
			return Statement(ContinueStmt);
		}
		if (GetNextPeeked().label == TokenLabel::KW_BREAK)
		{
			if (GetNextPeeked().label != TokenLabel::SYM_SEMICOLON)
			{
				throw ParserException("Expected Semicolon", GetFileName(), BreakStmt, GetLine());
			}
			ResetLookAhead();
			return Statement(BreakStmt);
		}

		next = 0;
		return std::nullopt;
	}

	std::optional<Expression> Parser::ParseExpression(TokenLabel flag1, TokenLabel flag2)
	{
		Token next_token = GetNextPeeked();
		bool is_unary = false;

		if (next_token.label == TokenLabel::SYM_NOT)
		{
			is_unary = true;
			ResetLookAhead();
			auto unary_expr = ParseExpression(flag1, flag2);

			if (not unary_expr.has_value())
			{
				throw ParserException("Expected Expression After ![here]", GetFileName(), ExprCallStmt, GetLine()); // no expression
			}

			return Expression(UnaryExpr, *unary_expr, next_token.attr);
		}
		std::vector <Expression> single_exprs;
		std::vector <Token> operators;
		if (next_token.label == TokenLabel::SYM_MINUS)
		{
			is_unary = true;
			next_token = GetNextPeeked();
			if (next_token.label == TokenLabel::LIT_NUMBER)
			{
				single_exprs.emplace_back(DoubleLiteralExpr, -1 * std::stod(next_token.attr));
			}
			else if (next_token.label == TokenLabel::LIT_INTEGER)
			{
				single_exprs.emplace_back(IntLiteralExpr, -1 * std::stoi(next_token.attr));
			}
			else if (next_token.label == TokenLabel::IDENTIFIER)
			{
				Token name_token = std::move(next_token);
				next_token = GetNextPeeked();
				if (next_token.label == TokenLabel::SYM_LPAR)
				{
					next_token = GetNextPeeked();
					std::vector <Expression> args;
					while (next_token.label != TokenLabel::SYM_RPAR)
					{
						auto expr = ParseExpression(TokenLabel::SYM_COMMA, TokenLabel::SYM_RPAR);
						if (not expr.has_value())
						{
							ParserException("Expected Expression in Function Body After '('", GetFileName(), ExprCallStmt, GetLine());
						}
						args.push_back(*expr);

						next_token = GetNextPeeked();
						if (next_token.label == TokenLabel::SYM_COMMA)
						{
							next_token = GetNextPeeked();
							continue;
						}
						else if (next_token.label == TokenLabel::SYM_RPAR)
						{
							break;
						}
						else {
							throw ParserException("Expected Closing ')'", GetFileName(), ExprCallStmt, GetLine());
						}
					}

					single_exprs.emplace_back(UnaryExpr, Expression(FnCallExpr, args.begin(), args.end(), name_token.attr), "-");
				}
				else if (next_token.label == TokenLabel::SYM_SEMICOLON)
				{
					next--;
					single_exprs.emplace_back(UnaryExpr, Expression(VarNameExpr, name_token.attr), "-");
				}
				else {
					throw ParserException("Expected ';'", GetFileName(), ExprCallStmt, GetLine());
				}
			}
			else if (next_token.label == TokenLabel::SYM_LPAR)
			{
				auto par_expr = ParseExpression(_Dupl(TokenLabel::SYM_RPAR));
				if (not par_expr.has_value())
				{
					throw ParserException("Expected Expression After '('", GetFileName(), ExprCallStmt, GetLine());
				}
				if (GetNextPeeked().label != TokenLabel::SYM_RPAR)
				{
					throw ParserException("Expected Closing ')'", GetFileName(), ExprCallStmt, GetLine());
				}
				single_exprs.emplace_back(UnaryExpr, *par_expr, "-");
			}
			else {
				throw ParserException("Invalid Expression After MINUS symbol", GetFileName(), ExprCallStmt, GetLine());
			}
		}

		if (is_unary)
		{
			next_token = GetNextPeeked();

			if (IsBinaryOperator(next_token.label))
			{
				operators.push_back(next_token);
				next_token = GetNextPeeked();
			}
		}

		while (next_token.label != flag1 && next_token.label != flag2)
		{
			if (next_token.label == TokenLabel::SYM_LPAR)
			{
				auto par_expr = ParseExpression(_Dupl(TokenLabel::SYM_RPAR));

				if (not par_expr.has_value())
				{
					throw ParserException("Expected Expression ([here])!", GetFileName(), ExprCallStmt, GetLine()); // no expr in ()
				}

				if (GetNextPeeked().label != TokenLabel::SYM_RPAR)
				{
					throw ParserException("Expected Closing ')'", GetFileName(), ExprCallStmt, GetLine()); // expected closing ')'
				}

				single_exprs.push_back(*par_expr);

				next_token = GetNextPeeked();

				if (next_token.label == flag1 || next_token.label == flag2)
				{
					next--;
					break;
				}
				else if (IsBinaryOperator(next_token.label))
				{
					operators.push_back(next_token);
					next_token = GetNextPeeked();
					continue;
				}
				else { throw ParserException("Expression MUST NOT Follow Expression", GetFileName(), ExprCallStmt, GetLine()); } // expression followed by expression
			}

			else if (next_token.label == TokenLabel::LIT_TRUE || next_token.label == TokenLabel::LIT_FALSE)
			{
				single_exprs.emplace_back(BoolLiteralExpr, next_token.label == TokenLabel::LIT_TRUE);

				next_token = GetNextPeeked();

				if (next_token.label == flag1 || next_token.label == flag2)
				{
					next--;
					break;
				}
				else if (IsBinaryOperator(next_token.label))
				{
					operators.push_back(next_token);
					next_token = GetNextPeeked();
					continue;
				}
				else { throw ParserException("Expression MUST NOT Follow Expression", GetFileName(), ExprCallStmt, GetLine()); } // expression followed by expression
			}

			else if (next_token.label == TokenLabel::LIT_INTEGER)
			{
				single_exprs.emplace_back(IntLiteralExpr, std::stoi(next_token.attr));

				next_token = GetNextPeeked();

				if (next_token.label == flag1 || next_token.label == flag2)
				{
					next--;
					break;
				}
				else if (IsBinaryOperator(next_token.label))
				{
					operators.push_back(next_token);
					next_token = GetNextPeeked();
					continue;
				}
				else { throw ParserException("Expression MUST NOT Follow Expression", GetFileName(), ExprCallStmt, GetLine());; } // expression followed by expression
			}

			else if (next_token.label == TokenLabel::LIT_NUMBER)
			{
				single_exprs.emplace_back(DoubleLiteralExpr, std::stod(next_token.attr));

				next_token = GetNextPeeked();

				if (next_token.label == flag1 || next_token.label == flag2)
				{
					next--;
					break;
				}
				else if (IsBinaryOperator(next_token.label))
				{
					operators.push_back(next_token);
					next_token = GetNextPeeked();
					continue;
				}
				else { throw ParserException("Expression MUST NOT Follow Expression", GetFileName(), ExprCallStmt, GetLine());; } // expression followed by expression
			}

			else if (next_token.label == TokenLabel::LIT_STRING)
			{
				single_exprs.emplace_back(StringLiteralExpr, next_token.attr);

				next_token = GetNextPeeked();

				if (next_token.label == flag1 || next_token.label == flag2)
				{
					next--;
					break;
				}
				else if (IsBinaryOperator(next_token.label))
				{
					operators.push_back(next_token);
					next_token = GetNextPeeked();
					continue;
				}
				else { throw ParserException("Expression MUST NOT Follow Expression", GetFileName(), ExprCallStmt, GetLine());; } // expression followed by expression
			}
			else if (next_token.label == TokenLabel::LIT_CHAR)
			{
				single_exprs.emplace_back(CharLiteralExpr, next_token.attr[1]);

				next_token = GetNextPeeked();

				if (next_token.label == flag1 || next_token.label == flag2)
				{
					next--;
					break;
				}
				else if (IsBinaryOperator(next_token.label))
				{
					operators.push_back(next_token);
					next_token = GetNextPeeked();
					continue;
				}
				else { throw ParserException("Expression MUST NOT Follow Expression", GetFileName(), ExprCallStmt, GetLine());; } // expression followed by expression
			}
			else if (next_token.label == TokenLabel::IDENTIFIER)
			{
				Token name_token = std::move(next_token);
				next_token = GetNextPeeked();

				if (IsBinaryOperator(next_token.label))
				{
					single_exprs.emplace_back(VarNameExpr, name_token.attr);
					operators.push_back(next_token);
					next_token = GetNextPeeked();
				}
				else if (next_token.label == TokenLabel::SYM_LBRACE)
				{
					next_token = GetNextPeeked();
					std::vector <Expression> struct_inits;
					while (next_token.label != TokenLabel::SYM_RBRACE)
					{
						next--;
						auto expr = ParseExpression(TokenLabel::SYM_COMMA, TokenLabel::SYM_RBRACE);
						if (not expr.has_value())
						{
							throw ParserException("Expected Expression in Struct Initialization", GetFileName(), ExprCallStmt, GetLine());;
						}
						struct_inits.push_back(*expr);

						next_token = GetNextPeeked();
						if (next_token.label == TokenLabel::SYM_COMMA)
						{
							next_token = GetNextPeeked();
							continue;
						}
						else if (next_token.label == TokenLabel::SYM_RBRACE)
						{
							break;
						}
						else {
							throw ParserException("Expected Closing '}'", GetFileName(), ExprCallStmt, GetLine());;
						}
					}
					single_exprs.emplace_back(StructInitExpr, struct_inits.begin(), struct_inits.end(), name_token.attr);

					next_token = GetNextPeeked();

					if (next_token.label == flag1 || next_token.label == flag2)
					{
						next--;
						break;
					}
					else if (IsBinaryOperator(next_token.label))
					{
						operators.push_back(next_token);
						next_token = GetNextPeeked();
						continue;
					}
					else { throw ParserException("Expression MUST NOT Follow Expression", GetFileName(), ExprCallStmt, GetLine());; } // expression followed by expression
				}
				else if (next_token.label == TokenLabel::SYM_LPAR)
				{
					next_token = GetNextPeeked();
					std::vector <Expression> args;
					while (next_token.label != TokenLabel::SYM_RPAR)
					{
						next--;
						auto expr = ParseExpression(TokenLabel::SYM_COMMA, TokenLabel::SYM_RPAR);
						if (not expr.has_value())
						{
							throw ParserException("Expected Expression in Function Args", GetFileName(), ExprCallStmt, GetLine());;
						}
						args.push_back(*expr);

						next_token = GetNextPeeked();
						if (next_token.label == TokenLabel::SYM_COMMA)
						{
							next_token = GetNextPeeked();
							continue;
						}
						else if (next_token.label == TokenLabel::SYM_RPAR)
						{
							break;
						}
						else {
							throw ParserException("Expected Closing ')'", GetFileName(), ExprCallStmt, GetLine());;
						}
					}
					single_exprs.emplace_back(FnCallExpr, args.begin(), args.end(), name_token.attr);

					next_token = GetNextPeeked();

					if (next_token.label == flag1 || next_token.label == flag2)
					{
						next--;
						break;
					}
					else if (IsBinaryOperator(next_token.label))
					{
						operators.push_back(next_token);
						next_token = GetNextPeeked();
						continue;
					}
					else { throw ParserException("Expression MUST NOT Follow Expression", GetFileName(), ExprCallStmt, GetLine());; } // expression followed by expression
				}
				else if (next_token.label == flag1 || next_token.label == flag2)
				{
					single_exprs.emplace_back(VarNameExpr, name_token.attr);
					next--;
					break;
				}
				else if (next_token.label == TokenLabel::SYM_DOT)
				{
					single_exprs.emplace_back(FieldCallExpr, Expression(VarNameExpr, name_token.attr), ParseFieldCall());

					next_token = GetNextPeeked();

					if (next_token.label == flag1 || next_token.label == flag2)
					{
						next--;
						break;
					}
					else if (IsBinaryOperator(next_token.label))
					{
						operators.push_back(next_token);
						next_token = GetNextPeeked();
						continue;
					}
					else { throw ParserException("Expression MUST NOT Follow Expression", GetFileName(), ExprCallStmt, GetLine());; } // expression followed by expression
				}
				else {
					throw ParserException("Invalid Symbol After Identifier", GetFileName(), ExprCallStmt, GetLine());;
				}
			}
			else {
				throw ParserException("Unknown Expression Type", GetFileName(), ExprCallStmt, GetLine());;
			}
		}

		if (single_exprs.empty()) { return std::nullopt; }

		/*for (size_t i = 0; i < operators.size(); i++)
		{
			std::cout << single_exprs[i].sel() << " " << operators[i] << " ";
		}
		std::cout << (single_exprs.end() - 1)->sel();*/

		return GenerateExpression(single_exprs, operators, 0, operators.size());
	}

	std::optional<Statement> Parser::AnalyzeInitalizationStatement()
	{
		Token type_token = GetNextPeeked();

		if (not CanBeTypeName(type_token.label))
		{
			next = 0;
			return std::nullopt;
		}

		Token next_token = GetNextPeeked();
		if (next_token.label == TokenLabel::SYM_LPAR || next_token.label == TokenLabel::SYM_LBRACE || next_token.label == TokenLabel::SYM_LBRACKET)
		{
			next = 0;
			return std::nullopt;
		}
		next--;
		
		std::vector <Statement> var_inits;
		
		next_token = GetNextPeeked();
		while (next_token.label != TokenLabel::SYM_SEMICOLON)
		{
			Token varname_token = std::move(next_token);

			if (varname_token.label != TokenLabel::IDENTIFIER)
			{
				throw ParserException("Bad Name for a Variable", GetFileName(), VarInitStmt, GetLine()); // initialization of variable with forbidden name;
			}

			next_token = GetNextPeeked();

			if (next_token.label == TokenLabel::SYM_COMMA)
			{
				var_inits.push_back(Statement(VarInitStmt, Expression(EmptyExpr), varname_token.attr, type_token.attr));
				next_token = GetNextPeeked();
				ResetLookAhead();
				continue;
			}

			if (next_token.label == TokenLabel::SYM_SEMICOLON)
			{
				var_inits.push_back(Statement(VarInitStmt, Expression(EmptyExpr), varname_token.attr, type_token.attr));
				ResetLookAhead();
				break;
			}

			else if (next_token.label == TokenLabel::SYM_ASSIGN)
			{
				auto expr = ParseExpression(TokenLabel::SYM_SEMICOLON, TokenLabel::SYM_COMMA);

				if (not expr.has_value()) 
				{ 
					throw ParserException("Expected Expression", GetFileName(), VarInitStmt, GetLine()); // no expression after equal symbol
				}
				
				var_inits.push_back(Statement(VarInitStmt, *expr, varname_token.attr, type_token.attr));
				// ResetLookAhead();
				
				next_token = GetNextPeeked();
				if (next_token.label == TokenLabel::SYM_SEMICOLON) { break; }
				else if (next_token.label == TokenLabel::SYM_COMMA) { next_token = GetNextPeeked(); }
				else { throw ParserException("Expected Semicolon", GetFileName(), VarInitStmt, GetLine()); }
			}
			else { throw ParserException("Expected Semicolon", GetFileName(), VarInitStmt, GetLine()); }
		}

		if (var_inits.empty()) 
		{ 
			return std::nullopt;
		}

		ResetLookAhead();
		return Statement(BlockOfStmt, var_inits.begin(), var_inits.end());
	}

	std::optional<Statement> Parser::AnalyzeAssignmentStatement()
	{
		Token varname_token = GetNextPeeked();

		if (varname_token.label != TokenLabel::IDENTIFIER)
		{
			next = 0;
			return std::nullopt;
		}

		Token next_token = GetNextPeeked();
		if (next_token.label == TokenLabel::SYM_ASSIGN)
		{
			auto expr = ParseExpression(_Dupl(TokenLabel::SYM_SEMICOLON));

			if (not expr.has_value())
			{
				throw ParserException("Expected Expression", GetFileName(), AssignmentStmt, GetLine()); // no expression after equal symbol
			}

			if (GetNextPeeked().label != TokenLabel::SYM_SEMICOLON)
			{
				throw ParserException("Expected Semicolon", GetFileName(), AssignmentStmt, GetLine()); // need semicolon
			}

			ResetLookAhead();
			return Statement(AssignmentStmt, *expr, Expression(VarNameExpr, varname_token.attr));
		}
		else if (next_token.label == TokenLabel::SYM_DOT)
		{
			next_token = GetNextPeeked();

			std::vector <std::string> call_order;

			if (next_token.label != TokenLabel::IDENTIFIER)
			{
				throw ParserException(next_token.attr + " is not a Field Name", GetFileName(), AssignmentStmt, GetLine());
			}

			call_order.push_back(varname_token.attr);
			call_order.push_back(next_token.attr);
			next_token = GetNextPeeked();

			while (next_token.label == TokenLabel::SYM_DOT)
			{
				next_token = GetNextPeeked();

				if (next_token.label != TokenLabel::IDENTIFIER)
				{
					throw ParserException(next_token.attr + " is not a Field Name", GetFileName(), AssignmentStmt, GetLine());
				}

				call_order.push_back(next_token.attr);
				next_token = GetNextPeeked();
			}

			if (next_token.label != TokenLabel::SYM_ASSIGN)
			{
				throw ParserException("Expected '='", GetFileName(), AssignmentStmt, GetLine());
			}

			auto expr = ParseExpression(_Dupl(TokenLabel::SYM_SEMICOLON));

			if (not expr.has_value())
			{
				throw ParserException("Expected Expression", GetFileName(), AssignmentStmt, GetLine()); // no expression after equal symbol
			}

			if (GetNextPeeked().label != TokenLabel::SYM_SEMICOLON)
			{
				throw ParserException("Expected Semicolon", GetFileName(), AssignmentStmt, GetLine()); // need semicolon
			}

			Expression dest = FieldCallFromCallVector(call_order);

			ResetLookAhead();
			return Statement(AssignmentStmt, *expr, dest);
		}
		else {
			next = 0;
			return std::nullopt;
		}
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
			throw ParserException("Expected '(' after 'if'", GetFileName(), ConditionStmt, GetLine()); // no ( after if
		}

		auto expr = ParseExpression(_Dupl(TokenLabel::SYM_RPAR));
		
		if (not expr.has_value())
		{
			throw ParserException("Expected Expression", GetFileName(), ConditionStmt, GetLine()); // no expression inside if
		}

		if (GetNextPeeked().label != TokenLabel::SYM_RPAR)
		{
			throw ParserException("Expected ')' after 'if (expr'", GetFileName(), ConditionStmt, GetLine()); // no ) after if (expr
		}

		if (GetNextPeeked().label != TokenLabel::SYM_LBRACE)
		{
			throw ParserException("Expected Body of 'if'", GetFileName(), ConditionStmt, GetLine()); // no { after if (expr)
		}

		ResetLookAhead();
		Statement if_body = ConstructAST(TokenLabel::SYM_RBRACE);
		
		if (GetNextPeeked().label != TokenLabel::SYM_RBRACE)
		{
			throw ParserException("Expected Closing '}'", GetFileName(), ConditionStmt, GetLine()); // no } after if (expr) { stmts 
		}
		
		ResetLookAhead();
		std::vector <Statement> elifs;
		Token next_token = GetNextPeeked();
		while (next_token.label == TokenLabel::KW_ELIF)
		{
			if (GetNextPeeked().label != TokenLabel::SYM_LPAR)
			{
				throw ParserException("Expected '(' after 'elif'", GetFileName(), ConditionStmt, GetLine()); // elif (!
			}

			auto elif_expr = ParseExpression(_Dupl(TokenLabel::SYM_RPAR));

			if (GetNextPeeked().label != TokenLabel::SYM_RPAR)
			{
				throw ParserException("Expected ')' after 'elif(expr'", GetFileName(), ConditionStmt, GetLine()); // no expression in elif()
			}

			if (GetNextPeeked().label != TokenLabel::SYM_LBRACE)
			{
				throw ParserException("Expected Body of 'elif'", GetFileName(), ConditionStmt, GetLine()); // elif (!
			}

			ResetLookAhead();
			Statement elif_body = ConstructAST(TokenLabel::SYM_RBRACE);

			if (GetNextPeeked().label != TokenLabel::SYM_RBRACE)
			{
				throw ParserException("Expected Closing '}'", GetFileName(), ConditionStmt, GetLine()); // elif () {}!
			}

			elifs.push_back(Statement(ElifConditionStmt, *elif_expr, elif_body));
			ResetLookAhead();
			next_token = GetNextPeeked();
		}

		Statement else_body(EmptyStmt);

		if (next_token.label == TokenLabel::KW_ELSE)
		{
			if (GetNextPeeked().label != TokenLabel::SYM_LBRACE)
			{
				throw ParserException("Expected Body of 'else'", GetFileName(), ConditionStmt, GetLine()); // else {!
			}

			ResetLookAhead();
			else_body = ConstructAST(TokenLabel::SYM_RBRACE);

			if (GetNextPeeked().label != TokenLabel::SYM_RBRACE)
			{
				throw ParserException("Expected Closing '}'", GetFileName(), ConditionStmt, GetLine()); // else {}!
			}
		}

		else { next--; } // very bad!
		return Statement(ConditionStmt, *expr, if_body, else_body, elifs.begin(), elifs.end());
	}

	std::optional<Statement> Parser::AnalyzeForLoopStatement()
	{
		if (GetNextPeeked().label != TokenLabel::KW_FOR)
		{
			next = 0;
			return std::nullopt;
		}

		if (GetNextPeeked().label != TokenLabel::SYM_LPAR)
		{
			throw ParserException("Expected '(' after 'for'", GetFileName(), ForLoopStmt, GetLine());
		}

		auto init_part = AnalyzeInitalizationStatement();

		auto check_part = ParseExpression(_Dupl(TokenLabel::SYM_SEMICOLON));

		if (GetNextPeeked().label != TokenLabel::SYM_SEMICOLON)
		{
			throw ParserException("Expected ';'", GetFileName(), ForLoopStmt, GetLine());
		}

		auto final_stmt = ParseForLoopFinalStatement();

		if (GetNextPeeked().label != TokenLabel::SYM_RPAR)
		{
			throw ParserException("Expected ')'", GetFileName(), ForLoopStmt, GetLine());
		}

		if (GetNextPeeked().label != TokenLabel::SYM_LBRACE)
		{
			throw ParserException("Expected Body of a 'for'", GetFileName(), ForLoopStmt, GetLine());
		}

		ResetLookAhead();
		auto for_body = ConstructAST(TokenLabel::SYM_RBRACE);

		if (GetNextPeeked().label != TokenLabel::SYM_RBRACE)
		{
			throw ParserException("Expected Closing '}'", GetFileName(), ForLoopStmt, GetLine());
		}

		ResetLookAhead();
		return Statement(ForLoopStmt, init_part.value_or(Statement(EmptyStmt)), check_part.value_or(Expression(EmptyExpr)), final_stmt.value_or(Statement(EmptyStmt)), for_body);
	}

	std::optional<Statement> Parser::AnalyzeWhileLoopStatement()
	{
		if (GetNextPeeked().label != TokenLabel::KW_WHILE)
		{
			next = 0;
			return std::nullopt;
		}

		if (GetNextPeeked().label != TokenLabel::SYM_LPAR)
		{
			throw ParserException("Expected '(' after 'while'", GetFileName(), WhileLoopStmt, GetLine()); // expected ( after while
		}

		auto expr = ParseExpression(_Dupl(TokenLabel::SYM_RPAR));

		if (not expr.has_value())
		{
			throw ParserException("Expected Expression", GetFileName(), WhileLoopStmt, GetLine()); // no expression in while
		}

		if (GetNextPeeked().label != TokenLabel::SYM_RPAR)
		{
			throw ParserException("Expected ')' after 'while(expr'", GetFileName(), WhileLoopStmt, GetLine()); // expected ) after while (expr
		}

		if (GetNextPeeked().label != TokenLabel::SYM_LBRACE)
		{
			throw ParserException("Expected Body of 'while'", GetFileName(), WhileLoopStmt, GetLine()); // expected { after while (expr)
		}

		ResetLookAhead();
		Statement while_body = ConstructAST(TokenLabel::SYM_RBRACE);

		if (GetNextPeeked().label != TokenLabel::SYM_RBRACE)
		{
			throw ParserException("Expected Closing '}'", GetFileName(), WhileLoopStmt, GetLine()); // expected } after while (expr) {
		}

		ResetLookAhead();
		return Statement(WhileLoopStmt, *expr, while_body);
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
			throw ParserException("Bad Name for a Function", GetFileName(), MakeFunctionStmt, GetLine()); // Forbidden name for function
		}

		if (GetNextPeeked().label != TokenLabel::SYM_LPAR)
		{
			throw ParserException("Expected Param List for a Function", GetFileName(), MakeFunctionStmt, GetLine()); // need ( after function name
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
				throw ParserException("Not a Type Name in a Function's Param List", GetFileName(), MakeFunctionStmt, GetLine()); // Not a Type name in function's param list
			}

			arg.view_FnArgs().update_type_name(std::move(next_token.attr));

			next_token = GetNextPeeked();

			if (next_token.label != TokenLabel::IDENTIFIER)
			{
				throw ParserException("Bad Name For a Function Param", GetFileName(), MakeFunctionStmt, GetLine()); // Expected name after type
			}

			arg.view_FnArgs().update_var_name(std::move(next_token.attr));
			
			next_token = GetNextPeeked();
			if (next_token.label == TokenLabel::SYM_EQUAL)
			{
				auto expr = ParseExpression(_Dupl(TokenLabel::SYM_SEMICOLON));
				if (not expr.has_value())
				{
					throw ParserException("Expected Expression", GetFileName(), MakeFunctionStmt, GetLine()); // expected expression after =
				}

				arg.view_FnArgs().update_default_expr(*expr);
			}

			args.push_back(std::move(arg));
			
			if (next_token.label == TokenLabel::SYM_COMMA) {
				next_token = GetNextPeeked();
				continue; 
			}
			else if (next_token.label == TokenLabel::SYM_RPAR) { break; }
			else {
				throw ParserException("Expected Next Arg or ')'", GetFileName(), MakeFunctionStmt, GetLine()); // expected next arg or ), not next_token.label
			}
		}

		ResetLookAhead();

		if (GetNextPeeked().label != TokenLabel::SYM_ARROW)
		{
			throw ParserException("Expected '-> return_type'", GetFileName(), MakeFunctionStmt, GetLine()); // expected -> return_type
		}

		Token ret_type_token = GetNextPeeked();

		if (not CanBeTypeName(ret_type_token.label) && ret_type_token.label != TokenLabel::KW_VOID)
		{
			throw ParserException("Not a Return Type Name", GetFileName(), MakeFunctionStmt, GetLine()); // expected valid return_type
		}

		if (GetNextPeeked().label != TokenLabel::SYM_LBRACE)
		{
			throw ParserException("Expected Body of 'fn'", GetFileName(), MakeFunctionStmt, GetLine()); // expected {
		}

		ResetLookAhead();
		Statement fn_body = ConstructAST(TokenLabel::SYM_RBRACE);

		if (GetNextPeeked().label != TokenLabel::SYM_RBRACE)
		{
			throw ParserException("Expected Closing '}'", GetFileName(), MakeFunctionStmt, GetLine()); // expected }
		}

		ResetLookAhead();
		return Statement(MakeFunctionStmt, fn_body, args.begin(), args.end(), fn_name_token.attr, ret_type_token.attr);
	}

	std::optional<Statement> Parser::AnalyzeMakeStructStatement()
	{
		if (GetNextPeeked().label != TokenLabel::KW_STRUCT)
		{
			next = 0;
			return std::nullopt;
		}

		Token struct_name_token = GetNextPeeked();

		if (struct_name_token.label != TokenLabel::IDENTIFIER)
		{
			throw ParserException("Bad Name For a Struct", GetFileName(), MakeStructStmt, GetLine()); // forbidden struct name
		}

		if (GetNextPeeked().label != TokenLabel::SYM_LBRACE)
		{
			throw ParserException("Expected Body of 'struct'", GetFileName(), MakeStructStmt, GetLine()); // expected { after struct
		}
		
		std::vector <Statement> var_inits;

		while (GetNextPeeked().label != TokenLabel::SYM_RBRACE)
		{
			next--;
			auto var_init_block = AnalyzeInitalizationStatement();

			if (not var_init_block.has_value())
			{
				throw ParserException("Struct Fields Error", GetFileName(), MakeStructStmt, GetLine()); // no init statements
			}

			for (size_t i = 0; i < var_init_block->view_Block().size(); i++)
			{
				var_inits.push_back(var_init_block->view_Block().statements(i));
			}
		}
		next--;
		
		if (GetNextPeeked().label != TokenLabel::SYM_RBRACE)
		{
			throw ParserException("Expected Closing '}'", GetFileName(), MakeStructStmt, GetLine()); // expected } after struct {
		}

		ResetLookAhead();
		return Statement(MakeStructStmt, var_inits.begin(), var_inits.end(), struct_name_token.attr);
	}

	std::optional<Statement> Parser::AnalyzeMakePropertyStatement()
	{
		if (GetNextPeeked().label != TokenLabel::KW_PROP)
		{
			next = 0;
			return std::nullopt;
		}

		Token prop_name_token = GetNextPeeked();

		if (prop_name_token.label != TokenLabel::IDENTIFIER)
		{
			throw ParserException("Bad Name For a Property", GetFileName(), MakePropertyStmt, GetLine()); // forbidden name for property
		}

		if (GetNextPeeked().label != TokenLabel::SYM_LBRACE)
		{
			throw ParserException("Expected Body of a Property", GetFileName(), MakePropertyStmt, GetLine()); // expected { after property
		}

		ResetLookAhead();
		std::vector <Statement> prop_opts;

		Token next_token = GetNextPeeked();
		while (next_token.label != TokenLabel::SYM_RBRACE)
		{
			if (next_token.label != TokenLabel::IDENTIFIER)
			{
				throw ParserException("Bad Name For a Option", GetFileName(), MakePropertyStmt, GetLine()); // not option name
			}

			if (GetNextPeeked().label != TokenLabel::SYM_COLON)
			{
				throw ParserException("Expected ':' after Option Name", GetFileName(), MakePropertyStmt, GetLine()); // option :!
			}

			if (GetNextPeeked().label != TokenLabel::SYM_LBRACE)
			{
				throw ParserException("Expected Body of an Option", GetFileName(), MakePropertyStmt, GetLine()); // option : {!
			}

			std::vector <Statement> opt_inits;

			while (GetNextPeeked().label != TokenLabel::SYM_RBRACE)
			{
				next--;
				auto opt_init_block = AnalyzeInitalizationStatement();

				if (not opt_init_block.has_value())
				{
					throw ParserException("Option Fields Error", GetFileName(), MakeStructStmt, GetLine()); // no init statements
				}

				for (size_t i = 0; i < opt_init_block->view_Block().size(); i++)
				{
					opt_inits.push_back(opt_init_block->view_Block().statements(i));
				}
			}
			next--;

			if (GetNextPeeked().label != TokenLabel::SYM_RBRACE)
			{
				throw ParserException("Expected Closing '}' but got", GetFileName(), MakePropertyStmt, GetLine()); // option : { ... }!
			}

			prop_opts.push_back(Statement(MakeStructStmt, opt_inits.begin(), opt_inits.end(), next_token.attr));
			next_token = GetNextPeeked();
		}

		ResetLookAhead();
		return Statement(MakePropertyStmt, prop_opts.begin(), prop_opts.end(), prop_name_token.attr);
	}

	std::optional<Statement> Parser::AnalyzeMakeEnumStatement()
	{
		if (GetNextPeeked().label != TokenLabel::KW_ENUM)
		{
			next = 0;
			return std::nullopt;
		}

		Token enum_name_token = GetNextPeeked();

		if (enum_name_token.label != TokenLabel::IDENTIFIER)
		{
			throw ParserException("Bad Enum Name", GetFileName(), MakeEnumStmt, GetLine()); // forbidden name for enum
		}

		if (GetNextPeeked().label != TokenLabel::SYM_LBRACE)
		{
			throw ParserException("Expected Body of an 'enum'", GetFileName(), MakeEnumStmt, GetLine()); // enum {!
		}

		ResetLookAhead();
		std::vector <std::string> enum_options;

		Token next_token = GetNextPeeked();
		while (next_token.label != TokenLabel::SYM_RBRACE)
		{
			if (next_token.label != TokenLabel::IDENTIFIER)
			{
				throw ParserException("Bad Enum Option Name", GetFileName(), MakeEnumStmt, GetLine()); // not enum option name
			}

			enum_options.push_back(next_token.attr);

			next_token = GetNextPeeked();
			if (next_token.label == TokenLabel::SYM_COMMA) 
			{ 
				next_token = GetNextPeeked();
				// , } - is it fine?
				continue; 
			}
			else if (next_token.label == TokenLabel::SYM_RBRACE) { break; }
			else {
				throw ParserException("Expected Closing '}'", GetFileName(), MakeEnumStmt, GetLine()); // expected next arg or }, not next_token.label
			}
		}

		ResetLookAhead();
		return Statement(MakeEnumStmt, enum_options.begin(), enum_options.end(), enum_name_token.attr);
	}

	std::optional<Statement> Parser::AnalyzeMatchStatement()
	{
		if (GetNextPeeked().label != TokenLabel::KW_MATCH)
		{
			next = 0;
			return std::nullopt;
		}

		if (GetNextPeeked().label != TokenLabel::SYM_LPAR)
		{
			throw ParserException("Expected Matching Variable", GetFileName(), MatchStmt, GetLine()); // match (!
		}

		auto matched_expr = ParseExpression(_Dupl(TokenLabel::SYM_RPAR));
		if (not matched_expr.has_value())
		{
			throw ParserException("Expected Expression", GetFileName(), MatchStmt, GetLine());
		}

		if (GetNextPeeked().label != TokenLabel::SYM_RPAR)
		{
			throw ParserException("Expected ')' after 'match (v'", GetFileName(), MatchStmt, GetLine()); // match (var)!
		}

		if (GetNextPeeked().label != TokenLabel::SYM_LBRACE)
		{
			throw ParserException("Expected Body of a 'match'", GetFileName(), MatchStmt, GetLine()); // match (var) {!
		}
		
		std::vector <Statement> case_clauses;
		
		ResetLookAhead();
		Token next_token = GetNextPeeked();
		while (next_token.label != TokenLabel::SYM_RBRACE)
		{
			if (next_token.label != TokenLabel::KW_CASE)
			{
				throw ParserException("Expected Case Clause", GetFileName(), MatchStmt, GetLine()); // expected case
			}

			bool is_wildcard = false;
			std::optional <Expression> case_expr;

			if (GetNextPeeked().label == TokenLabel::KW_WILDCARD)
			{
				is_wildcard = true;
			}
			else 
			{
				next--;
				case_expr = ParseExpression(_Dupl(TokenLabel::SYM_ARROW));

				if (not case_expr.has_value())
				{
					throw ParserException("Expected Expression", GetFileName(), MatchStmt, GetLine());;
				}
			}

			if (GetNextPeeked().label != TokenLabel::SYM_ARROW)
			{
				throw ParserException("Expected '->'", GetFileName(), MatchStmt, GetLine()); // case option ->!
			}

			if (GetNextPeeked().label != TokenLabel::SYM_LBRACE)
			{
				throw ParserException("Expected Body of 'case'", GetFileName(), MatchStmt, GetLine()); // case option -> {!
			}

			ResetLookAhead();
			Statement case_block = ConstructAST(TokenLabel::SYM_RBRACE);

			next_token = GetNextPeeked();
			if (next_token.label != TokenLabel::SYM_RBRACE)
			{
				throw ParserException("Expected Closing '}'", GetFileName(), MatchStmt, GetLine());
			}

			ResetLookAhead();
			case_clauses.push_back(Statement(CaseClauseStmt, case_block, case_expr.value_or(Expression(EmptyExpr)), is_wildcard));
			next_token = GetNextPeeked();
		}

		ResetLookAhead();
		return Statement(MatchStmt, *matched_expr, case_clauses.begin(), case_clauses.end());
	}

}