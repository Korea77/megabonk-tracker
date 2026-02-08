#include <windows.h>
#include <tlhelp32.h>
#include <iostream>
#include <string>
#include <Shlwapi.h>
#pragma comment(lib, "Shlwapi.lib")

DWORD FindProcessIdByName(const std::string& processName) {
    DWORD pid = 0;
    HANDLE snap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (snap == INVALID_HANDLE_VALUE) return 0;

    PROCESSENTRY32 pe;
    pe.dwSize = sizeof(pe);
    if (Process32First(snap, &pe)) {
        do {
            if (_stricmp(pe.szExeFile, processName.c_str()) == 0) {
                pid = pe.th32ProcessID;
                break;
            }
        } while (Process32Next(snap, &pe));
    }
    CloseHandle(snap);
    return pid;
}

bool InjectDLL(DWORD pid, const std::string& dllPath) {
    if (pid == 0 || dllPath.empty()) return false;

    HANDLE hProc = OpenProcess(PROCESS_CREATE_THREAD | PROCESS_QUERY_INFORMATION |
        PROCESS_VM_OPERATION | PROCESS_VM_WRITE | PROCESS_VM_READ,
        FALSE, pid);
    if (!hProc) {
        std::cerr << "OpenProcess failed: " << GetLastError() << "\n";
        return false;
    }

    size_t size = dllPath.size() + 1;
    LPVOID remoteMem = VirtualAllocEx(hProc, nullptr, size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    if (!remoteMem) {
        std::cerr << "VirtualAllocEx failed: " << GetLastError() << "\n";
        CloseHandle(hProc);
        return false;
    }

    SIZE_T written = 0;
    if (!WriteProcessMemory(hProc, remoteMem, dllPath.c_str(), size, &written) || written != size) {
        std::cerr << "WriteProcessMemory failed: " << GetLastError() << "\n";
        VirtualFreeEx(hProc, remoteMem, 0, MEM_RELEASE);
        CloseHandle(hProc);
        return false;
    }

    LPVOID loadLibAddr = reinterpret_cast<LPVOID>(
        GetProcAddress(GetModuleHandleA("kernel32.dll"), "LoadLibraryA"));
    if (!loadLibAddr) {
        std::cerr << "GetProcAddress(LoadLibraryA) failed\n";
        VirtualFreeEx(hProc, remoteMem, 0, MEM_RELEASE);
        CloseHandle(hProc);
        return false;
    }

    HANDLE hThread = CreateRemoteThread(hProc, nullptr, 0,
        (LPTHREAD_START_ROUTINE)loadLibAddr,
        remoteMem, 0, nullptr);
    if (!hThread) {
        std::cerr << "CreateRemoteThread failed: " << GetLastError() << "\n";
        VirtualFreeEx(hProc, remoteMem, 0, MEM_RELEASE);
        CloseHandle(hProc);
        return false;
    }

    WaitForSingleObject(hThread, INFINITE);

    DWORD exitCode = 0;
    if (GetExitCodeThread(hThread, &exitCode)) {
        if (exitCode == 0) {
            std::cerr << "DLL failed to load\n";
        }
        else {
            std::cout << "DLL loaded at address: 0x" << std::hex << exitCode << std::dec << "\n";
        }
    }

    CloseHandle(hThread);
    VirtualFreeEx(hProc, remoteMem, 0, MEM_RELEASE);
    CloseHandle(hProc);
    return true;
}

std::string GetCurrentDirDLL() {
    char path[MAX_PATH] = { 0 };
    if (!GetModuleFileNameA(nullptr, path, MAX_PATH)) return "";
    PathRemoveFileSpecA(path); // usuwa nazwę exe
    std::string dir = path;
    if (dir.back() != '\\') dir += '\\';
    dir += "Menu.dll";
    return dir;
}

int main() {
    std::string dllPath = GetCurrentDirDLL();
    if (dllPath.empty()) {
        std::cerr << "Cannot get Menu.dll path\n";
        return 1;
    }

    DWORD pid = 0;
    while (pid == 0) {
        pid = FindProcessIdByName("Megabonk.exe");
        if (pid == 0) {
            std::cout << "Waiting for MegaBonk.exe...\n";
            Sleep(1000);
        }
    }

    std::cout << "Found MegaBonk.exe PID: " << pid << "\n";

    if (!InjectDLL(pid, dllPath)) {
        std::cerr << "Injection failed\n";
        return 1;
    }

    std::cout << "Injection successful!\n";
    return 0;
}
