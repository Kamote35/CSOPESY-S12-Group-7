#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "Process.h"
#include <map>
#include <memory>
#include <mutex>
#include <string>

class Scheduler {
private:
    std::map<std::string, std::unique_ptr<Process>> procList;
    std::mutex processLock;
    int next_pid = 1;
    bool schedulerRunning = false;

public:
    static Scheduler& getInstance() {
        static Scheduler instance;
        return instance;
    }

    // Existing: Add process with random instructions + memory
    void addProcess(const std::string& name, int lines, int memory);
    
    // New: Add process with custom instructions + memory (for screen -c)
    void addProcess(const std::string& name, const std::vector<Instruction>& ins, int memory);
    
    Process* getProcess(const std::string& name);
    
    void startSchedulerLoop(); 
    void stopSchedulerLoop();  
    
    void generateReport(const std::string& filename = ""); 
    std::map<std::string, std::unique_ptr<Process>>& getProcessList(); 
    std::mutex& getLock();
};

#endif