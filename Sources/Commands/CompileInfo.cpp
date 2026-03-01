#include "CompileInfo.h"

namespace val
{

	FieldType CompileInfo::GetSymbolType(const std::string& name) noexcept
	{
		return lifetime_st.at(name)[cur_index[name]];
	}

	void CompileInfo::AdvanceNextST(const std::string& name) noexcept
	{
		cur_index[name]++;
	}

	const std::unordered_map <std::string, FieldType>& CompileInfo::GetADTFieldInfo(const std::string& adt_name) const noexcept
	{
		return adts.at(adt_name);
	}

	std::pair <std::string, FieldType> CompileInfo::GetNextExpr() const noexcept
	{
		return valid_c_exprs.front();
	}

	void CompileInfo::PopExpr()
	{
		valid_c_exprs.pop();
	}

}