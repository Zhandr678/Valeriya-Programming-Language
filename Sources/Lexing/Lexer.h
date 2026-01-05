#pragma once

#include <vector>
#include <string>
#include <optional>

#include "lexing2023/filereader.h"
#include "Token.h"

namespace val
{

	class Lexer
	{
	private:
		lexing::filereader inp;
		size_t token_size, view_capacity;

		bool Has(size_t next);
		void Commit(size_t next);

		std::optional <Token> AnalyzeNumbers();
		std::optional <Token> AnalyzeLiterals();
		std::optional <Token> AnalyzeKeywords();
		std::optional <Token> AnalyzeSymbolsAndOperators();
		std::optional <Token> AnalyzeComments();

	public:
		explicit Lexer(lexing::filereader&& input);

		Token ReadAndClassifyNext();
	};

}