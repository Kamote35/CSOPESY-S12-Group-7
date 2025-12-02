#include "ConfigLoader.h"
#include "Global.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>

using namespace std;

// Definition of the global config
SystemConfig g_Config; 
static bool g_configLoaded = false;

static inline void trim(string &s) {
    auto l = s.find_first_not_of(" \t\r\n");
    if (l == string::npos) { s.clear(); return; }
    auto r = s.find_last_not_of(" \t\r\n");
    s = s.substr(l, r - l + 1);
}

void readConfig() {
    if (g_configLoaded) return;

    ifstream file("config.txt");
    if (!file) {
        cout << "Warning: config.txt not found. Using defaults.\n";
        // enforce scheduler to rr
        g_Config.schedulerType = "rr";
        g_configLoaded = true;
        cout << "Configuration (defaults):\n";
        cout << "  num-cpu = " << g_Config.numCPU << "\n";
        cout << "  scheduler = " << g_Config.schedulerType << " \n";
        cout << "  quantum-cycles = " << g_Config.quantumCycles << "\n";
        cout << "  batch-process-freq = " << g_Config.batchFreq << "\n";
        cout << "  min-ins = " << g_Config.minIns << "\n";
        cout << "  max-ins = " << g_Config.maxIns << "\n";
        cout << "  delay-per-exec = " << g_Config.delayPerExec << "\n";
        // New Memory Defaults
        cout << "  max-overall-mem = " << g_Config.maxOverallMem << "\n";
        cout << "  mem-per-frame = " << g_Config.memPerFrame << "\n";
        cout << "  min-mem-per-proc = " << g_Config.minMemPerProc << "\n";
        cout << "  max-mem-per-proc = " << g_Config.maxMemPerProc << "\n";
        cout << "----------------------------------------\n";
        return;
    }

    string line;
    while (getline(file, line)) {
        trim(line);
        if (line.empty() || line[0] == '#') continue;

        istringstream iss(line);
        string key, value;
        iss >> key;
        // read rest of line as value (in case of quoted strings)
        string rest;
        getline(iss, rest);
        trim(rest);
        if (!rest.empty()) value = rest;
        // strip surrounding quotes if present
        if (!value.empty() && value.front() == '"' && value.back() == '"') {
            value = value.substr(1, value.size() - 2);
        }

        if (key.empty()) continue;
        transform(key.begin(), key.end(), key.begin(), ::tolower);
        try {
            if (key == "num-cpu" || key == "numcpu") {
                int v = stoi(value);
                if (v < 1) v = 1;
                if (v > 128) v = 128;
                g_Config.numCPU = v;
            } else if (key == "scheduler") {
                // ignore what's written: always force rr
                (void)value;
                g_Config.schedulerType = "rr";
            } else if (key == "quantum-cycles" || key == "quantumcycles") {
                int v = stoi(value);
                if (v < 1) v = 1;
                g_Config.quantumCycles = v;
            } else if (key == "batch-process-freq" || key == "batchprocessfreq") {
                // large range supported; treat as unsigned 64-bit cycles
                uint64_t v = stoull(value);
                if (v < 1) v = 1;
                g_Config.batchFreq = v;
            } else if (key == "min-ins" || key == "minins") {
                int v = stoi(value);
                if (v < 1) v = 1;
                g_Config.minIns = v;
            } else if (key == "max-ins" || key == "maxins") {
                int v = stoi(value);
                if (v < 1) v = 1;
                g_Config.maxIns = v;
            } else if (key == "delay-per-exec" || key == "delayperexec") {
                uint64_t v = stoull(value);
                g_Config.delayPerExec = v;
            } 
            // ---------------------------------------------------------
            // NEW LOGIC: Parsing Memory Parameters
            // ---------------------------------------------------------
            else if (key == "max-overall-mem") {
                g_Config.maxOverallMem = stoull(value);
            } else if (key == "mem-per-frame") {
                g_Config.memPerFrame = stoull(value);
            } else if (key == "min-mem-per-proc") {
                g_Config.minMemPerProc = stoull(value);
            } else if (key == "max-mem-per-proc") {
                g_Config.maxMemPerProc = stoull(value);
            }

        } catch (...) {
            cout << "Invalid config entry ignored: " << line << "\n";
        }
    }

    if (g_Config.maxIns < g_Config.minIns) swap(g_Config.maxIns, g_Config.minIns);
    if (g_Config.numCPU < 1) g_Config.numCPU = 1;
    if (g_Config.numCPU > 128) g_Config.numCPU = 128;

    // enforce scheduler to rr (test case)
    g_Config.schedulerType = "rr";

    // print loaded configuration from config.txt
    cout << "Configuration loaded:\n";
    cout << "  num-cpu = " << g_Config.numCPU << "\n";
    cout << "  scheduler = " << g_Config.schedulerType << " (forced)\n";
    cout << "  quantum-cycles = " << g_Config.quantumCycles << "\n";
    cout << "  batch-process-freq = " << g_Config.batchFreq << "\n";
    cout << "  min-ins = " << g_Config.minIns << "\n";
    cout << "  max-ins = " << g_Config.maxIns << "\n";
    cout << "  delay-per-exec = " << g_Config.delayPerExec << "\n";
    // Print new memory configs
    cout << "  max-overall-mem = " << g_Config.maxOverallMem << "\n";
    cout << "  mem-per-frame = " << g_Config.memPerFrame << "\n";
    cout << "  min-mem-per-proc = " << g_Config.minMemPerProc << "\n";
    cout << "  max-mem-per-proc = " << g_Config.maxMemPerProc << "\n";
    cout << "----------------------------------------\n";

    g_configLoaded = true;
}