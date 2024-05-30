#pragma once
#include "../../common/common.h"

#include "../endian/endian.h"
#include "../../../umem.h"

#include <map>

#ifndef CFR_STRINGS
#define CFR_STRINGS

namespace cfr
{
	//reads string till length or null term and returns it
	//throws exceptions on eof
	std::string freadString(UMEM* src, bool nullTerm = true, int length = 0);

	//returns just the file name when given a full path
	//throws exceptions if it doesn't get what it wants
	//std::string fileNameFromPath(std::string path);

	//returns the file name and the path as a pair
	std::pair<std::string,std::string> splitFullPath(std::string fullpath);

	std::wstring readWString(UMEM* src, bool endian);

	std::string readString(UMEM* src);

	int getWideStringLength(wchar_t* str);

	//seeks to string, reads till null, returns wchar array, sets len, returns to start
	wchar_t* getUniversalString(long offset, bool unicode, UMEM* src, int* len);

	char* jisToUtf8(char* str, int lengthIn, std::map<int,int>* charMap, int* lengthOut);
};

#endif