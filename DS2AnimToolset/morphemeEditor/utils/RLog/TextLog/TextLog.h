#pragma once

class TextLog
{
public:
	TextLog();
	TextLog(std::string outPath);
	~TextLog();

	bool GetInitStatus();
	void SetLogPath(std::string path);
	std::string GetLogPath();

	bool CreateLogFile();
	bool AddLog(bool print_time, const char* fmt, ...);

private:
	bool m_init;
	std::string m_outPath;
};