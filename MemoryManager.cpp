#include "MemoryManager.h"
#include <iostream>

void MemoryManager::initialize(uint64_t maxMem, uint64_t frameSize) {
    maxOverallMem = maxMem;
    memPerFrame = frameSize;
    numFrames = 0;
    
    if (memPerFrame > 0) {
        numFrames = static_cast<int>(maxOverallMem / memPerFrame);
    }
    
    // std::cout << "MemoryManager Initialized: " << maxOverallMem << " bytes, " 
    //           << numFrames << " frames.\n";
}