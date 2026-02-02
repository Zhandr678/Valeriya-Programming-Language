#pragma once

#include <vector>
#include <string>
#include <variant>
#include <memory>

namespace val
{
	enum class PrimitiveType { INT, UINT, DOUBLE, BOOL, CHAR, STRING };

	std::string PrimToStr(const PrimitiveType& type);
	PrimitiveType StrToPrim(const std::string& strtype);
	bool IsPrimitive(const std::string& strtype);

	class ObjectKind;
	class ArrayKind;
	using VariableKind = std::variant <std::unique_ptr <ObjectKind>, std::unique_ptr <ArrayKind>>;

	struct ObjectKind
	{
		bool is_primitive;
		std::string type_name;
	};

	struct ArrayKind
	{
		std::string valid_size_expression; // Directly translatable to C
		bool is_dynamic;
		VariableKind of_kind;
	};

	struct StructType
	{
		std::string name;

		struct Field
		{
			std::string field_name;
			VariableKind kind_of_field;
		};

		std::vector <Field> fields;
	};

	struct EnumType
	{
		std::string name;
		std::vector <std::string> enum_variants;
	};

	struct PropertyType
	{
		std::string name;

		struct Option
		{
			std::string opt_name;
			StructType opt_fields;
		};

		std::vector <Option> options;
	};

	using UnknownType = std::monostate;
	using TypeClass = std::variant <PrimitiveType, StructType, PropertyType, EnumType, UnknownType>;
	using CustomTypeClass = std::variant <StructType, PropertyType, EnumType, UnknownType>;
}