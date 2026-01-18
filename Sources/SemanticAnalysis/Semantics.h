#pragma once

#include "Statements/Statement.h"

namespace val
{

	class Semantics
	{
	public:
		static void Verify(Statement&& AST);
	};

}