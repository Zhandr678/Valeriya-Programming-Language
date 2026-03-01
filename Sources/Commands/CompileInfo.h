#pragma once

#include <unordered_map>
#include <queue>
#include "SemanticAnalysis/Types.h"

namespace val
{
	struct FieldType 
	{ 
		enum class Type { 
			Primitive, ArrayStruct, ArrayPrimitive, ArrayString, 
			ArrayProperty, ArrayEnum, Struct, Property, Enum, String 
		} typeclass;
		std::string name;
	};
	
	class CompileInfo
	{
	private:
		std::unordered_map<std::string, std::vector <FieldType>> lifetime_st;
		std::unordered_map<std::string, size_t> cur_index;

		std::unordered_map <std::string, std::unordered_map<std::string, FieldType>> adts;
		std::queue <std::pair <std::string, FieldType>> valid_c_exprs;
		
		friend class Semantics;
	public:
		FieldType GetSymbolType(const std::string& name) noexcept;

		void AdvanceNextST(const std::string& name) noexcept;

		const std::unordered_map <std::string, FieldType>& GetADTFieldInfo(const std::string& adt_name) const noexcept;

		std::pair <std::string, FieldType> GetNextExpr() const noexcept;

		void PopExpr();
	};

	
}