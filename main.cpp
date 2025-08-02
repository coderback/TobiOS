/**
 * @file main.cpp
 * @brief Entry point for TobiOS - Operating System Simulation
 * 
 * This file contains the main function that initializes and starts the TobiOS kernel.
 * TobiOS is an educational operating system simulation that demonstrates core OS concepts
 * including memory management, process scheduling, file systems, and shell interfaces.
 */

#include <iostream>
#include <exception>
#include "kernel.h"

/**
 * @brief Main entry point for TobiOS
 * 
 * Creates a kernel instance, boots the system, and runs the main loop.
 * Handles exceptions and provides appropriate error messages on failure.
 * 
 * @return 0 on successful execution, 1 on error
 */
int main() {
    try {
        // Create the TobiOS kernel instance
        TobiOS::Kernel kernel;
        
        // Attempt to boot the system - initializes all subsystems
        if (!kernel.boot()) {
            std::cerr << "Failed to boot TobiOS" << std::endl;
            return 1;
        }
        
        // Start the main kernel loop (which runs the shell)
        kernel.run();
        
    } catch (const std::exception& e) {
        // Handle any fatal errors during execution
        std::cerr << "Fatal error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}