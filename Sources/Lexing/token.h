#pragma once

#include <string>
#include <format>
#include <ostream>
#include <sstream>

struct Location
{
    std::string filename;
    uint32_t line, at;
};

enum class TokenLabel
{
    _ERROR_, WHITESPACE,

	KW_INT, KW_DOUBLE, KW_STRING, KW_BOOL, KW_CHAR, KW_VOID, 
	KW_IF, KW_ELIF, KW_ELSE, KW_FOR, KW_WHILE, KW_OPTION,
	KW_STRUCT, KW_PROP, KW_MATCH, KW_CASE, KW_RET, KW_FUNC, KW_INOUT,

	IDENTIFIER,

	LIT_NUMBER, LIT_STRING, LIT_TRUE, LIT_FALSE, LIT_NULL, LIT_INTEGER,

	SYM_LBRACE, SYM_RBRACE, SYM_LBRACKET, SYM_RBRACKET, SYM_LPAR, SYM_RPAR,
	SYM_COLON, SYM_SEMICOLON, SYM_COMMA, SYM_DOT, SYM_ARROW,

	OP_PLUS, OP_MINUS, OP_TIMES, OP_DIV, OP_MOD, OP_POW,
	OP_AND, OP_OR, OP_LOR, OP_LAND, OP_XOR, OP_NOT,

	OP_ASSIGN, OP_EQUAL, OP_NEQ, OP_LESS, OP_GREATER, OP_LEQ, OP_GEQ,

	COM_LINE, COM_BLOCK,

	_EOF_
};

struct Token
{
	TokenLabel label;
	std::string attr;
    Location loc;
};

/* Overloaded TokenLabel for std::ostream& */
inline std::ostream& operator<<(std::ostream& out, TokenLabel label) {
    switch (label) {
        case TokenLabel::KW_INT:        return out << "KW_INT";
        case TokenLabel::KW_DOUBLE:     return out << "KW_DOUBLE";
        case TokenLabel::KW_STRING:     return out << "KW_STRING";
        case TokenLabel::KW_BOOL:       return out << "KW_BOOL";
        case TokenLabel::KW_CHAR:       return out << "KW_CHAR";
        case TokenLabel::KW_VOID:       return out << "KW_VOID";
        case TokenLabel::KW_IF:         return out << "KW_IF";
        case TokenLabel::KW_ELIF:       return out << "KW_ELIF";
        case TokenLabel::KW_ELSE:       return out << "KW_ELSE";
        case TokenLabel::KW_FOR:        return out << "KW_FOR";
        case TokenLabel::KW_WHILE:      return out << "KW_WHILE";
        case TokenLabel::KW_STRUCT:     return out << "KW_STRUCT";
        case TokenLabel::KW_PROP:       return out << "KW_PROP";
        case TokenLabel::KW_OPTION:     return out << "KW_OPTION";
        case TokenLabel::KW_MATCH:      return out << "KW_MATCH";
        case TokenLabel::KW_CASE:       return out << "KW_CASE";
        case TokenLabel::KW_RET:        return out << "KW_RET";
        case TokenLabel::KW_FUNC:       return out << "KW_FUNC";
        case TokenLabel::KW_INOUT:      return out << "KW_INOUT";
        case TokenLabel::IDENTIFIER:    return out << "IDENTIFIER";
        case TokenLabel::LIT_NUMBER:    return out << "LIT_NUMBER";
        case TokenLabel::LIT_INTEGER:   return out << "LIT_INTEGER";
        case TokenLabel::LIT_STRING:    return out << "LIT_STRING";
        case TokenLabel::LIT_TRUE:      return out << "LIT_TRUE";
        case TokenLabel::LIT_FALSE:     return out << "LIT_FALSE";
        case TokenLabel::LIT_NULL:      return out << "LIT_NULL";
        case TokenLabel::SYM_LBRACE:    return out << "SYM_LBRACE";
        case TokenLabel::SYM_RBRACE:    return out << "SYM_RBRACE";
        case TokenLabel::SYM_LPAR:      return out << "SYM_LPAR";
        case TokenLabel::SYM_RPAR:      return out << "SYM_RPAR";
        case TokenLabel::SYM_LBRACKET:  return out << "SYM_LBRACKET";
        case TokenLabel::SYM_RBRACKET:  return out << "SYM_RBRACKET";
        case TokenLabel::SYM_SEMICOLON: return out << "SYM_SEMICOLON";
        case TokenLabel::SYM_COLON:     return out << "SYM_COLON";
        case TokenLabel::SYM_COMMA:     return out << "SYM_COMMA";
        case TokenLabel::SYM_DOT:       return out << "SYM_DOT";
        case TokenLabel::SYM_ARROW:     return out << "SYM_ARROW";
        case TokenLabel::OP_PLUS:       return out << "OP_PLUS";
        case TokenLabel::OP_MINUS:      return out << "OP_MINUS";
        case TokenLabel::OP_TIMES:      return out << "OP_TIMES";
        case TokenLabel::OP_DIV:        return out << "OP_DIV";
        case TokenLabel::OP_MOD:        return out << "OP_MOD";
        case TokenLabel::OP_POW:        return out << "OP_POW";
        case TokenLabel::OP_AND:        return out << "OP_AND";
        case TokenLabel::OP_OR:         return out << "OP_OR";
        case TokenLabel::OP_XOR:        return out << "OP_XOR";
        case TokenLabel::OP_NOT:        return out << "OP_NOT";
        case TokenLabel::OP_ASSIGN:     return out << "OP_ASSIGN";
        case TokenLabel::OP_EQUAL:      return out << "OP_EQUAL";
        case TokenLabel::OP_NEQ:        return out << "OP_NEQ";
        case TokenLabel::OP_LESS:       return out << "OP_LESS";
        case TokenLabel::OP_GREATER:    return out << "OP_GREATER";
        case TokenLabel::OP_LEQ:        return out << "OP_LEQ";
        case TokenLabel::OP_GEQ:        return out << "OP_GEQ";
        case TokenLabel::OP_LOR:        return out << "OP_LOR";
        case TokenLabel::OP_LAND:       return out << "OP_LAND";
        case TokenLabel::COM_LINE:      return out << "COM_LINE";
        case TokenLabel::COM_BLOCK:     return out << "COM_BLOCK";
        case TokenLabel::_EOF_:         return out << "_EOF_";
        case TokenLabel::_ERROR_:       return out << "_ERROR_";
        default:                        return out << "(unknown)";
    }
}

/* Overloaded TokenLabel for std::format */
template<>
struct std::formatter<TokenLabel>
{
    constexpr auto parse(std::format_parse_context& ctx)
    {
        return ctx.begin();
    }

    auto format(TokenLabel label, std::format_context& ctx) const
    {
        std::ostringstream oss;
        oss << label;
        return std::format_to(ctx.out(), "{}", oss.str());
    }
};
