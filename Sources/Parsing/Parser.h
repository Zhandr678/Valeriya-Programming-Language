#pragma once

#include "Lexing/Lexer.h"
#include "Statements/Statement.h"
#include <variant>
#include <vector>
#include <queue>

namespace val 
{
	class Parser
	{
	private:
		Lexer lexer;

		std::vector <Token> peeked;
		size_t next = 0;

		Token GetNextImportantToken();
		Token GetNextPeeked();
		void ResetLookAhead();

		std::optional <Statement> AnalyzeBlockOfStatement();
		std::optional <Statement> AnalyzeInitalizationStatement();
		std::optional <Statement> AnalyzeAssignmentStatement();
		std::optional <Statement> AnalyzeConditionStatement();
		std::optional <Statement> AnalyzeForLoopStatement();
		std::optional <Statement> AnalyzeWhileLoopStatement();
		std::optional <Statement> AnalyzeMakeFunctionStatement();
		std::optional <Statement> AnalyzeMakeStructStatement();
		std::optional <Statement> AnalyzeMakePropertyStatement();
		std::optional <Statement> AnalyzeMakeEnumStatement();
		std::optional <Statement> AnalyzeMatchStatement();
		std::optional <Statement> AnalyzeReturnStatement();
		std::optional <Statement> AnalyzeLoopCommands();
		std::optional <Statement> ParseForLoopFinalStatement();
		
		std::optional <Expression> ParseExpression(const std::vector <TokenLabel>& flags);

	public:
		explicit Parser(Lexer&& lexer);
		
		Parser() = delete;
		Parser(const Parser&) = delete;
		Parser& operator=(const Parser&) = delete;

		Parser(Parser&& parser) noexcept;
		Parser& operator=(Parser&& parser) noexcept;

		Statement ConstructAST(TokenLabel flag = TokenLabel::_EOF_);

		std::string GetFileName() const;
		size_t GetLine() const;
		size_t GetColumn() const;

		~Parser() = default;
	};

}