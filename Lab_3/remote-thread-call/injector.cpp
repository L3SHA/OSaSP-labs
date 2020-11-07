#include <windows.h>
#include <iostream>

using namespace std;

int main()
{
    int pid;
    cin>>pid;

    HANDLE hProcess = OpenProcess(PROCESS_VM_READ | PROCESS_VM_OPERATION | PROCESS_VM_WRITE
            | PROCESS_CREATE_THREAD | PROCESS_CREATE_PROCESS, FALSE, pid);

    if (!hProcess)
    {
        return 1;
    }

    char lib[] = "remote_entry.dll";

    auto loadLibrary = (LPVOID) GetProcAddress(GetModuleHandle("kernel32.dll"), "LoadLibraryA");
    auto memory = (LPVOID) VirtualAllocEx(hProcess, nullptr, strlen(lib),
                                          MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
    WriteProcessMemory(hProcess, memory, lib, strlen(lib), nullptr);
    CreateRemoteThread(hProcess, nullptr, 0, (LPTHREAD_START_ROUTINE) loadLibrary,
                       memory, NULL, nullptr);

    WaitForSingleObject(hProcess, INFINITE);
    CloseHandle(hProcess);

    return 0;
}

