#include "Process.h"
#include <iostream>
#include <chrono>
#include <ctime>
#include <iomanip>

using namespace std;

Process::Process(const string &n, int pid, int lines)
    : name(n), id(pid), finished(false), coreAssigned(0), currentInstrIndex(0), gen(rd()) {
    
    // Core assignment logic
    int numcpu = max(1, g_Config.numCPU);
    coreAssigned = ( (pid - 1) % numcpu );
    
    instructions = generateRandomInstructions(lines);
    totalLines = static_cast<int>(instructions.size());
}

Process::~Process() { join(); }

void Process::join() {
    if (worker.joinable()) worker.join();
}

void Process::start() {
    worker = thread([this]() {
        for (size_t i = 0; i < instructions.size(); ++i) {
            currentInstrIndex.store(static_cast<int>(i) + 1);
            execInstruction(instructions[i]);
        }
        currentInstrIndex.store(static_cast<int>(instructions.size()));
        lock_guard<mutex> lg(logLock);
        finished = true;
        logs.push_back("Process finished!");
    });
}

// ... (Copy execInstruction, log, generateRandomInstructions, and Getters from prototype here) ...
// Ensure you use g_Config.delayPerExec inside execInstruction