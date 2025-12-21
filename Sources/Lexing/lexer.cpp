#include "lexer.h"

void Lexer::BuildClassifier()
{
    using namespace lexing;

#pragma region("Keywords")

    cls.insert(word("int"), TokenLabel::KW_INT);
    cls.insert(word("double"), TokenLabel::KW_DOUBLE);
    cls.insert(word("string"), TokenLabel::KW_STRING);
    cls.insert(word("bool"), TokenLabel::KW_BOOL);
    cls.insert(word("char"), TokenLabel::KW_CHAR);
    cls.insert(word("void"), TokenLabel::KW_VOID);
    cls.insert(word("if"), TokenLabel::KW_IF);
    cls.insert(word("elif"), TokenLabel::KW_ELIF);
    cls.insert(word("else"), TokenLabel::KW_ELSE);
    cls.insert(word("for"), TokenLabel::KW_FOR);
    cls.insert(word("while"), TokenLabel::KW_WHILE);
    cls.insert(word("struct"), TokenLabel::KW_STRUCT);
    cls.insert(word("property"), TokenLabel::KW_PROP);
    cls.insert(word("match"), TokenLabel::KW_MATCH);
    cls.insert(word("case"), TokenLabel::KW_CASE);
    cls.insert(word("return"), TokenLabel::KW_RET);
    cls.insert(word("fn"), TokenLabel::KW_FUNC);
    cls.insert(word("inout"), TokenLabel::KW_INOUT);

#pragma endregion

#pragma region("Identifiers")

    auto idfirst = range('a', 'z') | range('A', 'Z') | just('_');
    auto idnext = idfirst | range('0', '9');
    cls.insert(idfirst * idnext.star(), TokenLabel::IDENTIFIER);

#pragma endregion

#pragma region("Literals")

    auto number = (just('0') | (range('1', '9') * range('0', '9').star()));
    auto exp = (just('e') | just('E')) * (just('+') | just('-')).optional() * range('0', '9').plus();
    auto floatconst = number * (just('.') * range('0', '9').plus()).optional() * exp.optional();
    cls.insert(floatconst, TokenLabel::LIT_NUMBER);

    auto stringlit = just('"') * (every<char>().without('"')).star() * just('"');
    cls.insert(stringlit, TokenLabel::LIT_STRING);

    cls.insert(word("true"), TokenLabel::LIT_BOOL);
    cls.insert(word("false"), TokenLabel::LIT_BOOL);

    cls.insert(word("null"), TokenLabel::LIT_NULL);

#pragma endregion

#pragma region("Symbols and Operators")

    cls.insert(just('{'), TokenLabel::SYM_LBRACE);
    cls.insert(just('}'), TokenLabel::SYM_RBRACE);
    cls.insert(just('('), TokenLabel::SYM_LPAR);
    cls.insert(just(')'), TokenLabel::SYM_RPAR);
    cls.insert(just('['), TokenLabel::SYM_LBRACKET);
    cls.insert(just(']'), TokenLabel::SYM_RBRACKET);
    cls.insert(just(';'), TokenLabel::SYM_SEMICOLON);
    cls.insert(just(':'), TokenLabel::SYM_COLON);
    cls.insert(just(','), TokenLabel::SYM_COMMA);
    cls.insert(just('.'), TokenLabel::SYM_DOT);
    cls.insert(word("->"), TokenLabel::SYM_ARROW);


    cls.insert(just('+'), TokenLabel::OP_PLUS);
    cls.insert(just('-'), TokenLabel::OP_MINUS);
    cls.insert(just('*'), TokenLabel::OP_TIMES);
    cls.insert(just('/'), TokenLabel::OP_DIV);
    cls.insert(just('%'), TokenLabel::OP_MOD);
    cls.insert(word("**"), TokenLabel::OP_POW);

    cls.insert(just('&'), TokenLabel::OP_AND);
    cls.insert(just('|'), TokenLabel::OP_OR);
    cls.insert(just('^'), TokenLabel::OP_XOR);
    cls.insert(just('!'), TokenLabel::OP_NOT);

    cls.insert(just('='), TokenLabel::OP_ASSIGN);
    cls.insert(word("=="), TokenLabel::OP_EQUAL);
    cls.insert(word("!="), TokenLabel::OP_NEQ);
    cls.insert(just('<'), TokenLabel::OP_LESS);
    cls.insert(just('>'), TokenLabel::OP_GREATER);
    cls.insert(word("<="), TokenLabel::OP_LEQ);
    cls.insert(word(">="), TokenLabel::OP_GEQ);

    cls.insert(word("||"), TokenLabel::OP_LOR);
    cls.insert(word("&&"), TokenLabel::OP_LAND);

#pragma endregion

#pragma region("Comments")

    auto whitespace = just(' ') | just('\f') | just('\n') | just('\r') | just('\t') | just('\v');
    cls.insert(whitespace, TokenLabel::WHITESPACE);

    auto linecomment = word("//") * (every<char>().without('\n')).star() * just('\n');
    cls.insert(linecomment, TokenLabel::COM_LINE);

    auto blockcomment = word("/*") *
        (every<char>().without('*') |
            (just('*').plus() * every<char>().without('/').without('*'))
            ).star() * just('*').plus() * just('/');
    cls.insert(blockcomment, TokenLabel::COM_BEGIN);

#pragma endregion

    cls = make_deterministic(cls);
    cls = minimize(cls);
}

Lexer::Lexer(lexing::filereader&& inp) : inp(std::move(inp)), cls(TokenLabel::_ERROR_)
{
	BuildClassifier();
}

Token Lexer::NextToken() 
{
    using namespace lexing;

restart:
    int startLine = (int)inp.line;
    int startCol = (int)inp.column;

    if (!inp.has(1))
    {
        return Token{ TokenLabel::_EOF_, "", inp.filename, startLine, startCol };
    }

    if (!inp.good())
    {
        return Token{ TokenLabel::_ERROR_, "File Bad", inp.filename, startLine, startCol };
    }

    auto p = readandclassify(cls, inp);

    // Handle Scan Errors (p.second == 0 means no match)
    if (p.second == 0) {
        std::string garbage(inp.view(1));
        inp.commit(1);
        return Token{ TokenLabel::_ERROR_, garbage, inp.filename, startLine, startCol };
    }

    TokenLabel label = p.first;

    // Skip Whitespace and Comments
    if (label == TokenLabel::WHITESPACE ||
        label == TokenLabel::COM_LINE ||
        label == TokenLabel::COM_BEGIN)
    {
        inp.commit(p.second);
        goto restart;
    }

    // Process Attributes (Handling Quotes for Strings/Chars)
    std::string_view view = inp.view(p.second);
    std::string lexeme;

    if (label == TokenLabel::LIT_STRING || label == TokenLabel::KW_CHAR) // Assuming LIT_CHAR/KW_CHAR uses quotes
    {
        // Remove surrounding quotes if they exist
        if (view.size() >= 2)
            lexeme = std::string(view.substr(1, view.size() - 2));
        else
            lexeme = std::string(view);
    }
    else
    {
        lexeme = std::string(view);
    }

    inp.commit(p.second);
    return Token{ label, lexeme, inp.filename, startLine, startCol };
}