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

		val::Semantics semantics(val_source_path.filename().string());
		semantics.Analysis(AST);

		

		val::CGenerator codegen(semantics.GetCompileInfo());
		codegen.GenerateC_IR(AST, val_source_path.string());

		std::filesystem::path c_file = val_source_path;
		c_file.replace_extension(".c");
		std::filesystem::path output_binary = val_source_path;

#ifdef _WIN32
		output_binary.replace_extension(".exe");
		std::string compile_cmd = std::format(
			"clang \"{}\" -o \"{}\"",
			c_file.string(),
			output_binary.string()
		);
#else
		output_binary.replace_extension(""); // no extension
		std::string compile_cmd = std::format(
			"clang \"{}\" -o \"{}\"",
			c_file.string(),
			output_binary.string()
		);
#endif

		std::cout << "Compiling with: " << compile_cmd << "\n";

		int compile_result = std::system(compile_cmd.c_str());

		if (compile_result != 0)
		{
			std::cerr << "Compilation failed.\n";
			return 1;
		}

		std::cout << "Binary generated: " << output_binary << "\n";
	}
	catch (const std::logic_error& e)
	{
		std::cout << e.what();
	}
	return 0;
}

#endif