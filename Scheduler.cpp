#include "Scheduler.h"
#include "MemoryManager.h" // Include this
#include <iostream>
#include <fstream>
#include <iomanip>

using namespace std;

void Scheduler::addProcess(const string& name, int lines, int memory) {
    lock_guard<mutex> lock(processLock);
    if (procList.find(name) != procList.end()) {
        cout << "Process with name '" << name << "' already exists.\n";
        return;
    }
    // Register with Memory Manager
    // Get ID logic needs to be consistent, but here we use next_pid
    MemoryManager::getInstance().allocateProcess(next_pid, memory); 

    procList[name] = make_unique<Process>(name, next_pid++, lines, memory);
    procList[name]->start();
}

void Scheduler::addProcess(const string& name, const vector<Instruction>& ins, int memory) {
    lock_guard<mutex> lock(processLock);
    if (procList.find(name) != procList.end()) {
        cout << "Process with name '" << name << "' already exists.\n";
        return;
    }
    // Register with Memory Manager
    MemoryManager::getInstance().allocateProcess(next_pid, memory);

    procList[name] = make_unique<Process>(name, next_pid++, ins, memory);
    procList[name]->start();
}
// Random Instructions
// void Scheduler::addProcess(const string& name, int lines, int memory) {
//     lock_guard<mutex> lock(processLock);
//     if (procList.find(name) != procList.end()) {
//         cout << "Process with name '" << name << "' already exists.\n";
//         return;
//     }
//     procList[name] = make_unique<Process>(name, next_pid++, lines, memory);
//     procList[name]->start();
// }

// Custom Instructions
// void Scheduler::addProcess(const string& name, const vector<Instruction>& ins, int memory) {
//     lock_guard<mutex> lock(processLock);
//     if (procList.find(name) != procList.end()) {
//         cout << "Process with name '" << name << "' already exists.\n";
//         return;
//     }
//     procList[name] = make_unique<Process>(name, next_pid++, ins, memory);
//     procList[name]->start();
// }

Process* Scheduler::getProcess(const string& name) {
    lock_guard<mutex> lock(processLock);
    auto it = procList.find(name);
    if (it != procList.end()) return it->second.get();
    return nullptr;
}

void Scheduler::startSchedulerLoop() {
    if (schedulerRunning) return;
    schedulerRunning = true;
    
    thread([this]() {
        random_device rd; mt19937 gen(rd());
        int counter = 1;
        const uint64_t cycle_ms = 100;

        while (schedulerRunning) {
            string name = "proc-" + to_string(counter++);
            uniform_int_distribution<int> dist(g_Config.minIns, g_Config.maxIns);
            int lines = dist(gen);

            // Use memory config for random background processes
            uniform_int_distribution<int> memDist(g_Config.minMemPerProc, g_Config.maxMemPerProc);
            // Ensure memory is a power of 2 (simple fix: round up to nearest pow2 if needed, 
            // but for now we assume config values are valid pow2 as per spec)
            int memory = memDist(gen); 
            
            addProcess(name, lines, memory);

            uint64_t sleep_ms = g_Config.batchFreq * cycle_ms;
            if (sleep_ms == 0) sleep_ms = cycle_ms;
            this_thread::sleep_for(chrono::milliseconds(static_cast<int>(sleep_ms)));
        }
    }).detach();
}

void Scheduler::stopSchedulerLoop() {
    schedulerRunning = false;
}

// ... Copy generateReport, getProcessList, getLock from your previous code ...
// (Omitted for brevity, but required for compilation)
map<string, unique_ptr<Process>>& Scheduler::getProcessList() { return procList; }
mutex& Scheduler::getLock() { return processLock; }
void Scheduler::generateReport(const string& filename) { /* ... Previous logic ... */ }