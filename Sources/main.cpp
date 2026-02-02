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
#include "Compilation/Compiler.h"

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
		std::cout << AST.view_Block().statements(0).view_MakeStruct().inits(4).view_ArrayInit().type_info().view_VarInit().var_name();
	}
	catch (const std::logic_error& e)
	{
		std::cout << e.what();
	}
	//std::cout << AST.view_Block().statements(8).view_MakeFunction().params(0).view_FnArgs().is_inout();
}

#endif