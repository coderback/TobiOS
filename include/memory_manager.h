/**
 * @file memory_manager.h
 * @brief Virtual memory management system for TobiOS
 * 
 * This file defines the MemoryManager class which simulates virtual memory
 * management using a first-fit allocation algorithm with block merging.
 */

#pragma once

#include <cstddef>
#include <vector>
#include <unordered_map>

namespace TobiOS {

/**
 * @struct MemoryBlock
 * @brief Represents a block of memory in the virtual memory system
 * 
 * Each memory block tracks its address, size, allocation status,
 * and which process owns it (if allocated).
 */
struct MemoryBlock {
    void* address;      ///< Starting address of the memory block
    size_t size;        ///< Size of the block in bytes
    bool allocated;     ///< Whether this block is currently allocated
    int processId;      ///< ID of the process that owns this block (-1 if free)
};

/**
 * @class MemoryManager
 * @brief Virtual memory management system with first-fit allocation
 * 
 * The MemoryManager simulates an operating system's memory management unit.
 * It provides:
 * - Dynamic memory allocation and deallocation
 * - First-fit allocation algorithm
 * - Memory fragmentation handling with automatic block merging
 * - Process-based memory tracking
 * - Memory usage statistics and visualization
 * 
 * The system allocates a large contiguous block of memory at initialization
 * and manages it through a list of MemoryBlock structures.
 */
class MemoryManager {
public:
    /**
     * @brief Constructor - initializes virtual memory system
     * @param totalMemory Total size of virtual memory to manage (default: 1MB)
     * @throws std::runtime_error if memory allocation fails
     */
    MemoryManager(size_t totalMemory = 1024 * 1024);
    
    /**
     * @brief Destructor - frees the underlying memory pool
     */
    ~MemoryManager();

    /**
     * @brief Allocate a block of memory using first-fit algorithm
     * @param size Number of bytes to allocate
     * @param processId ID of the requesting process (optional, default: -1)
     * @return Pointer to allocated memory, or nullptr if allocation fails
     */
    void* allocate(size_t size, int processId = -1);
    
    /**
     * @brief Deallocate a previously allocated memory block
     * @param ptr Pointer to the memory block to free
     * @return true if deallocation was successful, false if pointer not found
     */
    bool deallocate(void* ptr);
    
    /**
     * @brief Get the total amount of free memory available
     * @return Number of free bytes across all unallocated blocks
     */
    size_t getAvailableMemory() const;
    
    /**
     * @brief Get the total size of the virtual memory pool
     * @return Total memory size in bytes
     */
    size_t getTotalMemory() const;
    
    /**
     * @brief Print a detailed memory map showing all blocks
     * Displays block-by-block breakdown with allocation status and process IDs
     */
    void printMemoryMap() const;

private:
    void* memory_;                      ///< Pointer to the main memory pool
    size_t totalSize_;                  ///< Total size of the memory pool
    std::vector<MemoryBlock> blocks_;   ///< List of all memory blocks
    
    /**
     * @brief Merge adjacent free blocks to reduce fragmentation
     * 
     * This function scans through the block list and combines consecutive
     * free blocks into larger blocks to reduce memory fragmentation.
     */
    void mergeAdjacentBlocks();
};

}