/**
 * @file process_manager.h
 * @brief Process management and scheduling system for TobiOS
 * 
 * This file defines the ProcessManager class which handles process creation,
 * scheduling, and lifecycle management using round-robin scheduling.
 */

#pragma once

#include <string>
#include <vector>
#include <queue>
#include <memory>

namespace TobiOS {

/**
 * @enum ProcessState
 * @brief Enumeration of possible process states in the system
 * 
 * These states represent the standard process lifecycle states
 * found in most operating systems.
 */
enum class ProcessState {
    NEW,        ///< Process has been created but not yet scheduled
    READY,      ///< Process is ready to run and waiting for CPU time
    RUNNING,    ///< Process is currently executing on the CPU
    BLOCKED,    ///< Process is waiting for I/O or other resources
    TERMINATED  ///< Process has finished execution and is being cleaned up
};

/**
 * @struct Process
 * @brief Process Control Block (PCB) containing all process information
 * 
 * This structure contains all the information the OS needs to manage
 * a process throughout its lifetime, similar to a real OS PCB.
 */
struct Process {
    int pid;                        ///< Unique process identifier
    std::string name;               ///< Human-readable process name
    ProcessState state;             ///< Current state of the process
    int priority;                   ///< Process priority for scheduling (lower = higher priority)
    size_t memoryUsage;             ///< Amount of memory used by this process
    int parentPid;                  ///< PID of the parent process (-1 if no parent)
    std::vector<int> childPids;     ///< List of child process PIDs
};

/**
 * @class ProcessManager
 * @brief Process scheduling and lifecycle management system
 * 
 * The ProcessManager handles all aspects of process management including:
 * - Process creation and termination
 * - Round-robin scheduling algorithm
 * - Process state transitions
 * - Parent-child process relationships
 * - Process information queries and reporting
 * 
 * The scheduler uses a simple round-robin algorithm where each process
 * gets equal CPU time slices in rotation.
 */
class ProcessManager {
public:
    /**
     * @brief Constructor - initializes the process management system
     */
    ProcessManager();
    
    /**
     * @brief Destructor - cleans up all process resources
     */
    ~ProcessManager();

    /**
     * @brief Create a new process and add it to the system
     * @param name Human-readable name for the process
     * @param priority Process priority (lower numbers = higher priority)
     * @return PID of the newly created process
     */
    int createProcess(const std::string& name, int priority = 0);
    
    /**
     * @brief Terminate a process and remove it from scheduling
     * @param pid Process ID to terminate
     * @return true if process was found and terminated, false otherwise
     */
    bool terminateProcess(int pid);
    
    /**
     * @brief Get a pointer to a specific process by PID
     * @param pid Process ID to look up
     * @return Pointer to Process structure, or nullptr if not found
     */
    Process* getProcess(int pid);
    
    /**
     * @brief Get a list of all active (non-terminated) processes
     * @return Vector of pointers to all active Process structures
     */
    std::vector<Process*> getAllProcesses();
    
    /**
     * @brief Perform one round of process scheduling
     * 
     * Implements round-robin scheduling by moving the current running
     * process to the back of the ready queue and selecting the next
     * process to run.
     */
    void schedule();
    
    /**
     * @brief Print a formatted list of all processes and their states
     * 
     * Displays a table showing PID, name, state, and priority for
     * all active processes in the system.
     */
    void printProcessList() const;

private:
    std::vector<std::unique_ptr<Process>> processes_;  ///< All processes in the system
    std::queue<int> readyQueue_;                       ///< Queue of PIDs ready to run
    int currentRunningPid_;                            ///< PID of currently running process (-1 if none)
    int nextPid_;                                      ///< Next PID to assign to new processes
};

}