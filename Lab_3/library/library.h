#ifndef LABA3_LIBRARY_H
#define LABA3_LIBRARY_H

extern "C" __declspec(dllexport) void FindAndReplace(LPCSTR, LPCSTR);
extern "C" __declspec(dllexport) int DumpMem(DWORD dwPid, PCSTR pszDumpFilePath);

#endif
