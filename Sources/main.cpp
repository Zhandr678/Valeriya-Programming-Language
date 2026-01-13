#include <iostream>
#include <vector>
#include <string>
#include <variant>
#include <fstream>
#include <algorithm>
#include <format>

#include "Lexing/Lexer.h"
#include "Parsing/Parser.h"

#include "argsconfig.h"
#include "tvm/includes.h"
#include "Statements/Expression.h"

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

	val::Parser parser(
		val::Lexer(
			lexing::filereader(&in, val_source_path.filename().string())
		)
	);

	return 0;
}

#endif