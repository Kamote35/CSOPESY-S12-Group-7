#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <algorithm>
#include <random>
#include <cmath>
#include "Global.h"
#include "ConfigLoader.h"
#include "Scheduler.h"
#include "ConsoleManager.h"
#include "MemoryManager.h" 

using namespace std;

// Helper: Check if integer is power of 2
bool isPowerOfTwo(int n) {
    return (n > 0) && ((n & (n - 1)) == 0);
}

// Helper: Parse custom instructions string "CMD arg; CMD arg"
vector<Instruction> parseCustomInstructions(const string& code) {
    vector<Instruction> instructions;
    stringstream ss(code);
    string segment;
    
    // Split by semicolon
    while (getline(ss, segment, ';')) {
        stringstream ss2(segment);
        string cmd;
        ss2 >> cmd;
        
        if (cmd.empty()) continue;

        Instruction ins;
        // Map string command to Enum
        if (cmd == "PRINT") {
            ins.type = InstrType::PRINT;
            string arg; getline(ss2, arg); 
            // trim quotes and spaces
            size_t first = arg.find_first_not_of(" \"");
            size_t last = arg.find_last_not_of(" \"");
            if (first != string::npos) ins.args.push_back(arg.substr(first, (last-first+1)));
        } else {
            if (cmd == "DECLARE") ins.type = InstrType::DECLARE;
            else if (cmd == "ADD") ins.type = InstrType::ADD;
            else if (cmd == "SUBTRACT") ins.type = InstrType::SUBTRACT;
            else if (cmd == "SLEEP") ins.type = InstrType::SLEEP;
            else if (cmd == "READ") ins.type = InstrType::READ;
            else if (cmd == "WRITE") ins.type = InstrType::WRITE;
            else {
                // If unknown, just skip. 
                continue; 
            }

            // Parse arguments
            string arg;
            while (ss2 >> arg) ins.args.push_back(arg);
        }
        instructions.push_back(ins);
    }
    return instructions;
}

