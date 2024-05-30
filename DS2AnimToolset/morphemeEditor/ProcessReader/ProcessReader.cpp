#include "ProcessReader.h"
#include <TlHelp32.h>
#include <memoryapi.h>

ProcessReader::ProcessReader()
{
    this->m_moduleAddr = nullptr;
}

ProcessReader::ProcessReader(DWORD dwProcID, wchar_t* szModuleName)
{
    this->m_moduleAddr = nullptr;

    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, dwProcID);

    if (hSnapshot != INVALID_HANDLE_VALUE)
    {
        MODULEENTRY32 ModuleEntry32;
        ModuleEntry32.dwSize = sizeof(MODULEENTRY32);
        if (Module32First(hSnapshot, &ModuleEntry32))
        {
            do
            {
                if (wcscmp(ModuleEntry32.szModule, szModuleName) == 0)
                {
                    this->m_moduleAddr = (HMODULE)ModuleEntry32.modBaseAddr;
                    break;
                }
            } while (Module32Next(hSnapshot, &ModuleEntry32));
        }
        CloseHandle(hSnapshot);
    }
}

ProcessReader::~ProcessReader()
{
    this->m_moduleAddr = nullptr;
}

HMODULE ProcessReader::GetModuleAddress()
{
    return this->m_moduleAddr;
}

BOOL ProcessReader::Read(LPCVOID pBaseAddress, SIZE_T size, LPVOID pBuffer)
{
    return ReadProcessMemory(this->m_moduleAddr, pBaseAddress, pBuffer, size, nullptr);
}

BOOL ProcessReader::Write(LPVOID pBaseAddress, LPCVOID pValue, SIZE_T size)
{
    return WriteProcessMemory(this->m_moduleAddr, pBaseAddress, pValue, size, nullptr);
}