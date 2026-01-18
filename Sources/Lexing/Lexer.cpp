#include "Lexer.h"

#include <cctype>
#include <algorithm>

#include "LexerException.h"

#pragma region("Helpers")

#define _CurToken    this->inp.view(this->token_size)
#define _CurLocation Location{ this->inp.filename, this->inp.line + 1, this->inp.column }
#define _LastChar    this->inp.view(this->token_size + 1)[this->token_size]

static bool IsWhitespace(char c)
{
	return c == ' ' || c == '\n' || c == '\t' || c == '\v' || c == '\r';
}

static bool IsIdentStart(char c)
{
	return std::isalpha(c) || c == '_';
}

static bool IsIdent(char c)
{
	return std::isalnum(c) || c == '_';
}

namespace val
{

	bool Lexer::Has(size_t next)
	{
		if (next <= view_capacity) { return true; }

		if (inp.has(next))
		{
			view_capacity = next;
			return true;
		}

		return false;
	}

	void Lexer::Commit(size_t next)
	{
		inp.commit(std::min(next, view_capacity));
		view_capacity = 0;
		token_size = 0;
	}

#pragma endregion



	std::optional<Token> Lexer::AnalyzeNumbers()
	{
		if (!Has(1) || !std::isdigit(_LastChar))
			return std::nullopt;

		bool dot = false;
		bool exp = false;
		bool prev_was_exp = false;

		token_size++;

		while (Has(token_size + 1))
		{
			char c = _LastChar;
			token_size++;

			if (std::isdigit(c))
			{
				prev_was_exp = false;
			}
			else if (c == '.' && !exp)
			{
				if (dot)
				{
					Commit(token_size);
					throw LexerException("Multiple dots in number", _CurLocation);
				}
				dot = true;
				prev_was_exp = false;
			}
			else if (c == 'e' || c == 'E')
			{
				if (exp)
				{
					Commit(token_size);
					throw LexerException("Multiple exponent symbols", _CurLocation);
				}
				exp = true;
				prev_was_exp = true;
			}
			else if ((c == '+' || c == '-') && prev_was_exp)
			{
				prev_was_exp = false;
			}
			else if (std::isalpha(c))
			{
				Commit(token_size);
				throw LexerException("Identifier starting with number", _CurLocation);
			}
			else
			{
				token_size--;
				break;
			}
		}

		if (prev_was_exp)
		{
			Commit(token_size);
			throw LexerException("Exponent has no digits", _CurLocation);
		}

		TokenLabel label = (!dot && !exp) ? TokenLabel::LIT_INTEGER : TokenLabel::LIT_NUMBER;

		Token tok{ label, std::string(_CurToken), _CurLocation };
		Commit(token_size);
		return tok;
	}


	std::optional<Token> Lexer::AnalyzeLiterals()
	{
		if (not Has(1) || _LastChar != '"') { return std::nullopt; }
		token_size++;

		while (Has(token_size + 1))
		{
			char c = _LastChar;
			token_size++;

			if (c == '"')
			{
				Token tok{
					TokenLabel::LIT_STRING,
					std::string(_CurToken),
					_CurLocation
				};
				Commit(token_size);
				return tok;
			}

			if (c == '\n')
			{
				Commit(token_size);
				throw LexerException("Unterminated String Literal", _CurLocation); // unterminated string
			}
		}

		Commit(token_size);
		throw LexerException("Unterminated String Literal at EOF", _CurLocation); // EOF before closing quote
	}

