#pragma once

#include "Statements/Statement.h"

namespace val
{

	class Compile
	{
	public:
		static void Convert_to_C(const Statement& AST, std::string_view out_c_file);

		static void Compile(std::string_view out_c_file);
	};

}