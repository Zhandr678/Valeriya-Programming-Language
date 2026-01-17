#pragma once

#include <string>
#include <format>
#include <ostream>
#include <sstream>

namespace val
{

    struct Location
    {
        std::string filename;
        uint32_t line, at;
    };

    enum class TokenLabel
    {
        _ERROR_, WHITESPACE,

        KW_INT, KW_DOUBLE, KW_STRING, KW_BOOL, KW_CHAR, KW_VOID,
        KW_IF, KW_ELIF, KW_ELSE, KW_FOR, KW_WHILE, KW_ENUM, 
        KW_STRUCT, KW_PROP, KW_MATCH, KW_CASE, KW_RET, KW_FUNC, KW_INOUT,

        IDENTIFIER,

        LIT_NUMBER, LIT_STRING, LIT_TRUE, LIT_FALSE, LIT_NULL, LIT_INTEGER, LIT_CHAR,

        SYM_LBRACE, SYM_RBRACE, SYM_LBRACKET, SYM_RBRACKET, SYM_LPAR, SYM_RPAR,
        SYM_COLON, SYM_SEMICOLON, SYM_COMMA, SYM_DOT, SYM_ARROW,

        SYM_PLUS, SYM_MINUS, SYM_TIMES, SYM_DIV, SYM_MOD, SYM_POW,
        SYM_AND, SYM_OR, SYM_LOR, SYM_LAND, SYM_XOR, SYM_NOT,

        SYM_ASSIGN, SYM_EQUAL, SYM_NEQ, SYM_LESS, SYM_GREATER, SYM_LEQ, SYM_GEQ,

        COM_LINE, COM_BLOCK,

        _EOF_
    };

    struct Token
    {
        TokenLabel label;
        std::string attr;
        Location loc;
    };
}

namespace std {

    /* Overloaded TokenLabel for std::ostream& */
    inline std::ostream& operator<<(std::ostream& out, val::TokenLabel label) {
        switch (label) {
        case val::TokenLabel::KW_INT:        return out << "KW_INT";
        case val::TokenLabel::KW_DOUBLE:     return out << "KW_DOUBLE";
        case val::TokenLabel::KW_STRING:     return out << "KW_STRING";
        case val::TokenLabel::KW_BOOL:       return out << "KW_BOOL";
        case val::TokenLabel::KW_CHAR:       return out << "KW_CHAR";
        case val::TokenLabel::KW_VOID:       return out << "KW_VOID";
        case val::TokenLabel::KW_IF:         return out << "KW_IF";
        case val::TokenLabel::KW_ELIF:       return out << "KW_ELIF";
        case val::TokenLabel::KW_ELSE:       return out << "KW_ELSE";
        case val::TokenLabel::KW_FOR:        return out << "KW_FOR";
        case val::TokenLabel::KW_WHILE:      return out << "KW_WHILE";
        case val::TokenLabel::KW_STRUCT:     return out << "KW_STRUCT";
        case val::TokenLabel::KW_PROP:       return out << "KW_PROP";
        case val::TokenLabel::KW_MATCH:      return out << "KW_MATCH";
        case val::TokenLabel::KW_CASE:       return out << "KW_CASE";
        case val::TokenLabel::KW_RET:        return out << "KW_RET";
        case val::TokenLabel::KW_FUNC:       return out << "KW_FUNC";
        case val::TokenLabel::KW_INOUT:      return out << "KW_INOUT";
        case val::TokenLabel::KW_ENUM:       return out << "KW_ENUM";
        case val::TokenLabel::IDENTIFIER:    return out << "IDENTIFIER";
        case val::TokenLabel::LIT_NUMBER:    return out << "LIT_NUMBER";
        case val::TokenLabel::LIT_INTEGER:   return out << "LIT_INTEGER";
        case val::TokenLabel::LIT_STRING:    return out << "LIT_STRING";
        case val::TokenLabel::LIT_TRUE:      return out << "LIT_TRUE";
        case val::TokenLabel::LIT_FALSE:     return out << "LIT_FALSE";
        case val::TokenLabel::LIT_NULL:      return out << "LIT_NULL";
        case val::TokenLabel::LIT_CHAR:      return out << "LIT_CHAR";
        case val::TokenLabel::SYM_LBRACE:    return out << "SYM_LBRACE";
        case val::TokenLabel::SYM_RBRACE:    return out << "SYM_RBRACE";
        case val::TokenLabel::SYM_LPAR:      return out << "SYM_LPAR";
        case val::TokenLabel::SYM_RPAR:      return out << "SYM_RPAR";
        case val::TokenLabel::SYM_LBRACKET:  return out << "SYM_LBRACKET";
        case val::TokenLabel::SYM_RBRACKET:  return out << "SYM_RBRACKET";
        case val::TokenLabel::SYM_SEMICOLON: return out << "SYM_SEMICOLON";
        case val::TokenLabel::SYM_COLON:     return out << "SYM_COLON";
        case val::TokenLabel::SYM_COMMA:     return out << "SYM_COMMA";
        case val::TokenLabel::SYM_DOT:       return out << "SYM_DOT";
        case val::TokenLabel::SYM_ARROW:     return out << "SYM_ARROW";
        case val::TokenLabel::SYM_PLUS:       return out << "SYM_PLUS";
        case val::TokenLabel::SYM_MINUS:      return out << "SYM_MINUS";
        case val::TokenLabel::SYM_TIMES:      return out << "SYM_TIMES";
        case val::TokenLabel::SYM_DIV:        return out << "SYM_DIV";
        case val::TokenLabel::SYM_MOD:        return out << "SYM_MOD";
        case val::TokenLabel::SYM_POW:        return out << "SYM_POW";
        case val::TokenLabel::SYM_AND:        return out << "SYM_AND";
        case val::TokenLabel::SYM_OR:         return out << "SYM_OR";
        case val::TokenLabel::SYM_XOR:        return out << "SYM_XOR";
        case val::TokenLabel::SYM_NOT:        return out << "SYM_NOT";
        case val::TokenLabel::SYM_ASSIGN:     return out << "SYM_ASSIGN";
        case val::TokenLabel::SYM_EQUAL:      return out << "SYM_EQUAL";
        case val::TokenLabel::SYM_NEQ:        return out << "SYM_NEQ";
        case val::TokenLabel::SYM_LESS:       return out << "SYM_LESS";
        case val::TokenLabel::SYM_GREATER:    return out << "SYM_GREATER";
        case val::TokenLabel::SYM_LEQ:        return out << "SYM_LEQ";
        case val::TokenLabel::SYM_GEQ:        return out << "SYM_GEQ";
        case val::TokenLabel::SYM_LOR:        return out << "SYM_LOR";
        case val::TokenLabel::SYM_LAND:       return out << "SYM_LAND";
        case val::TokenLabel::COM_LINE:      return out << "COM_LINE";
        case val::TokenLabel::COM_BLOCK:     return out << "COM_BLOCK";
        case val::TokenLabel::_EOF_:         return out << "_EOF_";
        case val::TokenLabel::_ERROR_:       return out << "_ERROR_";
        default:                        return out << "(unknown)";
        }
    }

    /* Overloaded TokenLabel for std::format */
    template<>
    struct std::formatter<val::TokenLabel>
    {
        constexpr auto parse(std::format_parse_context& ctx)
        {
            return ctx.begin();
        }

        auto format(val::TokenLabel label, std::format_context& ctx) const
        {
            std::ostringstream oss;
            oss << label;
            return std::format_to(ctx.out(), "{}", oss.str());
        }
    };

}