	std::optional<Token> Lexer::AnalyzeKeywords()
	{
		if (not Has(1) || not IsIdentStart(_LastChar)) { return std::nullopt; }

		while (Has(token_size + 1) && IsIdent(_LastChar)) { token_size++; }

		std::string_view text = _CurToken;
		TokenLabel label = TokenLabel::IDENTIFIER;

		// Keywords
		if (text == "int")           label = TokenLabel::KW_INT;
		else if (text == "double")   label = TokenLabel::KW_DOUBLE;
		else if (text == "string")   label = TokenLabel::KW_STRING;
		else if (text == "bool")     label = TokenLabel::KW_BOOL;
		else if (text == "char")     label = TokenLabel::KW_CHAR;
		else if (text == "void")     label = TokenLabel::KW_VOID;

		else if (text == "if")       label = TokenLabel::KW_IF;
		else if (text == "elif")     label = TokenLabel::KW_ELIF;
		else if (text == "else")     label = TokenLabel::KW_ELSE;

		else if (text == "for")      label = TokenLabel::KW_FOR;
		else if (text == "while")    label = TokenLabel::KW_WHILE;

		else if (text == "continue") label = TokenLabel::KW_CONTINUE;
		else if (text == "break")    label = TokenLabel::KW_BREAK;

		else if (text == "struct")   label = TokenLabel::KW_STRUCT;
		else if (text == "enum")     label = TokenLabel::KW_ENUM;

		else if (text == "property") label = TokenLabel::KW_PROP;

		else if (text == "match")    label = TokenLabel::KW_MATCH;
		else if (text == "case")     label = TokenLabel::KW_CASE;
		else if (text == "_")        label = TokenLabel::KW_WILDCARD;

		else if (text == "return")   label = TokenLabel::KW_RET;
		else if (text == "fn")       label = TokenLabel::KW_FUNC;
		else if (text == "inout")    label = TokenLabel::KW_INOUT;
		else if (text == "true")     label = TokenLabel::LIT_TRUE;
		else if (text == "false")    label = TokenLabel::LIT_FALSE;
		else if (text == "null")     label = TokenLabel::LIT_NULL;

		Token tok{ label, std::string(text), _CurLocation };

		Commit(token_size);
		return tok;
	}


	std::optional<Token> Lexer::AnalyzeSymbols()
	{
		if (not Has(1)) { return std::nullopt; }

		// Two-character operators
		if (Has(2))
		{
			std::string_view v = inp.view(2);

			if (v == "->") return Commit(2), Token{ TokenLabel::SYM_ARROW,  "->", _CurLocation };
			if (v == "==") return Commit(2), Token{ TokenLabel::SYM_EQUAL,  "==", _CurLocation };
			if (v == "!=") return Commit(2), Token{ TokenLabel::SYM_NEQ,    "!=", _CurLocation };
			if (v == "<=") return Commit(2), Token{ TokenLabel::SYM_LEQ,    "<=", _CurLocation };
			if (v == ">=") return Commit(2), Token{ TokenLabel::SYM_GEQ,    ">=", _CurLocation };
			if (v == "&&") return Commit(2), Token{ TokenLabel::SYM_LAND,   "&&", _CurLocation };
			if (v == "||") return Commit(2), Token{ TokenLabel::SYM_LOR,    "||", _CurLocation };
			if (v == "**") return Commit(2), Token{ TokenLabel::SYM_POW,    "**", _CurLocation };
		}

		// Single-character operators
		char c = _LastChar;
		TokenLabel label;

		switch (c)
		{
		case '{': label = TokenLabel::SYM_LBRACE;     break;
		case '}': label = TokenLabel::SYM_RBRACE;     break;
		case '(': label = TokenLabel::SYM_LPAR;       break;
		case ')': label = TokenLabel::SYM_RPAR;       break;
		case '[': label = TokenLabel::SYM_LBRACKET;   break;
		case ']': label = TokenLabel::SYM_RBRACKET;   break;
		case ';': label = TokenLabel::SYM_SEMICOLON;  break;
		case ':': label = TokenLabel::SYM_COLON;      break;
		case ',': label = TokenLabel::SYM_COMMA;      break;
		case '.': label = TokenLabel::SYM_DOT;        break;
		case '+': label = TokenLabel::SYM_PLUS;       break;
		case '-': label = TokenLabel::SYM_MINUS;      break;
		case '*': label = TokenLabel::SYM_TIMES;      break;
		case '/': label = TokenLabel::SYM_DIV;        break;
		case '%': label = TokenLabel::SYM_MOD;        break;
		case '^': label = TokenLabel::SYM_XOR;        break;
		case '&': label = TokenLabel::SYM_AND;        break;
		case '|': label = TokenLabel::SYM_OR;         break;
		case '!': label = TokenLabel::SYM_NOT;        break;
		case '=': label = TokenLabel::SYM_ASSIGN;     break;
		case '<': label = TokenLabel::SYM_LESS;       break;
		case '>': label = TokenLabel::SYM_GREATER;    break;
		default:
			return std::nullopt;
		}

		Token token{ label, std::string(1, c), _CurLocation };
		Commit(1);
		return token;
	}

