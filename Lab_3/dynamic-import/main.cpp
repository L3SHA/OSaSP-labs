#include <windows.h>
#include <iostream>

typedef void TFindAndReplace(LPCSTR dest, LPCSTR source);
typedef int TDumpMem(DWORD dwPid, PCSTR pszDumpFilePath);

using namespace std;

int main() {
    HMODULE lib = LoadLibrary("library.dll");
    if (lib != nullptr) {
        TFindAndReplace *pFindAndReplace;
        TDumpMem *pDumpMem;
        pFindAndReplace = (TFindAndReplace *) GetProcAddress(lib, "FindAndReplace");
        pDumpMem = (TDumpMem *) GetProcAddress(lib, "DumpMem");
        pDumpMem(GetCurrentProcessId(), "test.txt");
        char str1[] = "hello";
        char str2[] = "bye";
        cout << str1 << endl << str2 << endl << "----" << endl;
        pFindAndReplace(str1, str2);
        cout << str1 << endl << str2;

        FreeLibrary(lib);
    }
    return 0;
}
