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

std::string RString::GuidToString(GUID guid)
{
	char guid_cstr[39];
	snprintf(guid_cstr, sizeof(guid_cstr),
		"%08x-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x",
		guid.Data1, guid.Data2, guid.Data3,
		guid.Data4[0], guid.Data4[1], guid.Data4[2], guid.Data4[3],
		guid.Data4[4], guid.Data4[5], guid.Data4[6], guid.Data4[7]);

	return std::string(guid_cstr);
}