	std::optional<Token> Lexer::AnalyzeComments()
	{
		if (not Has(2)) { return std::nullopt; }

		// Line comment
		if (inp.view(2) == "//")
		{
			token_size = 2;
			while (Has(token_size + 1))
			{
				if (_LastChar == '\n') { break; }
				token_size++;
			}

			Token token{ TokenLabel::COM_LINE, std::string(_CurToken), _CurLocation };
			Commit(token_size);
			return token;
		}

		// Block comment
		if (inp.view(2) == "/*")
		{
			token_size = 2;
			bool star = false;

			while (Has(token_size + 1))
			{
				char c = _LastChar;
				token_size++;
				if (c == '*') { star = true; }
				else if (c == '/' && star)
				{
					Token token{ TokenLabel::COM_BLOCK, std::string(_CurToken), _CurLocation };
					Commit(token_size);
					return token;
				}
				else { star = false; }
			}

			Commit(token_size);
			throw LexerException("Unterminated Block Comment", _CurLocation); // unterminated block comment
		}

		return std::nullopt;
	}

	std::optional<Token> Lexer::AnalyzeChars()
	{
		if (not Has(1) || _LastChar != '\'') { return std::nullopt; }

		token_size++;

		if (!Has(token_size + 1))
		{
			Commit(token_size);
			throw LexerException("Unterminated Character Literal", _CurLocation);
		}

		char c = _LastChar;
		token_size++;

		// Escape sequence
		if (c == '\\')
		{
			if (!Has(token_size + 1))
			{
				Commit(token_size);
				throw LexerException("Unterminated Character Literal", _CurLocation);
			}

			char esc = _LastChar;
			token_size++;

			switch (esc)
			{
			case 'n': case 't': case 'r':
			case '\\': case '\'': case '0':
				break;
			default:
				Commit(token_size);
				throw LexerException("Invalid Escape Sequence in Character Literal", _CurLocation);
			}
		}

		if (!Has(token_size + 1) || _LastChar != '\'')
		{
			Commit(token_size);
			throw LexerException("Character Literal Must Contain Exactly One Character", _CurLocation);
		}

		token_size++;

		Token token{ TokenLabel::LIT_CHAR, std::string(_CurToken), _CurLocation };

		Commit(token_size);
		return token;
	}


	Lexer::Lexer(lexing::filereader&& input)
		: inp(std::move(input)), token_size(0), view_capacity(0)
	{
	}

	Lexer::Lexer(Lexer&& lexer) noexcept :
		inp(std::move(lexer.inp)), token_size(lexer.token_size), view_capacity(lexer.view_capacity)
	{
	}

	Lexer& Lexer::operator=(Lexer&& lexer) noexcept
	{
		this->inp = std::move(lexer.inp);
		this->token_size = lexer.token_size;
		this->view_capacity = lexer.view_capacity;

		return *this;
	}

	Token Lexer::ReadAndClassifyNext()
	{
		// Skip whitespace
		while (Has(1) && IsWhitespace(_LastChar)) { Commit(1); }

		if (not Has(1)) { return Token{ TokenLabel::_EOF_, "", _CurLocation }; }

		if (auto t = AnalyzeComments()) return *t;
		if (auto t = AnalyzeKeywords()) return *t;
		if (auto t = AnalyzeNumbers())  return *t;
		if (auto t = AnalyzeChars())    return *t;
		if (auto t = AnalyzeLiterals()) return *t;
		if (auto t = AnalyzeSymbols())  return *t;

		// Unknown character
		throw LexerException("Unknown Character", _CurLocation);
	}

	std::string Lexer::GetFileName() const
	{
		return inp.filename;
	}

	size_t Lexer::GetLine() const
	{
		return inp.line + 1;
	}

	size_t Lexer::GetColumn() const
	{
		return inp.column;
	}

}