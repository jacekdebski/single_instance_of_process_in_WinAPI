#include <windows.h>
#include <tchar.h>
#include <iostream>
#include <psapi.h>
#include <tlhelp32.h>

#define ENVP_SIZE 5
#define ENVP_PATTERN_VALUE "NEW"
#define NAME_PROCESS "prog.exe"
#define NAME_MUTEX_LOCK "mutex_lock"

BOOL terminate_other_instances_of_process() {
    HANDLE hProcessSnap;
    HANDLE hOpenProcess;
    PROCESSENTRY32 pe32;
    BOOL res;

    hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hProcessSnap == INVALID_HANDLE_VALUE) {
        std::cout << "Error occurred!" << std::endl;
        return false;
    }
    pe32.dwSize = sizeof(PROCESSENTRY32);
    if (Process32First(hProcessSnap, &pe32)) {
        do {
            if (strcmp(pe32.szExeFile, NAME_PROCESS) == 0) {
                if (GetCurrentProcessId() != pe32.th32ProcessID) {
                    hOpenProcess = OpenProcess(PROCESS_TERMINATE, FALSE, pe32.th32ProcessID);
                    TerminateProcess(hOpenProcess, 1);
                    CloseHandle(hOpenProcess);
                }
            }
        } while (Process32Next(hProcessSnap, &pe32));
    } else {
        std::cout << "Error occurred!" << std::endl;
        res = FALSE;
    }
    CloseHandle(hProcessSnap);
    return res;
}

BOOL check_value_environmental_variable() {
    BOOL res;
    TCHAR envp[ENVP_SIZE];
    GetEnvironmentVariable("ENVP_VAR", envp, ENVP_SIZE);
    if (GetLastError() == ERROR_ENVVAR_NOT_FOUND) {
        res = FALSE;
    } else {
        if (strcmp(envp, ENVP_PATTERN_VALUE) == 0) {
            res = TRUE;
        } else {
            res = FALSE;
        }
    }
    return res;
}

BOOL create_mutex() {
    CreateMutexA(NULL, TRUE, NAME_MUTEX_LOCK);
    if (GetLastError() == ERROR_ALREADY_EXISTS) {
        return FALSE;
    }
    return TRUE;
}

int _tmain(int argc, TCHAR *argv[], TCHAR *envp[]) {
    if(strcmp(strrchr(*argv,'\\') + 1,NAME_PROCESS) !=0 ){
        std::cout << "Correct name of program should be: "<< NAME_PROCESS << std::endl;
        return 1;
    }
    if (check_value_environmental_variable() == TRUE) {
        if (terminate_other_instances_of_process() == TRUE) {
            std::cout << "Other processes have terminated if it existed..." << std::endl;
        }
    } else {
        if (create_mutex() == FALSE) {
            std::cout << "Another process is running" << std::endl;
            return 1;
        }
    }
    system("pause");
    return 0;
}