int main() {
    bool initialized = false;
    ConsoleManager::printHeader();
    string line;

    while (true) {
        cout << "root:\\> ";
        if (!getline(cin, line)) break;
        
        // Simple trim logic
        auto l = line.find_first_not_of(" \t");
        if (l == string::npos) continue;
        line = line.substr(l, line.find_last_not_of(" \t") - l + 1);

        if (line == "exit") {
            Scheduler::getInstance().stopSchedulerLoop();
            break;
        }

        if (!initialized) {
            if (line == "initialize") {
                readConfig();
                // Initialize Memory Manager (Phase 1 Logic)
                MemoryManager::getInstance().initialize(g_Config.maxOverallMem, g_Config.memPerFrame);
                initialized = true;
            } else {
                cout << "Unknown command: call 'initialize' first.\n";
            }
            continue;
        }

        // Standard Commands
        if (line == "scheduler-start") { Scheduler::getInstance().startSchedulerLoop(); continue; }
        if (line == "scheduler-stop") { Scheduler::getInstance().stopSchedulerLoop(); continue; }
        if (line == "report-util") { Scheduler::getInstance().generateReport("log.txt"); continue; }
        if (line == "screen -ls") { ConsoleManager::cmdScreenList(); continue; }

        // screen -r <name>
        if (line.rfind("screen -r", 0) == 0) {
            string name = line.substr(9);
            // trim name
            name.erase(0, name.find_first_not_of(" "));
            ConsoleManager::attachToProcess(name);
            continue;
        }

        // ---------------------------------------------------------
        // UPDATED: screen -s <name> <mem>
        // ---------------------------------------------------------
        if (line.rfind("screen -s", 0) == 0) {
            stringstream ss(line);
            string cmd, flag, name; 
            int mem = 0;
            
            // Format: screen -s name mem
            ss >> cmd >> flag >> name >> mem;
            
            if (name.empty() || mem <= 0) {
                cout << "Usage: screen -s <process_name> <memory_size>\n";
                continue;
            }
            if (!isPowerOfTwo(mem)) {
                cout << "Error: Memory must be a power of 2.\n";
                continue;
            }

            // Generate random instructions logic
            random_device rd; mt19937 gen(rd());
            uniform_int_distribution<int> dist(g_Config.minIns, g_Config.maxIns);
            
            // Call the new addProcess overload with memory
            Scheduler::getInstance().addProcess(name, dist(gen), mem);
            ConsoleManager::attachToProcess(name);
            continue;
        }

        // ---------------------------------------------------------
        // NEW: screen -c <name> <mem> "<commands>"
        // ---------------------------------------------------------
        if (line.rfind("screen -c", 0) == 0) {
            // 1. Find the quotes to separate arguments from code
            size_t quoteStart = line.find('"');
            if (quoteStart == string::npos || line.back() != '"') {
                cout << "Usage: screen -c <name> <mem> \"<commands>\"\n";
                continue;
            }

            // 2. Parse arguments before the quotes
            string preArgs = line.substr(0, quoteStart);
            stringstream ss(preArgs);
            string cmd, flag, name; 
            int mem = 0;
            ss >> cmd >> flag >> name >> mem;

            if (name.empty() || mem <= 0) {
                cout << "Invalid arguments.\n";
                continue;
            }
            if (!isPowerOfTwo(mem)) {
                cout << "Error: Memory must be a power of 2.\n";
                continue;
            }
            

            // 3. Parse code inside quotes
            string code = line.substr(quoteStart + 1, line.length() - quoteStart - 2);
            vector<Instruction> ins = parseCustomInstructions(code);
            
            // Call the custom instruction overload
            Scheduler::getInstance().addProcess(name, ins, mem);
            ConsoleManager::attachToProcess(name);
            continue;
        }

        if (line == "mem-test") {
            cout << "Performing Memory Stress Test...\n";
            
            // 1. Calculate how many frames we have
            int totalFrames = MemoryManager::getInstance().getNumFrames();
            int frameSize = MemoryManager::getInstance().getMemPerFrame();
            
            cout << "Total Frames Available: " << totalFrames << "\n";
            
            // 2. Allocate a dummy process
            int pid = 9999;
            MemoryManager::getInstance().allocateProcess(pid, totalFrames * frameSize + frameSize); // Request more than RAM
            
            // 3. Fill up ALL RAM frames
            for (int i = 0; i < totalFrames; ++i) {
                int address = i * frameSize;
                cout << "Accessing Page " << i << " (Address " << address << ")... ";
                
                // This should load pages into free frames
                int frameID = MemoryManager::getInstance().checkMemory(pid, address);
                if (frameID == -1) {
                    MemoryManager::getInstance().handlePageFault(pid, address);
                    cout << "Page Fault Handled.\n";
                } else {
                    cout << "Already in memory.\n";
                }
            }
            
            cout << "RAM should now be FULL.\n";
            
            // 4. Force ONE more page access (Overflow)
            // This MUST trigger an eviction (FIFO)
            int overflowAddr = totalFrames * frameSize;
            cout << "Accessing Overflow Page (Address " << overflowAddr << ")... ";
            
            if (MemoryManager::getInstance().checkMemory(pid, overflowAddr) == -1) {
                MemoryManager::getInstance().handlePageFault(pid, overflowAddr);
                cout << "Page Fault Handled (Swap should have occurred).\n";
            }
            
            // 5. Check Stats
            cout << "\n--- STATS ---\n";
            cout << "Paged In: " << MemoryManager::getInstance().getPagedIn() << "\n";
            cout << "Paged Out (Swap): " << MemoryManager::getInstance().getPagedOut() << "\n";
            
            if (MemoryManager::getInstance().getPagedOut() > 0) {
                cout << "[SUCCESS] Page replacement logic is working!\n";
            } else {
                cout << "[FAIL] No pages were swapped out.\n";
            }
            continue;
        }
        if (line == "vmstat") {
            ConsoleManager::cmdVmstat(); continue; 
        }

        cout << "Unknown command.\n";
    }

    return 0;
}