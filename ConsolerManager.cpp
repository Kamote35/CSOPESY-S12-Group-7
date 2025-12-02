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
    // Re-implement cmdScreenList logic using Scheduler::getInstance()
    // This outputs the specific screen -ls format [cite: 224]
    Scheduler& sched = Scheduler::getInstance();
    lock_guard<mutex> lock(sched.getLock());
    
    // ... (Copy logic from prototype cmdScreenList) ...
}

void ConsoleManager::attachToProcess(const string &rawName) {
    // Re-implement attach logic [cite: 179]
    // Handles process-smi loop
}