#include <iostream>
#include <vector>
#include <string>
#include <windows.h>
#include <psapi.h>
#include <tlhelp32.h>

struct ProcessInfo {
    DWORD pid;           // Process ID
    std::string name;    // Process name
    double cpuUsage;     // CPU usage (placeholder)
    SIZE_T memoryUsage;  // Memory usage in bytes
};

std::vector<ProcessInfo> getProcessList() {
    std::vector<ProcessInfo> processes;
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnapshot == INVALID_HANDLE_VALUE) {
        std::cerr << "Failed to create snapshot!" << std::endl;
        return processes;
    }

    PROCESSENTRY32 pe32;
    pe32.dwSize = sizeof(PROCESSENTRY32);

    if (Process32First(hSnapshot, &pe32)) {
        do {
            ProcessInfo info;
            info.pid = pe32.th32ProcessID;
            info.name = pe32.szExeFile;

            HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, info.pid);
            if (hProcess) {
                PROCESS_MEMORY_COUNTERS pmc;
                if (GetProcessMemoryInfo(hProcess, &pmc, sizeof(pmc))) {
                    info.memoryUsage = pmc.WorkingSetSize;
                }
                CloseHandle(hProcess);
            }
            info.cpuUsage = 0.0; // Simplified (real CPU usage needs time sampling)
            processes.push_back(info);
        } while (Process32Next(hSnapshot, &pe32));
    }

    CloseHandle(hSnapshot);
    return processes;
}

void displayProcesses(const std::vector<ProcessInfo>& processes) {
    std::cout << "PID\tName\t\tCPU Usage (%)\tMemory Usage (KB)" << std::endl;
    std::cout << "----------------------------------------------------" << std::endl;
    for (const auto& proc : processes) {
        std::cout << proc.pid << "\t" 
                  << proc.name.substr(0, 15) << "\t" 
                  << proc.cpuUsage << "\t\t" 
                  << proc.memoryUsage / 1024 << std::endl;
    }
}

int main() {
    std::cout << "Process Monitoring Tool\n" << std::endl;
    std::vector<ProcessInfo> processes = getProcessList();
    displayProcesses(processes);
    return 0;
}