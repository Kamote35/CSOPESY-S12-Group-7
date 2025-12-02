#include "ConfigLoader.h"
#include "Global.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>

using namespace std;

SystemConfig g_Config; // Definition of the global config

static inline void trim(string &s) {
    auto l = s.find_first_not_of(" \t\r\n");
    if (l == string::npos) { s.clear(); return; }
    auto r = s.find_last_not_of(" \t\r\n");
    s = s.substr(l, r - l + 1);
}

void loadConfiguration() {
    ifstream file("config.txt");
    if (!file) {
        cout << "Warning: config.txt not found. Using defaults.\n";
        return;
    }

    string line;
    while (getline(file, line)) {
        trim(line);
        if (line.empty() || line[0] == '#') continue;

        istringstream iss(line);
        string key, value;
        iss >> key;
        string rest;
        getline(iss, rest);
        trim(rest);
        if (!rest.empty()) value = rest;
        if (!value.empty() && value.front() == '"' && value.back() == '"') {
            value = value.substr(1, value.size() - 2);
        }

        if (key.empty()) continue;
        transform(key.begin(), key.end(), key.begin(), ::tolower);

        try {
            if (key == "num-cpu") g_Config.numCPU = stoi(value);
            else if (key == "scheduler") g_Config.schedulerType = value; // "rr" or "fcfs"
            else if (key == "quantum-cycles") g_Config.quantumCycles = stoi(value);
            else if (key == "batch-process-freq") g_Config.batchFreq = stoull(value);
            else if (key == "min-ins") g_Config.minIns = stoi(value);
            else if (key == "max-ins") g_Config.maxIns = stoi(value);
            else if (key == "delay-per-exec") g_Config.delayPerExec = stoull(value);
        } catch (...) {
            cout << "Invalid config entry ignored: " << line << "\n";
        }
    }

    // Validation
    if (g_Config.numCPU < 1) g_Config.numCPU = 1;
    if (g_Config.numCPU > 128) g_Config.numCPU = 128;
    
    cout << "Configuration loaded from config.txt\n";
}