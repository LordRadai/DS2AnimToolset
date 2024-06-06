#include "INI.h"
#include "utils/RString/RString.h"

INI::INI()
{
}

INI::~INI()
{
}

bool INI::Open(std::string path)
{
	mINI::INIFile file(path);

	return file.read(this->m_data);
}

bool INI::GetBool(std::string section, std::string key, bool vDefault)
{
	if (!this->m_data.has(section) || !this->m_data.get(section).has(key))
		return vDefault;

	std::string value = this->m_data.get(section).get(key);

	if (value == "false")
		return false;

	return true;
}

int INI::GetInt(std::string section, std::string key, int vDefault)
{
	if (!this->m_data.has(section) || !this->m_data.get(section).has(key))
		return vDefault;

	return std::stoi(this->m_data.get(section).get(key));
}

float INI::GetFloat(std::string section, std::string key, float vDefault)
{
	if (!this->m_data.has(section) || !this->m_data.get(section).has(key))
		return vDefault;

	return std::stof(this->m_data.get(section).get(key));
}

std::string INI::GetString(std::string section, std::string key, std::string vDefault)
{
	if (!this->m_data.has(section) || !this->m_data.get(section).has(key))
		return vDefault;

	return this->m_data.get(section).get(key);
}

std::wstring INI::GetWString(std::string section, std::string key, std::wstring vDefault)
{
	if (!this->m_data.has(section) || !this->m_data.get(section).has(key))
		return vDefault;

	return RString::ToWide(this->m_data.get(section).get(key));
}