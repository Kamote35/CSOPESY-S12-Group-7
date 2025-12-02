#include <iostream>
#include <string>
#include <algorithm>
#include <random>
#include "Global.h"
#include "ConfigLoader.h"
#include "Scheduler.h"
#include "ConsoleManager.h"

using namespace std;

int main() {
    bool initialized = false;
    ConsoleManager::printHeader();
    string line;

    while (true) {
        cout << "root:\\> ";
        if (!getline(cin, line)) break;
        
        // simple trim
        auto l = line.find_first_not_of(" \t");
        if (l == string::npos) continue;
        line = line.substr(l, line.find_last_not_of(" \t") - l + 1);

        if (line == "exit") {
            Scheduler::getInstance().stopSchedulerLoop();
            // Optional: Join all processes before quitting
            break;
        }

        if (!initialized) {
            if (line == "initialize") {
                readConfig(); // Updated function name
                initialized = true;
                // cout << "Processor initialized.\n"; // Removed to avoid double printing since readConfig now prints
            } else {
                cout << "Unknown command: call 'initialize' first.\n";
            }
            continue;
        }

        if (line == "scheduler-start") {
            cout << "Starting scheduler...\n";
            Scheduler::getInstance().startSchedulerLoop();
            continue; 
        }
        if (line == "scheduler-stop") {
            cout << "Stopping scheduler...\n";
            Scheduler::getInstance().stopSchedulerLoop();
            continue; 
        }
        if (line == "report-util") {
            // Generates report to csopesy-log.txt
            Scheduler::getInstance().generateReport("csopesy-log.txt");
            cout << "Report generated at csopesy-log.txt\n";
            continue; 
        }
        if (line == "screen -ls") {
            ConsoleManager::cmdScreenList(); 
            continue; 
        }

        if (line.rfind("screen -r", 0) == 0) {
            string name = line.substr(9);
            // trim name
            name.erase(0, name.find_first_not_of(" "));
            ConsoleManager::attachToProcess(name);
            continue;
        }

        if (line.rfind("screen -s", 0) == 0) {
            string name = line.substr(9);
            // trim name
            name.erase(0, name.find_first_not_of(" "));
            
            // Generate random instructions logic for this specific process
            random_device rd; mt19937 gen(rd());
            uniform_int_distribution<int> dist(g_Config.minIns, g_Config.maxIns);
            
            Scheduler::getInstance().addProcess(name, dist(gen));
            ConsoleManager::attachToProcess(name);
            continue;
        }

        cout << "Unknown command.\n";
    }

    return 0;
}