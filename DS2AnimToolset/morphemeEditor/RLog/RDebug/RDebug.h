#pragma once
#include <windows.h>
#include <string>
#include <debugapi.h>

enum MsgLevel
{
	MsgLevel_Debug = 2000000001,
	MsgLevel_Info = 1000000001,
	MsgLevel_Warn = 1300000001,
	MsgLevel_Error = 1400000001,
};

namespace RDebug
{
	void DebuggerOut(UINT usr_level, MsgLevel level, const char* fmt, ...);
	void SystemPanic(const char* src_module, const char* fmt, ...);
	void SystemAlert(UINT usr_level, MsgLevel level, const char* src_module, const char* fmt, ...);
}