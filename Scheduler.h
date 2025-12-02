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

    void addProcess(const std::string& name, int lines);
    Process* getProcess(const std::string& name);
    
    // Commands
    void startSchedulerLoop(); // scheduler-start
    void stopSchedulerLoop();  // scheduler-stop
    
    // Data Access for Reports
    void generateReport(const std::string& filename = ""); // If empty, print to screen
    std::map<std::string, std::unique_ptr<Process>>& getProcessList(); // Use carefully
    std::mutex& getLock();
};

#endif