#pragma once
#include <ostream>
#include <string>
#include <sstream>
#include <iomanip>

namespace RString
{
	std::string ToNarrow(const std::wstring& s);
	std::string FloatToString(float value, int precision = 3);
	std::wstring ToWide(const std::string& s);
	std::string RemoveExtension(std::string filename);
	std::string RemovePathAndExtension(std::string filename);
	std::string Replace(std::string str, std::string substr, std::string replacement);
}