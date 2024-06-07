#include <filesystem>
#include "RString.h"

std::string RString::ToNarrow(const std::wstring& s)
{
	return std::string(s.begin(), s.end());
}

std::string RString::FloatToString(float value, int precision)
{
	std::stringstream ss;
	ss << std::fixed << std::setprecision(precision) << value;
	return ss.str();
}

std::wstring RString::ToWide(const std::string& s)
{
	return std::wstring(s.begin(), s.end());
}

std::string RString::RemoveExtension(std::string filename)
{
	return std::filesystem::path(filename).stem().string();
}

std::string RString::Replace(std::string str, std::string substr, std::string replacement)
{
	std::string new_str;
	size_t pos = str.find(substr);

	if (pos != std::string::npos) 
		str.replace(pos, substr.length(), replacement);

	new_str = str;

	return new_str;
}