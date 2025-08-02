/**
 * @file shell.cpp
 * @brief Implementation of the interactive command-line shell
 * 
 * This file contains the implementation of the Shell class which provides
 * a UNIX-like command-line interface for interacting with TobiOS services.
 */

#include "shell.h"
#include "kernel.h"
#include <iostream>
#include <sstream>
#include <algorithm>

namespace TobiOS {

Shell::Shell(Kernel* kernel) : kernel_(kernel), running_(false) {
    // Initialize all built-in commands during shell construction
    initializeBuiltinCommands();
}

Shell::~Shell() {}

void Shell::run() {
    // Start the main interactive shell loop
    running_ = true;
    std::cout << "TobiOS Shell started. Type 'help' for available commands." << std::endl;
    
    // Main shell loop - read and execute commands until user exits
    while (running_) {
        std::cout << "$ ";  // Display shell prompt
        std::string commandLine;
        std::getline(std::cin, commandLine);  // Read full line from user
        
        // Execute command if user entered something
        if (!commandLine.empty()) {
            executeCommand(commandLine);
        }
    }
}

void Shell::executeCommand(const std::string& commandLine) {
    // Parse the command line into command and arguments
    auto args = parseCommand(commandLine);
    if (args.empty()) return;
    
    std::string commandName = args[0];  // First argument is the command name
    
    // Search for the command in our registered command list
    auto it = std::find_if(commands_.begin(), commands_.end(),
                          [&commandName](const Command& cmd) {
                              return cmd.name == commandName;
                          });
    
    // Execute the command if found, otherwise show error message
    if (it != commands_.end()) {
        it->handler(args);  // Call the command's handler function
    } else {
        std::cout << "Unknown command: " << commandName << std::endl;
        std::cout << "Type 'help' for available commands." << std::endl;
    }
}

void Shell::registerCommand(const std::string& name, const std::string& description,
                          std::function<void(const std::vector<std::string>&)> handler) {
    // Add a new command to the shell's command registry
    Command cmd;
    cmd.name = name;
    cmd.description = description;
    cmd.handler = handler;
    commands_.push_back(cmd);
}

void Shell::initializeBuiltinCommands() {
    // Register all built-in shell commands with their descriptions and handlers
    
    registerCommand("help", "Show available commands", 
                   [this](const std::vector<std::string>& args) { helpCommand(args); });
    
    registerCommand("exit", "Exit the shell", 
                   [this](const std::vector<std::string>& args) { exitCommand(args); });
    
    registerCommand("ls", "List directory contents", 
                   [this](const std::vector<std::string>& args) { lsCommand(args); });
    
    registerCommand("cat", "Display file contents", 
                   [this](const std::vector<std::string>& args) { catCommand(args); });
    
    registerCommand("ps", "List running processes", 
                   [this](const std::vector<std::string>& args) { psCommand(args); });
    
    registerCommand("mkdir", "Create directory", 
                   [this](const std::vector<std::string>& args) { mkdirCommand(args); });
    
    registerCommand("touch", "Create empty file", 
                   [this](const std::vector<std::string>& args) { touchCommand(args); });
    
    // System status command - shows the state of all kernel subsystems
    registerCommand("status", "Show system status", 
                   [this](const std::vector<std::string>& args) { 
                       std::cout << "=== TobiOS System Status ===" << std::endl;
                       std::cout << "Kernel: Running" << std::endl;
                       std::cout << "Memory Manager: " << (kernel_->getMemoryManager() ? "Initialized" : "Not initialized") << std::endl;
                       std::cout << "Process Manager: " << (kernel_->getProcessManager() ? "Initialized" : "Not initialized") << std::endl;
                       std::cout << "File System: " << (kernel_->getFileSystem() ? "Initialized" : "Not initialized") << std::endl;
                       std::cout << "Shell: " << (kernel_->getShell() ? "Initialized" : "Not initialized") << std::endl;
                   });
}

std::vector<std::string> Shell::parseCommand(const std::string& commandLine) {
    // Parse command line by splitting on whitespace
    std::vector<std::string> args;
    std::stringstream ss(commandLine);
    std::string arg;
    
    // Extract each whitespace-separated token
    while (ss >> arg) {
        args.push_back(arg);
    }
    
    return args;
}

void Shell::helpCommand(const std::vector<std::string>& args) {
    // Display all available commands and their descriptions
    std::cout << "Available commands:" << std::endl;
    for (const auto& cmd : commands_) {
        std::cout << "  " << cmd.name << " - " << cmd.description << std::endl;
    }
}

void Shell::exitCommand(const std::vector<std::string>& args) {
    // Terminate the shell loop
    std::cout << "Exiting shell..." << std::endl;
    running_ = false;
}

void Shell::lsCommand(const std::vector<std::string>& args) {
    std::string path = (args.size() > 1) ? args[1] : "/";
    auto files = kernel_->getFileSystem()->listDirectory(path);
    
    if (files.empty()) {
        std::cout << "Directory is empty or does not exist." << std::endl;
    } else {
        for (const auto& file : files) {
            std::cout << file << std::endl;
        }
    }
}

void Shell::catCommand(const std::vector<std::string>& args) {
    if (args.size() < 2) {
        std::cout << "Usage: cat <filename>" << std::endl;
        return;
    }
    
    std::string content = kernel_->getFileSystem()->readFile(args[1]);
    if (content.empty()) {
        std::cout << "File not found or empty: " << args[1] << std::endl;
    } else {
        std::cout << content << std::endl;
    }
}

void Shell::psCommand(const std::vector<std::string>& args) {
    kernel_->getProcessManager()->printProcessList();
}

void Shell::mkdirCommand(const std::vector<std::string>& args) {
    if (args.size() < 2) {
        std::cout << "Usage: mkdir <directory>" << std::endl;
        return;
    }
    
    if (kernel_->getFileSystem()->createDirectory(args[1])) {
        std::cout << "Directory created: " << args[1] << std::endl;
    } else {
        std::cout << "Failed to create directory: " << args[1] << std::endl;
    }
}

void Shell::touchCommand(const std::vector<std::string>& args) {
    if (args.size() < 2) {
        std::cout << "Usage: touch <filename>" << std::endl;
        return;
    }
    
    if (kernel_->getFileSystem()->createFile(args[1])) {
        std::cout << "File created: " << args[1] << std::endl;
    } else {
        std::cout << "Failed to create file: " << args[1] << std::endl;
    }
}

}