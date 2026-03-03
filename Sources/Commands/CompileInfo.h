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

	struct ExpressionInfo
	{
		std::string valid_expr;
		FieldType expr_type;
		bool is_lvalue;
	};
	
	class CompileInfo
	{
	private:
		std::unordered_map<std::string, std::vector <FieldType>> lifetime_st;
		std::unordered_map<std::string, size_t> cur_index;

		std::unordered_map <std::string, std::unordered_map<std::string, FieldType>> adts;

		std::queue <ExpressionInfo> valid_c_exprs;
		
		friend class Semantics;
	public:
		FieldType GetSymbolType(const std::string& name) noexcept;

		void AdvanceNextST(const std::string& name) noexcept;

		const std::unordered_map <std::string, FieldType>& GetADTFieldInfo(const std::string& adt_name) const noexcept;

		ExpressionInfo GetNextExpr() const noexcept;

		void PopExpr();
	};

	
}