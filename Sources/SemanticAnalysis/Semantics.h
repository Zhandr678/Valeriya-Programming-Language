#pragma once

#include "Statements/Statement.h"

namespace val
{

	class Semantics
	{
	private:
		Statement AST;

	public:
		void Verify();
	};

}