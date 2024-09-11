#pragma once
#include <string>
#include <vector>

class FileIDNamesTable
{
public:
	FileIDNamesTable() {}
	~FileIDNamesTable() {}

	int getNumEntries() const { return this->m_numEntries; }
	std::wstring getPath(int idx) const { return this->m_paths[idx]; }
	int getName(int idx) const { return this->m_ids[idx]; }

	std::wstring getPathById(int id);

	void addEntry(std::wstring path, int id);
	void clear();
private:
	int m_numEntries = 0;
	std::vector<std::wstring> m_paths;
	std::vector<int> m_ids;
};