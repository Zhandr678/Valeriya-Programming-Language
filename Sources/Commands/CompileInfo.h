#pragma once

#include <unordered_map>
#include <queue>
#include "SemanticAnalysis/Types.h"

namespace val
{
	enum class FieldType { Primitive, Array, ADT, String };
	
	class CompileInfo
	{
	private:
		std::unordered_map<std::string, std::vector <FieldType>> lifetime_st;
		std::unordered_map<std::string, size_t> cur_index;

		std::unordered_map <std::string, std::unordered_map<std::string, FieldType>> adts;
		std::queue <std::string> valid_c_exprs;
		
		friend class Semantics;
	public:
		FieldType GetSymbolType(const std::string& name) noexcept;

		void AdvanceNextST(const std::string& name) noexcept;

		FieldType GetADTFieldInfo(const std::string& adt_name, const std::string& field_name) const noexcept;

		std::string GetNextExpr() const noexcept;

		void PopExpr();
	};

	
}