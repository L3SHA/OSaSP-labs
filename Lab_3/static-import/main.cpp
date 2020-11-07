#include <iostream>
#include <windows.h>

extern "C" __declspec(dllimport) void FindAndReplace(LPCSTR dest, LPCSTR source);
extern "C" __declspec(dllimport) int DumpMem(DWORD dwPid, PCSTR pszDumpFilePath);

using namespace std;

int main() {
    char str1[] = "hello";
    char str2[] = "bye";
    DumpMem(GetCurrentProcessId(), "test.txt");
    cout<<str1<<endl<<str2<<endl<<"----"<<endl;
    FindAndReplace(str1, str2);
    cout<<str1<<endl<<str2;
    return 0;
}
