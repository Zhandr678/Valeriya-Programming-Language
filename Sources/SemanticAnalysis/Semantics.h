#pragma once

#include "Statements/Statement.h"

#include <unordered_map>

namespace val
{

	class Semantics
	{
	private:
		inline static std::unordered_map <std::string, bool> variable_table, type_table;

	public:
		static bool Verify(const Statement& AST);
	};

}