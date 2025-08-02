/**
 * @file memory_manager.cpp
 * @brief Implementation of the virtual memory management system
 * 
 * This file contains the implementation of the MemoryManager class which
 * provides first-fit memory allocation with automatic defragmentation.
 */

#include "memory_manager.h"
#include <iostream>
#include <cstdlib>
#include <algorithm>

namespace TobiOS {

MemoryManager::MemoryManager(size_t totalMemory) : totalSize_(totalMemory) {
    // Allocate the main memory pool using malloc
    memory_ = std::malloc(totalSize_);
    if (!memory_) {
        throw std::runtime_error("Failed to allocate memory for MemoryManager");
    }
    
    // Initialize with one large free block covering the entire memory space
    MemoryBlock initialBlock;
    initialBlock.address = memory_;
    initialBlock.size = totalSize_;
    initialBlock.allocated = false;
    initialBlock.processId = -1;
    
    blocks_.push_back(initialBlock);
}

MemoryManager::~MemoryManager() {
    // Free the main memory pool
    if (memory_) {
        std::free(memory_);
    }
}

void* MemoryManager::allocate(size_t size, int processId) {
    // First-fit allocation algorithm: find the first free block large enough
    for (auto& block : blocks_) {
        if (!block.allocated && block.size >= size) {
            // If block is larger than needed, split it into two blocks
            if (block.size > size) {
                // Create a new free block for the remaining space
                MemoryBlock newBlock;
                newBlock.address = static_cast<char*>(block.address) + size;
                newBlock.size = block.size - size;
                newBlock.allocated = false;
                newBlock.processId = -1;
                blocks_.push_back(newBlock);
            }
            
            // Mark the current block as allocated and assign to process
            block.size = size;
            block.allocated = true;
            block.processId = processId;
            
            return block.address;
        }
    }
    
    // No suitable block found - allocation failed
    return nullptr;
}

bool MemoryManager::deallocate(void* ptr) {
    // Find the block with the matching address and mark it as free
    for (auto& block : blocks_) {
        if (block.address == ptr && block.allocated) {
            block.allocated = false;
            block.processId = -1;
            
            // Attempt to merge adjacent free blocks to reduce fragmentation
            mergeAdjacentBlocks();
            return true;
        }
    }
    
    // Pointer not found in allocated blocks
    return false;
}

size_t MemoryManager::getAvailableMemory() const {
    // Sum up all free (unallocated) blocks
    size_t available = 0;
    for (const auto& block : blocks_) {
        if (!block.allocated) {
            available += block.size;
        }
    }
    return available;
}

size_t MemoryManager::getTotalMemory() const {
    return totalSize_;
}

void MemoryManager::printMemoryMap() const {
    // Display comprehensive memory usage information
    std::cout << "=== Memory Map ===" << std::endl;
    std::cout << "Total Memory: " << totalSize_ << " bytes" << std::endl;
    std::cout << "Available Memory: " << getAvailableMemory() << " bytes" << std::endl;
    std::cout << "Used Memory: " << (totalSize_ - getAvailableMemory()) << " bytes" << std::endl;
    std::cout << "Blocks:" << std::endl;
    
    // Display each memory block with its status
    for (size_t i = 0; i < blocks_.size(); ++i) {
        const auto& block = blocks_[i];
        std::cout << "  Block " << i << ": " << block.size << " bytes, ";
        std::cout << (block.allocated ? "ALLOCATED" : "FREE");
        
        // Show process ID for allocated blocks
        if (block.allocated && block.processId != -1) {
            std::cout << " (PID: " << block.processId << ")";
        }
        std::cout << std::endl;
    }
}

void MemoryManager::mergeAdjacentBlocks() {
    // Merge consecutive free blocks to reduce memory fragmentation
    // This is essential for preventing the memory from becoming too fragmented
    
    for (size_t i = 0; i < blocks_.size(); ++i) {
        // Only consider free blocks for merging
        if (!blocks_[i].allocated) {
            // Look for adjacent free blocks that can be merged
            for (size_t j = i + 1; j < blocks_.size(); ++j) {
                // Check if blocks are adjacent and both free
                if (!blocks_[j].allocated && 
                    static_cast<char*>(blocks_[i].address) + blocks_[i].size == blocks_[j].address) {
                    
                    // Merge block j into block i
                    blocks_[i].size += blocks_[j].size;
                    blocks_.erase(blocks_.begin() + j);
                    --j; // Adjust index since we removed an element
                }
            }
        }
    }
}