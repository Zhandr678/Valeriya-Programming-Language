#include "SemanticException.h"

namespace val 
{

	SemanticException::SemanticException(const std::string& msg, const std::string& filename, selector sel, size_t line)
		: std::logic_error("SemanticException: [" + msg + "], \
			\n\tin [" + getcstring(sel) + "]" +
			"\n\tin File : [" + filename + "] " +
			"\n\tline " + std::to_string(line))
	{

	}

}