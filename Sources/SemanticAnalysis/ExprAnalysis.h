#pragma once

#include <string>
#include "Statements/Expression.h"

namespace val
{

	struct ExpressionAnalysisResult
	{
		std::string final_type_name;
		Expression valid_simplified_expr;
	};

}