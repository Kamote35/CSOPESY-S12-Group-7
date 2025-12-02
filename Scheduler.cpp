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

map<string, unique_ptr<Process>>& Scheduler::getProcessList() {
    return procList;
}

mutex& Scheduler::getLock() {
    return processLock;
}

void Scheduler::generateReport(const string& filename) {
    lock_guard<mutex> lock(processLock);
    
    // Output stream: either to file or cout (if filename empty)
    // For this assignment, we mostly write to file.
    ofstream outFile;
    if (!filename.empty()) {
        outFile.open(filename, ios::out | ios::trunc);
        if (!outFile.is_open()) {
            cout << "Error: Unable to write to " << filename << "\n";
            return;
        }
    }
    
    ostream& out = filename.empty() ? cout : outFile;

    // Logic adapted from handleReportUtil
    if (procList.empty()) {
        out << "CPU utilization: 0%\n";
        out << "Cores used: 0\n";
        out << "Cores available: " << g_Config.numCPU << "\n";
        out << "------------------------------------\n";
        out << "(no processes)\n";
        return;
    }

    int total = (int)procList.size();
    int finished = 0;
    for (auto &kv : procList)
        if (kv.second->isFinished()) finished++;

    int running = total - finished;
    int usedCores = min(running, g_Config.numCPU);
    int available = max(0, g_Config.numCPU - usedCores);

    out << "CPU utilization: " << fixed << setprecision(1)
        << (100.0 * usedCores / g_Config.numCPU) << "%\n";
    out << "Cores used: " << usedCores << "\n";
    out << "Cores available: " << available << "\n";
    out << "------------------------------------\n";

    if (running > 0) {
        out << "Running processes:\n";
        for (auto &kv : procList) {
            auto &p = kv.second;
            if (!p->isFinished()) {
                // Formatting time
                time_t now = time(nullptr);
                tm local_tm = *localtime(&now);
                char buf[64];
                strftime(buf, sizeof(buf), "(%m/%d/%Y %I:%M:%S%p)", &local_tm);
                
                out << left << setw(10) << p->getName()
                    << " " << buf
                    << "   Core: " << p->getCoreAssigned()
                    << "   " << p->getCurrentInstructionLine() << " / " << p->getTotalLines() << "\n";
            }
        }
    } else {
        out << "(no running processes)\n";
    }

    out << "\nFinished processes:\n";
    for (auto &kv : procList) {
        auto &p = kv.second;
        if (p->isFinished()) {
            time_t now = time(nullptr);
            tm local_tm = *localtime(&now);
            char buf[64];
            strftime(buf, sizeof(buf), "(%m/%d/%Y %I:%M:%S%p)", &local_tm);
            
            out << left << setw(10) << p->getName()
                << " " << buf
                << "   Finished   " << p->getTotalLines() << " / " << p->getTotalLines() << "\n";
        }
    }

    out << "------------------------------------\n";
}