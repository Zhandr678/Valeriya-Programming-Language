#include <iostream>
#include <vector>
#include <string>
#include <variant>
#include <fstream>
#include <algorithm>
#include <format>
#include <memory>

#include "argsconfig.h"
#include "Lexing/Lexer.h"
#include "Parsing/Parser.h"
#include "SemanticAnalysis/Semantics.h"
#include "Commands/CGenerator.h"

#ifndef RUN_TESTS

int main(int argc, char* argv[])
{

#pragma region("Argument Parsing")

	cxxopts::Options args(val::PROGRAM_NAME, val::PROGRAM_DESC);
	val::AddOptions(args, val::OPTIONS);

	cxxopts::ParseResult result;
	if (not val::AssertCorrectOptions(result, argc, argv, args))
	{
		return 1;
	}

#pragma endregion

	std::filesystem::path val_source_path = result["file"].as<std::string>();
	std::ifstream in(val_source_path);

	try {

		val::Parser parser(
			val::Lexer(
				lexing::filereader(&in, val_source_path.filename().string())
			)
		);

		auto AST = parser.ConstructAST();
		val::Semantics(val_source_path.filename().string()).Analysis(AST);
		val::CGenerator::GenerateC_IR(AST, val_source_path.string());
	}
	catch (const std::logic_error& e)
	{
		std::cout << e.what();
	}
	return 0;
}

#endif