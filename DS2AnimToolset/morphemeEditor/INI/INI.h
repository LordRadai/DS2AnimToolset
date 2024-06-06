#include <string>
#include "mini/ini.h"

class INI
{
public:
	INI();
	~INI();

	bool Open(std::string path);

	bool GetBool(std::string section, std::string key, bool vDefault);
	int GetInt(std::string section, std::string key, int vDefault);
	float GetFloat(std::string section, std::string key, float vDefault);
	std::string GetString(std::string section, std::string key, std::string vDefault);
	std::wstring GetWString(std::string section, std::string key, std::wstring vDefault);

private:
	mINI::INIStructure m_data;
};