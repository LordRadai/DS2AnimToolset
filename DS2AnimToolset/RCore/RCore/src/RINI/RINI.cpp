#include "RINI.h"
#include "RString/RString.h"

//Create an INI object from the specified filepath
RINI* RINI::open(std::string path)
{
	RINI* ini = new RINI;
	ini->m_path = path;

	mINI::INIFile file(path);

	if (!file.read(ini->m_data))
		return nullptr;

	return ini;
}

//Creates an INI object
RINI* RINI::create(std::string path)
{
	RINI* ini = new RINI;
	ini->m_path = path;

	return ini;
}

//Free memory
void RINI::destroy()
{
	delete this;
}

//Generate INI file from. Set pretty to true for a more spaced output
void RINI::write(bool pretty)
{
	mINI::INIFile file(this->m_path);

	file.write(this->m_data, pretty);
	file.generate(this->m_data, pretty);
}

bool RINI::getBool(std::string section, std::string key, bool vDefault)
{
	if (!this->m_data.has(section) || !this->m_data.get(section).has(key))
		return vDefault;

	std::string value = this->m_data.get(section).get(key);

	if (value == "false")
		return false;

	return true;
}

int RINI::getInt(std::string section, std::string key, int vDefault)
{
	if (!this->m_data.has(section) || !this->m_data.get(section).has(key))
		return vDefault;

	return std::stoi(this->m_data.get(section).get(key));
}

float RINI::getFloat(std::string section, std::string key, float vDefault)
{
	if (!this->m_data.has(section) || !this->m_data.get(section).has(key))
		return vDefault;

	return std::stof(this->m_data.get(section).get(key));
}

std::string RINI::getString(std::string section, std::string key, std::string vDefault)
{
	if (!this->m_data.has(section) || !this->m_data.get(section).has(key))
		return vDefault;

	return this->m_data.get(section).get(key);
}

void RINI::setBool(std::string section, std::string key, bool value)
{
	if (value)
		this->m_data[section][key] = "true";
	else
		this->m_data[section][key] = "false";
}

void RINI::setInt(std::string section, std::string key, int value)
{
	this->m_data[section][key] = std::to_string(value);
}

void RINI::setFloat(std::string section, std::string key, float value)
{
	this->m_data[section][key] = std::to_string(value);
}

void RINI::setString(std::string section, std::string key, std::string value)
{
	this->m_data[section][key] = value;
}