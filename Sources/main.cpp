#include <iostream>
#include <vector>
#include <string>
#include <variant>
#include <fstream>
#include <algorithm>

#include "Lexing/lexer.h"

#include "argsconfig.h"

int main(int argc, char* argv[])
{

#pragma region("Argument Parsing")

	cxxopts::Options args(args_config::PROGRAM_NAME, args_config::PROGRAM_DESC);
	args_config::AddOptions(args, args_config::OPTIONS);

	cxxopts::ParseResult result;
	if (not args_config::AssertCorrectOptions(result, argc, argv, args)) 
	{
		return 1;
	}

#pragma endregion



#pragma region("Lexical Analysis")

	std::filesystem::path val_source_path = result["file"].as<std::string>();
	std::ifstream in(val_source_path);
	
	lexing::filereader freader(&in, val_source_path.filename().string());

	Lexer lexer(std::move(freader));
	std::cout << lexer.NextToken().attr;

#pragma endregion

	std::cout << "Compilation Finished\n";
	return 0;
}

// std::ifstream in("C:/Users/sagin/Desktop/exprog.val");
// lexing::filereader f(&in, "C:/Users/sagin/Desktop/exprog.val");