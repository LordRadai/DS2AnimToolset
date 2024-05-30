#include <time.h>
#include <fstream>
#include <ostream>
#include <filesystem>
#include "TextLog.h"
#include "../RDebug/RDebug.h"

inline std::string getCurrentDateTime(std::string format) 
{
    time_t now = time(0);
    struct tm tstruct = *localtime(&now);
    char  buf[80];

    if (format.compare("now") == 0)
        strftime(buf, sizeof(buf), "%d-%m-%Y %X", &tstruct);
    else if (format.compare("date") == 0)
        strftime(buf, sizeof(buf), "%d-%m-%Y", &tstruct);
    else if (format.compare("time") == 0)
        strftime(buf, sizeof(buf), "%X", &tstruct);
    else
        RDebug::DebuggerOut(MsgLevel_Error, MsgLevel_Error, "Invalid format type %s\n", format);

    return std::string(buf);
};

TextLog::TextLog()
{
    this->m_init = false;
    this->m_outPath = "";
}

TextLog::TextLog(std::string outPath)
{
    this->m_init = true;
    this->m_outPath = outPath;

    this->CreateLogFile();
}

TextLog::~TextLog()
{
    this->m_init = false;
    this->m_outPath = "";
}

bool TextLog::GetInitStatus()
{
    return this->m_init;
}

void TextLog::SetLogPath(std::string path)
{
    this->m_outPath = path;
}

std::string TextLog::GetLogPath()
{
    return this->m_outPath;
}

bool TextLog::CreateLogFile()
{
    if (!this->m_init)
        return false;

    std::filesystem::create_directories(std::filesystem::path(this->m_outPath).parent_path());

    std::string filePath = this->m_outPath;
    std::string now = "[" + getCurrentDateTime("now") + "]";

    std::ofstream out(filePath.c_str(), std::ios_base::out);
    out.close();

    return true;
}

bool TextLog::AddLog(bool print_time, const char* fmt, ...)
{
    if (!this->m_init)
        return false;

    va_list args;
	__va_start(&args, fmt);
	std::string msg;

	char msg_buf[1000];

	msg = fmt;
	vsprintf_s(msg_buf, msg.c_str(), args);

    std::string filePath = this->m_outPath;
    std::string now = "[" + getCurrentDateTime("now") + "]";

    std::ofstream out(filePath.c_str(), std::ios_base::out | std::ios_base::app);

    if (print_time)
        out << now << '\t' << msg_buf;
    else
        out << msg_buf;

    out.close();

    return true;
}