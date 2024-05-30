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
	size_t lastdot = filename.find_last_of(".");

	if (lastdot == std::string::npos) return filename;

	return filename.substr(0, lastdot);
}

std::string RString::RemovePathAndExtension(std::string filename)
{
	return std::filesystem::path(RemoveExtension(filename).c_str()).filename().string();
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