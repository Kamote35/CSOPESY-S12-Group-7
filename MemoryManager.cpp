#include "MemoryManager.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cmath>
#include <iomanip>

using namespace std;

void MemoryManager::initialize(uint64_t maxMem, uint64_t frameSize) {
    lock_guard<mutex> lock(memLock);
    maxOverallMem = maxMem;
    memPerFrame = frameSize;
    numFrames = 0;

    if (memPerFrame > 0) {
        numFrames = static_cast<int>(maxOverallMem / memPerFrame);
    }

    // Initialize RAM
    ram.clear();
    freeFrames.clear();
    fifoQueue.clear();
    
    for (int i = 0; i < numFrames; ++i) {
        Frame f;
        f.frameID = i;
        f.data.resize(memPerFrame, 0); // Initialize memory with 0
        ram.push_back(f);
        freeFrames.push_back(i);
    }

    // Clear backing store file
    ofstream ofs("csopesy-backing-store.txt", ios::trunc);
    ofs.close();
}

// Check if (PID, Page) is in RAM. Returns FrameID or -1.
int MemoryManager::checkMemory(int pid, int address) {
    lock_guard<mutex> lock(memLock);
    int pageNum = address / memPerFrame;

    // Linear search for now (or could use a map cache for speed)
    for (const auto& frame : ram) {
        if (frame.occupied && frame.processID == pid && frame.pageNum == pageNum) {
            return frame.frameID;
        }
    }
    return -1; // Page Fault
}

void MemoryManager::handlePageFault(int pid, int address) {
    lock_guard<mutex> lock(memLock);
    int pageNum = address / memPerFrame;
    int frameToUse = -1;

    // 1. Check for free frames
    if (!freeFrames.empty()) {
        frameToUse = freeFrames.front();
        freeFrames.pop_front();
    } 
    // 2. Replacement Algorithm (FIFO)
    else {
        if (fifoQueue.empty()) return; // Should not happen if initialized correctly
        
        frameToUse = fifoQueue.front();
        fifoQueue.pop_front();
        
        // Evict current occupant
        Frame& victim = ram[frameToUse];
        writeToBackingStore(victim.processID, victim.pageNum, victim.data);
        pagedOutCount++;
        
        // Log eviction (Optional debug)
        // cout << "Evicted Process " << victim.processID << " Page " << victim.pageNum << "\n";
    }

    // 3. Load new page
    Frame& frame = ram[frameToUse];
    frame.processID = pid;
    frame.pageNum = pageNum;
    frame.occupied = true;
    
    // Try to read from backing store, or init with 0s
    vector<char> swappedData = readFromBackingStore(pid, pageNum);
    if (!swappedData.empty() && swappedData.size() == memPerFrame) {
        frame.data = swappedData;
        pagedInCount++;
    } else {
        // First time access, zero out
        fill(frame.data.begin(), frame.data.end(), 0);
    }

    // Add to FIFO queue
    fifoQueue.push_back(frameToUse);
}

// ---------------------------------------------------------
// Backing Store I/O (Simulated via Text File)
// ---------------------------------------------------------
void MemoryManager::writeToBackingStore(int pid, int pageNum, const vector<char>& data) {
    // We strictly append. Reading will search for the *last* occurrence.
    ofstream file("csopesy-backing-store.txt", ios::app);
    if (!file) return;

    file << "PID:" << pid << " Page:" << pageNum << " Data:";
    for (char c : data) {
        // Hex encode for text safety
        file << hex << setw(2) << setfill('0') << (int)(unsigned char)c;
    }
    file << dec << "\n"; // Newline
}

vector<char> MemoryManager::readFromBackingStore(int pid, int pageNum) {
    ifstream file("csopesy-backing-store.txt");
    if (!file) return {};

    string line, lastMatch;
    string targetPrefix = "PID:" + to_string(pid) + " Page:" + to_string(pageNum) + " Data:";
    
    // Find the LAST matching line (most recent save)
    while (getline(file, line)) {
        if (line.find(targetPrefix) == 0) {
            lastMatch = line;
        }
    }

    if (lastMatch.empty()) return {};

    // Parse Hex Data
    vector<char> data;
    string hexData = lastMatch.substr(targetPrefix.length());
    for (size_t i = 0; i < hexData.length(); i += 2) {
        string byteString = hexData.substr(i, 2);
        char byte = (char)strtol(byteString.c_str(), nullptr, 16);
        data.push_back(byte);
    }
    return data;
}

void MemoryManager::removeFromBackingStore(int pid) {
    // In a real FS, we'd delete files. In this text-file simulation, 
    // we assume "deallocation" just means we stop looking for them.
    // Or we could rewrite the file excluding PID, but that's expensive.
    // For this project, we might leave artifacts or clear file on initialize.
}

// ---------------------------------------------------------
// Helpers
// ---------------------------------------------------------
char MemoryManager::readByte(int frameID, int offset) {
    lock_guard<mutex> lock(memLock);
    if (frameID >= 0 && frameID < numFrames) {
        return ram[frameID].data[offset];
    }
    return 0;
}

void MemoryManager::writeByte(int frameID, int offset, char value) {
    lock_guard<mutex> lock(memLock);
    if (frameID >= 0 && frameID < numFrames) {
        ram[frameID].data[offset] = value;
    }
}

void MemoryManager::allocateProcess(int pid, int memSize) {
    // Just placeholder if we need to track total committed memory
}

void MemoryManager::deallocateProcess(int pid) {
    lock_guard<mutex> lock(memLock);
    
    // 1. Remove from RAM
    for (auto& frame : ram) {
        if (frame.occupied && frame.processID == pid) {
            frame.occupied = false;
            frame.processID = -1;
            frame.pageNum = -1;
            // Return to free pool
            freeFrames.push_front(frame.frameID); // Push front to reuse quickly
            
            // Remove from FIFO Queue
            auto it = find(fifoQueue.begin(), fifoQueue.end(), frame.frameID);
            if (it != fifoQueue.end()) fifoQueue.erase(it);
        }
    }

    // 2. (Optional) Remove from Backing Store
    removeFromBackingStore(pid);
}

size_t MemoryManager::getProcessMemoryUsage(int pid) {
    lock_guard<mutex> lock(memLock);
    size_t count = 0;
    for (const auto& frame : ram) {
        if (frame.occupied && frame.processID == pid) {
            count += memPerFrame;
        }
    }
    return count;
}

size_t MemoryManager::getNumPagedOut() const {
    // To get actual current swap usage, we'd scan the file or maintain a map.
    // pagedOutCount is just a counter of events.
    return pagedOutCount; 
}