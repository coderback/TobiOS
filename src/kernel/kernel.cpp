/**
 * @file kernel.cpp
 * @brief Implementation of the TobiOS kernel class
 * 
 * This file contains the implementation of the Kernel class which manages
 * the entire operating system lifecycle including boot, run, and shutdown
 * sequences. The kernel coordinates all major subsystems.
 */

#include "kernel.h"
#include "logger.h"
#include <iostream>

namespace TobiOS {

// Static member initialization - ensures singleton pattern
Kernel* Kernel::instance_ = nullptr;

Kernel::Kernel() : isRunning_(false) {
    // Enforce singleton pattern - only one kernel instance allowed
    if (instance_ != nullptr) {
        throw std::runtime_error("Kernel instance already exists!");
    }
    instance_ = this;
}

Kernel::~Kernel() {
    // Ensure clean shutdown if kernel is still running
    if (isRunning_) {
        shutdown();
    }
    // Clear singleton instance pointer
    instance_ = nullptr;
}

bool Kernel::boot() {
    // Display boot message and TobiOS ASCII art
    printBootMessage();
    
    // Initialize logger first so we can log the boot process
    logger_ = std::make_unique<Logger>();
    logger_->setLogLevel(LogLevel::INFO);
    logger_->info("TobiOS Kernel starting...");
    
    // Initialize all OS subsystems
    if (!initializeSubsystems()) {
        logger_->critical("Failed to initialize subsystems");
        return false;
    }
    
    // Mark kernel as running
    isRunning_ = true;
    logger_->info("TobiOS boot sequence completed successfully");
    return true;
}

void Kernel::run() {
    // Ensure kernel has been booted before running
    if (!isRunning_) {
        std::cerr << "Kernel not running. Call boot() first." << std::endl;
        return;
    }
    
    logger_->info("Starting TobiOS main loop");
    
    std::cout << "\n=== TobiOS Ready ===" << std::endl;
    
    // Start the interactive shell - this is the main user interface
    // The shell will run until the user exits
    shell_->run();
    
    // Once shell exits, shutdown the system
    shutdown();
}

void Kernel::shutdown() {
    // Prevent multiple shutdown calls
    if (!isRunning_) {
        return;
    }
    
    logger_->info("TobiOS shutdown initiated");
    
    // Shutdown subsystems in reverse order of initialization
    // This ensures proper cleanup and dependency resolution
    shell_.reset();        // Shutdown shell first (user interface)
    fileSystem_.reset();   // Then filesystem
    processManager_.reset();// Then process manager
    memoryManager_.reset(); // Finally memory manager
    
    // Mark kernel as not running
    isRunning_ = false;
    logger_->info("TobiOS shutdown completed");
    
    // Logger is shutdown last so we can log the entire shutdown process
    logger_.reset();
    
    std::cout << "TobiOS has been shut down. Goodbye!" << std::endl;
}

Kernel& Kernel::getInstance() {
    // Return singleton instance, throw error if not created
    if (instance_ == nullptr) {
        throw std::runtime_error("Kernel instance not created!");
    }
    return *instance_;
}

bool Kernel::initializeSubsystems() {
    logger_->info("Initializing subsystems...");
    
    try {
        // Initialize Memory Manager with 1MB of virtual memory
        logger_->info("Initializing Memory Manager...");
        memoryManager_ = std::make_unique<MemoryManager>(1024 * 1024); // 1MB
        
        // Initialize Process Manager for process scheduling and management
        logger_->info("Initializing Process Manager...");
        processManager_ = std::make_unique<ProcessManager>();
        
        // Initialize Virtual File System
        logger_->info("Initializing File System...");
        fileSystem_ = std::make_unique<FileSystem>();
        
        // Create initial directory structure similar to UNIX systems
        fileSystem_->createDirectory("/home");  // User home directories
        fileSystem_->createDirectory("/tmp");   // Temporary files
        fileSystem_->createDirectory("/bin");   // Binary/executable files
        
        // Create a welcome file for demonstration
        fileSystem_->createFile("/home/welcome.txt", "Welcome to TobiOS!\nThis is a simulated operating system.");
        
        // Initialize Interactive Shell with reference to this kernel
        logger_->info("Initializing Shell...");
        shell_ = std::make_unique<Shell>(this);
        
        // Create the initial system process (similar to init process in real OS)
        processManager_->createProcess("init", 0);
        
        logger_->info("All subsystems initialized successfully");
        return true;
        
    } catch (const std::exception& e) {
        // Log any initialization errors and return failure
        logger_->error("Failed to initialize subsystems: " + std::string(e.what()));
        return false;
    }
}

void Kernel::printBootMessage() {
    // Display TobiOS ASCII art banner
    std::cout << R"(
████████  ██████  ██████  ██  ██████  ███████ 
   ██    ██    ██ ██   ██ ██ ██    ██ ██      
   ██    ██    ██ ██████  ██ ██    ██ ███████ 
   ██    ██    ██ ██   ██ ██ ██    ██      ██ 
   ██     ██████  ██████  ██  ██████  ███████ 
                                              
)" << std::endl;
    
    // Display version and boot status
    std::cout << "TobiOS v1.0 - Operating System Simulation" << std::endl;
    std::cout << "Booting..." << std::endl << std::endl;
}

}