#ifdef RUN_TESTS

#include <filesystem>
#include <string>
#include <fstream>

#include "Lexing/Lexer.h"
#include "argsconfig.h"
#include "Lexing/LexerException.h"

void LexerTest(const std::filesystem::path& val_file)
{
	std::ifstream in(val_file);

	val::Lexer lexer(lexing::filereader(&in, val_file.filename().string()));

	try {
		auto t = lexer.ReadAndClassifyNext();
		while (t.label != val::TokenLabel::_EOF_)
		{
			std::cout << std::format("[\n\tLabel: {}, \n\tAttr: {}, \n\tline: {}, at: {} \n]\n", t.label, t.attr, t.loc.line, t.loc.at);
			t = lexer.ReadAndClassifyNext();
		}
	}
	catch (const val::LexerException& e)
	{
		std::cerr << e.what();
	}
}

int main()
{
	LexerTest(std::string(PROJECT_ROOT_DIR) + std::string("/ExampleCode/no_expr.val"));
}

#endif