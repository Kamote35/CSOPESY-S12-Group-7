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

void Process::log(const string &msg) {
    lock_guard<mutex> lg(logLock);
    time_t now = time(nullptr);
    tm local_tm = *localtime(&now);
    char buf[64];
    strftime(buf, sizeof(buf), "(%m/%d/%Y %I:%M:%S%p)", &local_tm);
    string entry = string(buf) + " Core:" + to_string(coreAssigned) + " \"" + msg + "\"";
    logs.push_back(entry);
}

void Process::execInstruction(const Instruction &ins) {
    if (g_Config.delayPerExec > 0) {
            this_thread::sleep_for(chrono::milliseconds(static_cast<int>(g_Config.delayPerExec)));
        }

        switch (ins.type) {
        case InstrType::PRINT:
            if (!ins.args.empty()) {
                string msg = ins.args[0];
                // Check for variable printing syntax
                if (msg.size() > 1 && msg[0] == '+') {
                    string var = msg.substr(1);
                    if (vars.count(var))
                        msg = var + " = " + to_string(vars[var]);
                }
                log("PRINT: " + msg);
            }
            break;
        case InstrType::DECLARE:
            if (ins.args.size() >= 2) {
                string var = ins.args[0];
                uint16_t val = static_cast<uint16_t>(stoi(ins.args[1]));
                vars[var] = val;
                log("DECLARE: " + var + " = " + to_string(val));
            }
            break;
        case InstrType::ADD:
            if (ins.args.size() >= 3) {
                string v1 = ins.args[0], v2 = ins.args[1], v3 = ins.args[2];
                uint16_t a = vars.count(v2) ? vars[v2] : static_cast<uint16_t>(stoi(v2));
                uint16_t b = vars.count(v3) ? vars[v3] : static_cast<uint16_t>(stoi(v3));
                vars[v1] = a + b;
                log("ADD: " + v1 + " = " + to_string(a) + " + " + to_string(b));
            }
            break;
        case InstrType::SUBTRACT:
            if (ins.args.size() >= 3) {
                string v1 = ins.args[0], v2 = ins.args[1], v3 = ins.args[2];
                uint16_t a = vars.count(v2) ? vars[v2] : static_cast<uint16_t>(stoi(v2));
                uint16_t b = vars.count(v3) ? vars[v3] : static_cast<uint16_t>(stoi(v3));
                vars[v1] = a - b;
                log("SUBTRACT: " + v1 + " = " + to_string(a) + " - " + to_string(b));
            }
            break;
        case InstrType::SLEEP:
            if (!ins.args.empty()) {
                int ticks = stoi(ins.args[0]);
                log("SLEEP for " + to_string(ticks) + " ticks");
                this_thread::sleep_for(chrono::milliseconds(ticks * 10)); // 1 tick = 10ms sim
            }
            break;
        case InstrType::FOR:
            for (int r = 0; r < ins.repeatCount; ++r)
                for (auto &nestedIns : ins.nested)
                    execInstruction(nestedIns);
        break;
    }
}

vector<Instruction> Process::generateRandomInstructions(int count, int depth) {
    vector<Instruction> list;
    // Using the generator initialized in constructor
    uniform_int_distribution<int> typeDist(0, 4);
    uniform_int_distribution<int> sleepDist(1, 5);
    uniform_int_distribution<int> valueDist(0, 50);
    uniform_int_distribution<int> repeatDist(1, 3);

    for (int i = 0; i < count; ++i) {
        int type = typeDist(gen);
        Instruction ins;
        switch (type) {
        case 0: ins.type = InstrType::PRINT; ins.args = {"Hello world from " + name + "!"}; break;
        case 1: ins.type = InstrType::DECLARE; ins.args = {"x" + to_string(i), to_string(valueDist(gen))}; break;
        case 2: ins.type = InstrType::ADD; ins.args = {"x" + to_string(i), to_string(valueDist(gen)), to_string(valueDist(gen))}; break;
        case 3: ins.type = InstrType::SUBTRACT; ins.args = {"x" + to_string(i), to_string(valueDist(gen)), to_string(valueDist(gen))}; break;
        case 4: ins.type = InstrType::SLEEP; ins.args = {to_string(sleepDist(gen))}; break;
        }

        if (depth < 3 && uniform_int_distribution<int>(0, 9)(gen) == 0) {
            Instruction loop;
            loop.type = InstrType::FOR;
            loop.repeatCount = repeatDist(gen);
            loop.nested = generateRandomInstructions(3, depth + 1);
            list.push_back(loop);
        } else list.push_back(ins);
    }
    return list;
}

// Getters
bool Process::isFinished() const {
    return finished;
}

const string& Process::getName() const {
    return name;
}

int Process::getId() const {
    return id;
}

int Process::getCoreAssigned() const {
    return coreAssigned;
}

int Process::getCurrentInstructionLine() const {
    return currentInstrIndex.load();
}

int Process::getTotalLines() const {
    return totalLines;
}

vector<string> Process::snapshotLogs() {
    lock_guard<mutex> lg(logLock);
    return logs;
}