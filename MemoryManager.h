#ifndef MEMORY_MANAGER_H
#define MEMORY_MANAGER_H

#include "Global.h"
#include <vector>
#include <mutex>
#include <deque>
#include <map>
#include <string>

// Represents a Physical Frame in RAM
struct Frame {
    int frameID;
    int processID = -1; // -1 if empty
    int pageNum = -1;
    bool occupied = false;
    std::vector<char> data; // Actual simulated memory storage
};

class MemoryManager {
private:
    uint64_t maxOverallMem;
    uint64_t memPerFrame;
    int numFrames;

    std::vector<Frame> ram;        // The Physical Memory
    std::deque<int> freeFrames;    // Queue of empty frame indices
    std::deque<int> fifoQueue;     // For FIFO Page Replacement
    
    // Stats for vmstat
    size_t pagedInCount = 0;
    size_t pagedOutCount = 0;

    std::mutex memLock;

    // Helper: Backing Store I/O
    void writeToBackingStore(int pid, int pageNum, const std::vector<char>& data);
    std::vector<char> readFromBackingStore(int pid, int pageNum);
    void removeFromBackingStore(int pid); // Clean up on exit

public:
    static MemoryManager& getInstance() {
        static MemoryManager instance;
        return instance;
    }

    void initialize(uint64_t maxMem, uint64_t frameSize);

    // Core Paging Logic
    // Returns frame ID if in memory, -1 if Page Fault
    int checkMemory(int pid, int address); 
    
    // Called when checkMemory returns -1. Handles eviction and loading.
    void handlePageFault(int pid, int address); 
    
    // Accessors for Read/Write (Phase 4 usage)
    char readByte(int frameID, int offset);
    void writeByte(int frameID, int offset, char value);

    // Process Management
    void allocateProcess(int pid, int memSize); // Just registers needed space
    void deallocateProcess(int pid); // Cleans up frames and swap

    // Getters for Visualization
    uint64_t getMaxMemory() const { return maxOverallMem; }
    uint64_t getMemPerFrame() const { return memPerFrame; }
    int getNumFrames() const { return numFrames; }
    size_t getPagedIn() const { return pagedInCount; }
    size_t getPagedOut() const { return pagedOutCount; }
    size_t getNumPagedOut() const; // Calculate current swap usage
    
    // Snapshot for process-smi
    size_t getProcessMemoryUsage(int pid);
};

#endif