#include "INI.h"
#include "utils/RString/RString.h"

INI::INI()
{
}

INI::~INI()
{
}

//Generate INI structure from existing file
bool INI::Open(std::string path)
{
	mINI::INIFile file(path);

	return file.read(this->m_data);
}

//Generate INI file from. Set pretty to true for a more spaced output
void INI::Write(std::string path, bool pretty)
{
	mINI::INIFile file(path);

	file.generate(this->m_data, pretty);
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

void INI::SetBool(std::string section, std::string key, bool value)
{
	if (value)
		this->m_data[section][key] = "true";
	else
		this->m_data[section][key] = "false";
}

void INI::SetInt(std::string section, std::string key, int value)
{
	this->m_data[section][key] = value;
}

void INI::SetFloat(std::string section, std::string key, float value)
{
	this->m_data[section][key] = value;
}

void INI::SetString(std::string section, std::string key, std::string value)
{
	this->m_data[section][key] = value;
}