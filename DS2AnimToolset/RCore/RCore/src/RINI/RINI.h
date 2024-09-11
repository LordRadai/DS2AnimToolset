#pragma once
#define MINI_CASE_SENSITIVE

#include <string>
#include "mini/ini.h"

class RINI
{
public:
	static RINI* open(std::string path);
	static RINI* create(std::string path);

	std::string getPath() const { return this->m_path; }
	void setPath(std::string path) { this->m_path = path; }

	void destroy();

	void write(bool pretty = true);

	bool getBool(std::string section, std::string key, bool vDefault);
	int getInt(std::string section, std::string key, int vDefault);
	float getFloat(std::string section, std::string key, float vDefault);
	std::string getString(std::string section, std::string key, std::string vDefault);

	void setBool(std::string section, std::string key, bool value);
	void setInt(std::string section, std::string key, int value);
	void setFloat(std::string section, std::string key, float value);
	void setString(std::string section, std::string key, std::string value);

private:
	RINI() {}
	~RINI() {}

	std::string m_path = "";
	mINI::INIStructure m_data;
};