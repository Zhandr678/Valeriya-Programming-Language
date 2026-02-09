#pragma once

#include "Command.h"

namespace val
{

	class DeclareVarCommand : public ICommand <DeclareVarCommand>
	{
	private:
		std::string var_name;
		std::string type_name;

		std::optional <std::string> refers_to;
		std::optional <std::string> valid_init_expr;

	public:
		DeclareVarCommand(const std::string& var_name, const std::string& type_name);
		DeclareVarCommand(const std::string& var_name, const std::string& type_name, const std::string& refers_to);
		DeclareVarCommand(const std::string& var_name, const std::string& type_name, const std::string& refers_to, const std::string& valid__init_expr);

		std::string Convert_To_C() const noexcept;
	};

}