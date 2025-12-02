#include "Scheduler.h"
#include <iostream>
#include <fstream>
#include <iomanip>

using namespace std;

void Scheduler::addProcess(const string& name, int lines) {
    lock_guard<mutex> lock(processLock);
    if (procList.find(name) != procList.end()) {
        cout << "Process with name '" << name << "' already exists.\n";
        return;
    }
    procList[name] = make_unique<Process>(name, next_pid++, lines);
    procList[name]->start();
}

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
        const uint64_t cycle_ms = 100; // Simulated cycle duration

        while (schedulerRunning) {
            string name = "proc-" + to_string(counter++);
            uniform_int_distribution<int> dist(g_Config.minIns, g_Config.maxIns);
            int lines = dist(gen);

            addProcess(name, lines);

            uint64_t sleep_ms = g_Config.batchFreq * cycle_ms;
            if (sleep_ms == 0) sleep_ms = cycle_ms;
            this_thread::sleep_for(chrono::milliseconds(static_cast<int>(sleep_ms)));
        }
    }).detach();
}

void Scheduler::stopSchedulerLoop() {
    schedulerRunning = false;
}

// ... (Copy handleReportUtil logic here as generateReport, modifying it to write to file OR cout based on arg) ...