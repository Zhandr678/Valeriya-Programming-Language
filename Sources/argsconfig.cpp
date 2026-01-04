#include "argsconfig.h"
#include <filesystem>
#include <iostream>

namespace args_config
{
	const std::string PROGRAM_NAME = "Valeria";
	const std::string PROGRAM_DESC = "Compiler for Valeria Programming Language";

	const std::vector<CompilerOption> OPTIONS = {
		{ "file", "Input file", cxxopts::value<std::string>(), true},

		// Add More
	};

	static bool IsValeriaSource(std::string_view path)
	{
		if (path.length() < 4) { return false; }
		return path[path.length() - 4] == '.' and path[path.length() - 3] == 'v' and path[path.length() - 2] == 'a' and path[path.length() - 1] == 'l';
	}

	void AddOptions(cxxopts::Options& args, const std::vector <CompilerOption>& options)
	{
		for (const auto& [opt, desc, val, req] : options)
		{
			args.add_options()(opt, desc, val);
		}
	}

	bool AssertCorrectOptions(cxxopts::ParseResult& result, int argc, char** argv, const cxxopts::Options& args)
	{
		try {
			result = args.parse(argc, argv);
			for (const auto& [opt, desc, val, req] : OPTIONS)
			{
				if (req and result.count(opt) != 1)
				{
					throw cxxopts::exceptions::exception(
						std::format("Option '--{}' requires 1 argument, but got {}", opt, result.count(opt)));
				}

				if (opt == "file")
				{
					std::string file_path = result[opt].as<std::string>();
					if (not std::filesystem::exists(file_path))
					{
						throw cxxopts::exceptions::exception(
							std::format("Option '--{}' provided file '{}' does not exist", opt, file_path));
					}

					if (not IsValeriaSource(file_path))
					{
						throw cxxopts::exceptions::exception(
							std::format("Option '--{}' provided file '{}' must be .val file", opt, file_path));
					}
				}
			}
		} 
		catch (const cxxopts::exceptions::exception& e) 
		{
			std::cerr << e.what() << "\n\n";
			std::cerr << args.help() << "\n";
			return false;
		}

		return true;
	}

};