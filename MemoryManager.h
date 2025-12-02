#ifndef MEMORY_MANAGER_H
#define MEMORY_MANAGER_H

#include "Global.h"
#include <vector>
#include <mutex>

class MemoryManager {
private:
    uint64_t maxOverallMem;
    uint64_t memPerFrame;
    int numFrames;

public:
    static MemoryManager& getInstance() {
        static MemoryManager instance;
        return instance;
    }

    // Initialize with config values
    void initialize(uint64_t maxMem, uint64_t frameSize);

    // Getters for visualization
    uint64_t getMaxMemory() const { return maxOverallMem; }
    uint64_t getMemPerFrame() const { return memPerFrame; }
    int getNumFrames() const { return numFrames; }
};

#endif