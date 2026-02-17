#pragma once

#include <string>
#include <iostream>
#include "selector.h"

namespace val 
{

	class SemanticException : public std::logic_error
	{
	public:
		SemanticException(const std::string& msg, const std::string& filename, selector sel, size_t line);

		SemanticException() = default;
		SemanticException(const SemanticException& other) = default;
		SemanticException& operator =(const SemanticException& other) = default;
		SemanticException(SemanticException&& other) noexcept = default;
		SemanticException& operator =(SemanticException&& other) noexcept = default;

		~SemanticException() = default;
	};

}