#include <Psapi.h>
#include <windows.h>
#include <stdio.h>

int main(int argc, char **argv)
{
    if (argc != 3){
        printf("usage: injector <path to dll> <pid>");
        return 0;
    }
    PCSTR pathToDll = argv[1];
    DWORD pid = atoi(argv[2]);
    HANDLE processHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
    if (!processHandle){
        printf("Couldn't open process with pid %d, maybe the pid is invalid?", pid);
        return 1;
    }
    char processName[1024];
    int open_process_result;
    open_process_result = GetModuleBaseName(processHandle, NULL, processName, 1024 );
    if (open_process_result == 0){
        strcpy(&processName, "unknown");
    }
    printf("Injecting%s into pid: %d (%s)\n", pathToDll, pid, processHandle);
    // convert it to a number
    
    printf("Pid: %d\n", pid);

    
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