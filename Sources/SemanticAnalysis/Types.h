#pragma once

#include <vector>
#include <string>
#include <variant>
#include <memory>
#include <unordered_map>
#include <unordered_set>

namespace val
{

#pragma region("Variable Structs")

	struct ObjectKind;
	struct ArrayKind;
	using VariableKind = std::variant <ObjectKind, ArrayKind>;

	struct ObjectKind
	{
		bool is_primitive;
		std::string type_name;
	};

	struct ArrayKind
	{
		std::string valid_size_expression; // Directly translatable to C
		bool is_dynamic{0};
		ObjectKind of_kind;
	};

#pragma endregion

#pragma region("Type Structs")

	enum class PrimitiveType { INT, UINT, DOUBLE, BOOL, CHAR, STRING, VOID };

	std::string PrimToStr(const PrimitiveType& type);
	PrimitiveType StrToPrim(const std::string& strtype);
	bool IsPrimitive(const std::string& strtype);

	struct ArrayType
	{
		size_t dim;
		std::string type_name;
	};

	struct StructType
	{
		std::string name;
		std::vector <std::string> order_fields;
		std::unordered_map <std::string, VariableKind> fields;
	};

	struct EnumType
	{
		std::string name;
		std::unordered_set <std::string> enum_variants;
	};

	struct PropertyType
	{
		std::string name;
		std::unordered_map <std::string, StructType> options;
	};

	struct FnTable
	{
		std::string name;
		std::string ret_type_name;
		struct Param
		{
			bool is_inout{0};
			VariableKind kind;
		};
		std::unordered_map <std::string, Param> param_symbol_table;
		std::vector <std::string> order_param;
	};

	using AnyType = std::monostate;
	using TypeClass = std::variant <PrimitiveType, StructType, PropertyType, EnumType, ArrayType, AnyType>;

#pragma endregion
}