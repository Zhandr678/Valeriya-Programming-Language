#pragma once

#include "lexing2023/includes.h"

#include "token.h"

class Lexer
{
private:
	lexing::filereader inp;
	lexing::classifier <char, TokenLabel> cls;

	void BuildClassifier();
public:
	Lexer() = delete;

	Lexer(lexing::filereader&& inp);

	Token NextToken();
};