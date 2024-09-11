#pragma once

class TextLog
{
public:
	TextLog();
	TextLog(std::string outPath);
	~TextLog();

	bool isInitialised();
	void setLogPath(std::string path);
	std::string getLogPath();

	bool createLogFile();
	bool addLog(bool print_time, const char* fmt, ...);

private:
	bool m_init;
	std::string m_outPath;
};