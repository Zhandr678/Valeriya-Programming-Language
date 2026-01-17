#pragma once

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
		std::optional <Token> AnalyzeSymbols();
		std::optional <Token> AnalyzeComments();
		std::optional <Token> AnalyzeChars();

	public:
		explicit Lexer(lexing::filereader&& input);

		Lexer() = delete;
		Lexer(const Lexer&) = delete;
		Lexer& operator=(const Lexer& lexer) = delete;

		Lexer(Lexer&& lexer) noexcept;
		Lexer& operator=(Lexer&& lexer) noexcept;

		Token ReadAndClassifyNext();

		std::string GetFileName() const;
		size_t GetLine() const;
		size_t GetColumn() const;

		~Lexer() = default;
	};

}