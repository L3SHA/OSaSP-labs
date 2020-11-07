#include <windows.h>
#include "library.h"
#include <stdio.h>

int DumpMem(DWORD dwPid, PCSTR pszDumpFilePath)
{
    HANDLE hProc = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwPid);
    if (!hProc) {
    return 1;
    }

    SYSTEM_INFO sysinfo;
    GetSystemInfo(&sysinfo);
    DWORD_PTR pMaxAddr = (DWORD_PTR)sysinfo.lpMaximumApplicationAddress;
    DWORD_PTR pMinAddr = (DWORD_PTR)sysinfo.lpMinimumApplicationAddress;
    MEMORY_BASIC_INFORMATION mbi;
    PCHAR pcaBuff = new CHAR[sysinfo.dwPageSize];

    FILE *pDumpFile;
    fopen_s(&pDumpFile, pszDumpFilePath, "wb");
    for (DWORD_PTR pAddr = pMinAddr; pAddr < pMaxAddr; pAddr += sysinfo.dwPageSize) {
        VirtualQueryEx(hProc, (LPCVOID)pAddr, &mbi, sizeof(mbi));
        if (mbi.Protect == PAGE_READWRITE && mbi.State == MEM_COMMIT) {
            ZeroMemory(pcaBuff, sysinfo.dwPageSize);
            ReadProcessMemory(hProc, (LPCVOID)pAddr, pcaBuff, sysinfo.dwPageSize, NULL);
            fwrite(pcaBuff, sizeof(char), sysinfo.dwPageSize, pDumpFile);
        }
    }
    fclose(pDumpFile);

    CloseHandle(hProc);
    delete[] pcaBuff;
    return 0;
}

void FindAndReplace(LPCSTR dest, LPCSTR source)
{
    size_t destLen = strlen(dest);
    size_t sourceLen = strlen(source);

    SYSTEM_INFO systemInfo;
    GetSystemInfo(&systemInfo);

    MEMORY_BASIC_INFORMATION memoryInfo;
    auto baseAddress = (LPSTR) systemInfo.lpMinimumApplicationAddress;

    while (baseAddress < systemInfo.lpMaximumApplicationAddress)
    {
        if (VirtualQuery(baseAddress, &memoryInfo, sizeof(memoryInfo)))
        {
            if (memoryInfo.State == MEM_COMMIT && memoryInfo.AllocationProtect == PAGE_READWRITE)
            {
                char *buffer = (char *) malloc(memoryInfo.RegionSize);
                SIZE_T bytesRead;

                if (ReadProcessMemory(GetCurrentProcess(), baseAddress, buffer, memoryInfo.RegionSize, &bytesRead))
                {
                    for (SIZE_T i = 0; i < bytesRead - destLen; i++)
                    {
                        if (strcmp(baseAddress + i, dest) == 0)
                        {
                            memcpy(baseAddress + i, source, sourceLen + 1);
                        }
                    }
                }

                free(buffer);
            }
        }

        baseAddress += memoryInfo.RegionSize;
    }
}

#ifdef REMOTE_ENTRY

BOOL WINAPI DllMain(HINSTANCE, DWORD fdwReason, LPVOID) {
    if (fdwReason == DLL_PROCESS_ATTACH) {
        FindAndReplace("hello", "bye");
    }

    return TRUE;
}

#endif



