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
        case val::TokenLabel::OP_PLUS:       return out << "OP_PLUS";
        case val::TokenLabel::OP_MINUS:      return out << "OP_MINUS";
        case val::TokenLabel::OP_TIMES:      return out << "OP_TIMES";
        case val::TokenLabel::OP_DIV:        return out << "OP_DIV";
        case val::TokenLabel::OP_MOD:        return out << "OP_MOD";
        case val::TokenLabel::OP_POW:        return out << "OP_POW";
        case val::TokenLabel::OP_AND:        return out << "OP_AND";
        case val::TokenLabel::OP_OR:         return out << "OP_OR";
        case val::TokenLabel::OP_XOR:        return out << "OP_XOR";
        case val::TokenLabel::OP_NOT:        return out << "OP_NOT";
        case val::TokenLabel::OP_ASSIGN:     return out << "OP_ASSIGN";
        case val::TokenLabel::OP_EQUAL:      return out << "OP_EQUAL";
        case val::TokenLabel::OP_NEQ:        return out << "OP_NEQ";
        case val::TokenLabel::OP_LESS:       return out << "OP_LESS";
        case val::TokenLabel::OP_GREATER:    return out << "OP_GREATER";
        case val::TokenLabel::OP_LEQ:        return out << "OP_LEQ";
        case val::TokenLabel::OP_GEQ:        return out << "OP_GEQ";
        case val::TokenLabel::OP_LOR:        return out << "OP_LOR";
        case val::TokenLabel::OP_LAND:       return out << "OP_LAND";
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