#ifndef GLOBAL_H
#define GLOBAL_H

#include <string>
#include <vector>
#include <cstdint>

// Instruction Types as per spec 
enum class InstrType { PRINT, DECLARE, ADD, SUBTRACT, SLEEP, FOR };

struct Instruction {
    InstrType type;
    std::vector<std::string> args;
    std::vector<Instruction> nested; // For nested loops
    int repeatCount = 0;
};

// Global Configuration object
struct SystemConfig {
    int numCPU = 4;
    std::string schedulerType = "rr";
    int quantumCycles = 5;
    uint64_t batchFreq = 1;
    int minIns = 100;
    int maxIns = 1000;
    uint64_t delayPerExec = 0;
};

extern SystemConfig g_Config; // Global instance declaration

#endif