#include <windows.h>
#include <stdio.h>

DWORD WINAPI threadFunc(LPVOID lpParam)
{
    printf("MWAGHAHHAHAH");
    return 0;
}

int main(int argc, char **argv)
{
    Sleep(5000);
    if (argc != 3){
        printf("What are you doing!?!?? ARGC was: %d\n", argc);
        return 0;
    }
    PCSTR pathToDll = argv[1];
    printf("%s\n", pathToDll);
    // convert it to a number
    DWORD pid = atoi(argv[2]);
    printf("Pid: %d\n", pid);

    HANDLE processHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
    if (processHandle == NULL)
    {
        printf("Process handle was null, kys.");
        return 1;
    }

    LPVOID mem = VirtualAllocEx(processHandle, NULL, strlen(pathToDll) + 1, (MEM_COMMIT | MEM_RESERVE), PAGE_READWRITE);

    if (mem == NULL)
    {
        printf("Couldn't allocate memory kys.");
        return 1;
    }

    BOOL result = WriteProcessMemory(processHandle, mem, pathToDll, strlen(pathToDll) + 1, NULL);

    if (result == FALSE)
    {
        printf("Couldn't write mem. kys");
        return 1;
    }

    HMODULE addressOfKernel32 = GetModuleHandleW(L"kernel32.dll");

    if (addressOfKernel32 == NULL)
    {
        printf("AH!\n");
        printf("%d", GetLastError());
        return 1;
    }

    FARPROC load_library_address = GetProcAddress(addressOfKernel32, "LoadLibraryA");

    if (load_library_address == NULL)
    {
        printf("ajkjsd");
    }

    HANDLE hThread = CreateRemoteThread(
        processHandle, NULL, 0,
        (LPTHREAD_START_ROUTINE)load_library_address,
        mem, 0, NULL);

    if (hThread == NULL)
    {
        printf("CreateRemoteThread failed. Error: %lu\n", GetLastError());
        return 1;
    }

    WaitForSingleObject(hThread, INFINITE);
    CloseHandle(processHandle);
    return 0;
}