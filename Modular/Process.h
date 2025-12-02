#ifndef PROCESS_H
#define PROCESS_H

#include "Global.h"
#include <string>
#include <vector>
#include <map>
#include <mutex>
#include <thread>
#include <atomic>
#include <random>

class Process {
private:
    std::string name;
    int id;
    bool finished;
    std::thread worker;
    std::vector<std::string> logs;
    std::map<std::string, uint16_t> vars; // Stores uint16 variables [cite: 88]
    std::mutex logLock;
    std::vector<Instruction> instructions;
    
    int coreAssigned;
    std::atomic<int> currentInstrIndex;
    int totalLines;
    
    std::random_device rd;
    std::mt19937 gen;

    void log(const std::string &msg);
    void execInstruction(const Instruction &ins);
    std::vector<Instruction> generateRandomInstructions(int count, int depth = 0);

public:
    Process(const std::string &n, int pid, int lines);
    ~Process();

    void start();
    void join();
    bool isFinished() const;
    
    // Getters
    const std::string& getName() const;
    int getId() const;
    int getCoreAssigned() const;
    int getCurrentInstructionLine() const;
    int getTotalLines() const;
    std::vector<std::string> snapshotLogs();
};

#endif