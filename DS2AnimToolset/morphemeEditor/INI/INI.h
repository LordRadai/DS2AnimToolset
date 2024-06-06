#pragma once
#include <string>
#include "mini/ini.h"

class INI
{
public:
	INI();
	~INI();

	bool Open(std::string path);
	void Write(std::string path, bool pretty = true);

	bool GetBool(std::string section, std::string key, bool vDefault);
	int GetInt(std::string section, std::string key, int vDefault);
	float GetFloat(std::string section, std::string key, float vDefault);
	std::string GetString(std::string section, std::string key, std::string vDefault);

	void SetBool(std::string section, std::string key, bool value);
	void SetInt(std::string section, std::string key, int value);
	void SetFloat(std::string section, std::string key, float value);
	void SetString(std::string section, std::string key, std::string value);

private:
	mINI::INIStructure m_data;
};