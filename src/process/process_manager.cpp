/**
 * @file process_manager.cpp
 * @brief Implementation of the process management and scheduling system
 * 
 * This file contains the implementation of the ProcessManager class which
 * handles process lifecycle management and round-robin scheduling.
 */

#include "process_manager.h"
#include <iostream>
#include <algorithm>

namespace TobiOS {

ProcessManager::ProcessManager() : currentRunningPid_(-1), nextPid_(1) {}

ProcessManager::~ProcessManager() {}

int ProcessManager::createProcess(const std::string& name, int priority) {
    // Create a new process with unique PID
    auto process = std::make_unique<Process>();
    process->pid = nextPid_++;           // Assign unique PID and increment counter
    process->name = name;
    process->state = ProcessState::NEW;  // Start in NEW state
    process->priority = priority;
    process->memoryUsage = 0;           // No memory allocated yet
    process->parentPid = currentRunningPid_; // Set parent to currently running process
    
    int pid = process->pid;
    
    // If there's a currently running process, establish parent-child relationship
    if (currentRunningPid_ != -1) {
        auto parent = getProcess(currentRunningPid_);
        if (parent) {
            parent->childPids.push_back(pid);
        }
    }
    
    // Transition from NEW to READY state and add to scheduling queue
    process->state = ProcessState::READY;
    readyQueue_.push(pid);
    
    // Add process to the system process list
    processes_.push_back(std::move(process));
    
    return pid;
}

bool ProcessManager::terminateProcess(int pid) {
    // Find the process with the specified PID
    auto it = std::find_if(processes_.begin(), processes_.end(),
                          [pid](const std::unique_ptr<Process>& p) {
                              return p->pid == pid;
                          });
    
    if (it != processes_.end()) {
        // Mark process as terminated
        (*it)->state = ProcessState::TERMINATED;
        
        // If this was the currently running process, clear the running PID
        if (currentRunningPid_ == pid) {
            currentRunningPid_ = -1;
        }
        
        return true;
    }
    
    // Process not found
    return false;
}

Process* ProcessManager::getProcess(int pid) {
    // Linear search through all processes to find matching PID
    auto it = std::find_if(processes_.begin(), processes_.end(),
                          [pid](const std::unique_ptr<Process>& p) {
                              return p->pid == pid;
                          });
    
    return (it != processes_.end()) ? it->get() : nullptr;
}

std::vector<Process*> ProcessManager::getAllProcesses() {
    // Return only active (non-terminated) processes
    std::vector<Process*> result;
    for (const auto& process : processes_) {
        if (process->state != ProcessState::TERMINATED) {
            result.push_back(process.get());
        }
    }
    return result;
}

void ProcessManager::schedule() {
    // Round-robin scheduling algorithm
    
    if (!readyQueue_.empty()) {
        // If there's a currently running process, move it back to ready queue
        if (currentRunningPid_ != -1) {
            auto currentProcess = getProcess(currentRunningPid_);
            if (currentProcess && currentProcess->state == ProcessState::RUNNING) {
                // Preempt current process and put it back in ready queue
                currentProcess->state = ProcessState::READY;
                readyQueue_.push(currentRunningPid_);
            }
        }
        
        // Get next process from ready queue and schedule it
        currentRunningPid_ = readyQueue_.front();
        readyQueue_.pop();
        
        // Transition the selected process to RUNNING state
        auto process = getProcess(currentRunningPid_);
        if (process) {
            process->state = ProcessState::RUNNING;
        }
    }
}

void ProcessManager::printProcessList() const {
    // Display formatted table of all active processes
    std::cout << "=== Process List ===" << std::endl;
    std::cout << "PID\tName\t\tState\t\tPriority" << std::endl;
    std::cout << "---\t----\t\t-----\t\t--------" << std::endl;
    
    // Iterate through all processes and display non-terminated ones
    for (const auto& process : processes_) {
        if (process->state != ProcessState::TERMINATED) {
            std::cout << process->pid << "\t" << process->name << "\t\t";
            
            // Convert process state enum to string for display
            switch (process->state) {
                case ProcessState::NEW: std::cout << "NEW"; break;
                case ProcessState::READY: std::cout << "READY"; break;
                case ProcessState::RUNNING: std::cout << "RUNNING"; break;
                case ProcessState::BLOCKED: std::cout << "BLOCKED"; break;
                case ProcessState::TERMINATED: std::cout << "TERMINATED"; break;
            }
            
            std::cout << "\t\t" << process->priority << std::endl;
        }
    }
}

}  // namespace TobiOS
