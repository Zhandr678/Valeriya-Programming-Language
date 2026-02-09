#pragma once

class DSU
{
private:

public:

	void Union(const std::string& a, const std::string& b);

	bool EqualPointers(const std::string& a, const std::string& b) const;

	void ReleaseA(const std::string& a, const std::string& from);

	bool Single(const std::string& a);

	bool Has(const std::string& a) const;

	void Add(const std::string& a);
};