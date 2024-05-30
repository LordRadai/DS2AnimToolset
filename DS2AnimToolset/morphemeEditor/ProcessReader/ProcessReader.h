#pragma once
#include <Windows.h>

class ProcessReader
{
public:
	ProcessReader();
	ProcessReader(DWORD dwProcID, wchar_t* szModuleName);
	~ProcessReader();

	HMODULE GetModuleAddress();
	BOOL Read(LPCVOID pBaseAddress, SIZE_T size, LPVOID pBuffer);
	BOOL Write(LPVOID pBaseAddress, LPCVOID pValue, SIZE_T size);

private:
	HMODULE m_moduleAddr;
};