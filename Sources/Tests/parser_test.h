#ifdef RUN_TESTS

#include <filesystem>
#include <string>
#include <fstream>

#include "Parsing/Parser.h"

void ParserTest(const std::filesystem::path& val_file)
{
	std::ifstream in(val_file);

	val::Lexer lexer(lexing::filereader(&in, val_file.filename().string()));

	val::Parser parser(std::move(lexer));

	try {

		auto AST = parser.ConstructAST();

		size_t N = AST.view_Block().size();
		std::cout << "List of Statements of file: " << val_file << "\n";
		for (size_t i = 0; i < N; i++)
		{
			std::cout << AST.view_Block().statements(i).sel();
			std::cout << "\n";
		}

	}
	catch (const std::logic_error& e)
	{
		std::cerr << e.what();
	}
}

#endif