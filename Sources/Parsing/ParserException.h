#pragma once

#include <string>
#include <exception>

#include "Statements/selector.h"

namespace val 
{

	class ParserException : public std::logic_error
	{
	public:
		explicit ParserException(const std::string& msg, const std::string& filename, selector sel, size_t line);

		ParserException() = default;
		ParserException(const ParserException& other) = default;
		ParserException& operator =(const ParserException& other) = default;
		ParserException(ParserException&& other) noexcept = default;
		ParserException& operator =(ParserException&& other) noexcept = default;

		~ParserException() = default;
	};

}