#pragma once

#include <variant>
#include <concepts>

namespace val
{

	template <typename TCommand>
	concept HasConvertToC = requires(const TCommand & cmd)
	{
		{ cmd.Convert_To_C() } -> std::same_as <std::string>;
	};

	template <typename DerivedCommand>
	requires HasConvertToC <DerivedCommand>
	class ICommand
	{
	private:

	public:
		std::string Convert_To_C() const noexcept
		{
			return static_cast <const DerivedCommand*>(this)->Convert_To_C();
		}
	};
}