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
    std::map<std::string, uint16_t> vars; 
    std::mutex logLock;
    std::vector<Instruction> instructions;
    
    int coreAssigned;
    std::atomic<int> currentInstrIndex;
    int totalLines;
    
    // Memory Info
    int memoryRequired;
    int currentMemoryUsage; // For tracking actual usage later
    
    std::random_device rd;
    std::mt19937 gen;

    void log(const std::string &msg);
    void execInstruction(const Instruction &ins);
    std::vector<Instruction> generateRandomInstructions(int count, int depth = 0);

public:
    // Constructor for Random Instructions (screen -s / scheduler-start)
    Process(const std::string &n, int pid, int lines, int memReq);
    
    // Constructor for Custom Instructions (screen -c)
    Process(const std::string &n, int pid, const std::vector<Instruction>& ins, int memReq);
    
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
    int getMemoryRequired() const;
    std::vector<std::string> snapshotLogs();
};

#endif