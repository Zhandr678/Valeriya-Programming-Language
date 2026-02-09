#include "Types.h"

namespace val
{

	std::string PrimToStr(const PrimitiveType& type)
	{
		switch (type)
		{
		case PrimitiveType::INT:
			return "int";
		case PrimitiveType::UINT:
			return "uint";
		case PrimitiveType::DOUBLE:
			return "double";
		case PrimitiveType::BOOL:
			return "bool";
		case PrimitiveType::CHAR:
			return "char";
		case PrimitiveType::STRING:
			return "string";
		default:
			return "void";
		}
	}

	PrimitiveType StrToPrim(const std::string& strtype)
	{
		if (strtype == "int") { return PrimitiveType::INT; }
		else if (strtype == "uint") { return PrimitiveType::UINT; }
		else if (strtype == "double") { return PrimitiveType::DOUBLE; }
		else if (strtype == "bool") { return PrimitiveType::BOOL; }
		else if (strtype == "char") { return PrimitiveType::CHAR; }
		else if (strtype == "string") { return PrimitiveType::STRING; }
		else { return PrimitiveType::VOID; }
	}

	bool IsPrimitive(const std::string& strtype)
	{
		return strtype == "int" || strtype == "uint" || strtype == "double" || strtype == "bool" ||
			strtype == "char" || strtype == "string" || strtype == "void";
	}
}