//  Code to run a process as a protected process
//  Authors:
//      Michael DePlante
//      Nicholas Zubrisky
//
//  Special thanks to @itm4n for their work on PPLcontrol.
//  https://github.com/itm4n/PPLcontrol
//
//  This code must be run as an administrator to work successfully
//

#include <Windows.h>
#include <cstdio>
#include <iostream>
#include <string>
#include "Driver.h"
#include "OffsetFinder.h"
#include "RTCore.h"
#include "Controller.h"

Driver driver;

int wmain(int argc, wchar_t* argv[]) {
    std::wstring binary_path = L"";
    std::wstring binary_args = L"";
    STARTUPINFO info = { sizeof(info) };
    PROCESS_INFORMATION pi;
    DWORD pid = NULL;
    OffsetFinder* of;
    Controller* ctrl;

    if (argc < 2) {
        wprintf(L"Enter path to binary to start: ");
        std::getline(std::wcin, binary_path);
        wprintf(L"Enter any arguments for the binary: ");
        std::getline(std::wcin, binary_args);
    } else {
        binary_path = argv[1];
        for (int i = 2; i < argc; i++) {
            binary_args.append(argv[i]);
            binary_args.append(L" ");
        }
    }

    if (!CreateProcessW(_wcsdup(binary_path.c_str()), _wcsdup(binary_args.c_str()), NULL, NULL, FALSE, CREATE_NEW_CONSOLE, NULL, NULL, &info, &pi)) {
        wprintf(L"Error creating process, Error Code: %d", GetLastError());
        return 1;
    }

    if (!driver.install_driver()) {
        wprintf(L"Driver installation failed! Exiting...\n");
        driver.uninstall_driver();
        return 1;
    }    

    pid = pi.dwProcessId;
    wprintf(L"%ws PID: %d\n", binary_path.c_str(), pid);

    of = new OffsetFinder();
    ctrl = new Controller(new RTCore(), of);

    if (!of->FindAllOffsets()) {
        wprintf(L"Failed to determine the required offsets. Exiting...\n");
        driver.uninstall_driver();
        return 1;
    }

    if (!ctrl->ProtectProcess(pid, L"PPL", L"WinTcb")) {
        wprintf(L"Failed to protect process. Exiting...\n");
        driver.uninstall_driver();
        return 1;
    }

    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

    driver.uninstall_driver();
    Sleep(15000);
    return 0;
}