#include "ConsoleManager.h"
#include "Scheduler.h"
#include "Process.h"
#include "Global.h"
#include <iostream>
#include <iomanip>

using namespace std;

void ConsoleManager::clearScreen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

void ConsoleManager::printHeader() {
    cout << "------------------------------------\n";
    cout << "Welcome to CSOPESY Emulator!\n\n";
    cout << "Developers:\n";
    // ... [Add names from prototype]
    cout << "Last updated: " << __DATE__ << "\n";
    cout << "------------------------------------\n";
}

void ConsoleManager::cmdScreenList() {
    Scheduler& sched = Scheduler::getInstance();
    lock_guard<mutex> lock(sched.getLock());
    auto& procList = sched.getProcessList(); 

    if (procList.empty()) {
        cout << "CPU utilization: 0%\n";
        cout << "Cores used: 0\n";
        cout << "Cores available: " << g_Config.numCPU << "\n";
        cout << "------------------------------------\n";
        cout << "(no processes)\n";
        return;
    }

    if (procList.empty()) {
        cout << "CPU utilization: 0%\n";
        cout << "Cores used: 0\n";
        cout << "Cores available: " << g_Config.numCPU << "\n";
        cout << "------------------------------------\n";
        cout << "(no processes)\n";
        return;
    }

    int total = (int)procList.size();
    int finished = 0;
    for (auto &kv : procList)
        if (kv.second->isFinished()) finished++;

    int running = total - finished;
    int usedCores = min(running, g_Config.numCPU);
    int available = max(0, g_Config.numCPU - usedCores);

    cout << "CPU utilization: " << fixed << setprecision(1)
         << (100.0 * usedCores / g_Config.numCPU) << "%\n";
    cout << "Cores used: " << usedCores << "\n";
    cout << "Cores available: " << available << "\n";
    cout << "------------------------------------\n";

    cout << "Running processes:\n";
    for (auto &kv : procList) {
        auto &p = kv.second;
        if (!p->isFinished()) {
            time_t now = time(nullptr);
            tm local_tm = *localtime(&now);
            char buf[64];
            strftime(buf, sizeof(buf), "(%m/%d/%Y %I:%M:%S%p)", &local_tm);
            
            cout << left << setw(10) << p->getName()
                 << " " << buf
                 << "   Core: " << p->getCoreAssigned()
                 << "   " << p->getCurrentInstructionLine() << " / " << p->getTotalLines() << "\n";
        }
    }

    cout << "\nFinished processes:\n";
    for (auto &kv : procList) {
        auto &p = kv.second;
        if (p->isFinished()) {
            time_t now = time(nullptr);
            tm local_tm = *localtime(&now);
            char buf[64];
            strftime(buf, sizeof(buf), "(%m/%d/%Y %I:%M:%S%p)", &local_tm);
            
            cout << left << setw(10) << p->getName()
                 << " " << buf
                 << "   Finished   " << p->getTotalLines() << " / " << p->getTotalLines() << "\n";
        }
    }
    cout << "------------------------------------\n";
}

void ConsoleManager::attachToProcess(const std::string&processName) {
    Scheduler& sched = Scheduler::getInstance();
    Process* p = sched.getProcess(processName);

    if (!p) {
        cout << "Process '" << processName << "' not found.\n";
        return;
    }
    if (p->isFinished()) {
        cout << "Process " << p->getName() << " is finished.\n";
        return;
    }

    clearScreen();
    cout << "Attached to process: " << processName << "\nType 'process-smi' for status, 'exit' to return.\n";

    string cmd;
    // Inner input loop
    while (true) {
        cout << "proc> ";
        if (!getline(cin, cmd)) break;
        
        // trim cmd
        auto l = cmd.find_first_not_of(" \t");
        if(l != string::npos) cmd = cmd.substr(l, cmd.find_last_not_of(" \t") - l + 1);

        if (cmd == "exit") {
            clearScreen();
            printHeader(); // Reprint main menu header
            break;
        }

        if (cmd == "process-smi") {
            cout << "\nProcess name: " << p->getName() << "\n";
            cout << "ID: " << p->getId() << "\n";
            cout << "Logs:\n";
            
            // Get logs safely
            vector<string> logs = p->snapshotLogs();
            for (const auto &line : logs) cout << line << "\n";
            
            cout << "\nCurrent instruction line: " << p->getCurrentInstructionLine() << "\n";
            cout << "Lines of code: " << p->getTotalLines() << "\n\n";
            
            if (p->isFinished()) cout << "(process finished)\n";
        } else {
            cout << "Unknown command in process screen.\n";
        }
    }
}