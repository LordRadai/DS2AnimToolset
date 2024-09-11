#include "FileIDNamesTable.h"

std::wstring FileIDNamesTable::getPathById(int id)
{
	int idx = -1;

	for (size_t i = 0; i < this->m_numEntries; i++)
	{
		if (this->m_ids[i] == id)
			idx = i;
	}

	if (idx != -1)
		return this->getPath(idx);

	return L"";
}

void FileIDNamesTable::addEntry(std::wstring path, int id)
{
	this->m_numEntries++;
	this->m_paths.push_back(path);
	this->m_ids.push_back(id);
}

void FileIDNamesTable::clear()
{
	this->m_numEntries = 0;
	this->m_paths.clear();
	this->m_ids.clear();
}