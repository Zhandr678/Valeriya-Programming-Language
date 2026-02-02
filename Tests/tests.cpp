#ifdef RUN_TESTS

#include "lexer_test.h"
#include "parser_test.h"

int main()
{
	// LexerTest(std::string(PROJECT_ROOT_DIR) + std::string("/ExampleCode/example.val"));

	ParserTest(std::string(PROJECT_ROOT_DIR) + std::string("/ExampleCode/example.val"));
}

#endif