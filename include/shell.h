/**
 * @file shell.h
 * @brief Interactive command-line shell interface for TobiOS
 * 
 * This file defines the Shell class which provides a UNIX-like command-line
 * interface for interacting with the TobiOS operating system simulation.
 */

#pragma once

#include <string>
#include <vector>
#include <functional>

namespace TobiOS {

class Kernel;

/**
 * @struct Command
 * @brief Represents a shell command with its handler function
 * 
 * This structure encapsulates a command's name, description, and the
 * function that implements its behavior. Commands are registered with
 * the shell and can be executed by users.
 */
struct Command {
    std::string name;        ///< Command name as typed by the user
    std::string description; ///< Help text describing what the command does
    
    /// Function that implements the command behavior
    /// @param args Vector of command arguments (args[0] is the command name)
    std::function<void(const std::vector<std::string>&)> handler;
};

/**
 * @class Shell
 * @brief Interactive command-line shell for TobiOS
 * 
 * The Shell class provides a UNIX-like command-line interface that allows
 * users to interact with the operating system. Features include:
 * - Command parsing and execution
 * - Built-in commands for filesystem, process, and system operations
 * - Extensible command registration system
 * - Interactive prompt with command history
 * - Help system for discovering available commands
 * 
 * The shell serves as the primary user interface for TobiOS and demonstrates
 * how user-space applications interact with kernel services.
 */
class Shell {
public:
    /**
     * @brief Constructor - initializes shell with reference to kernel
     * @param kernel Pointer to the TobiOS kernel for accessing system services
     */
    Shell(Kernel* kernel);
    
    /**
     * @brief Destructor - cleans up shell resources
     */
    ~Shell();

    /**
     * @brief Start the interactive shell loop
     * 
     * Displays the shell prompt and processes user commands until
     * the user exits. This is a blocking call that runs the main
     * shell interaction loop.
     */
    void run();
    
    /**
     * @brief Execute a single command line
     * @param commandLine Complete command line string to parse and execute
     */
    void executeCommand(const std::string& commandLine);
    
    /**
     * @brief Register a new command with the shell
     * @param name Command name that users will type
     * @param description Help text for the command
     * @param handler Function to call when command is executed
     */
    void registerCommand(const std::string& name, const std::string& description,
                        std::function<void(const std::vector<std::string>&)> handler);

private:
    Kernel* kernel_;                ///< Reference to the kernel for system calls
    std::vector<Command> commands_; ///< List of all registered commands
    bool running_;                  ///< Flag controlling the main shell loop

    /**
     * @brief Initialize all built-in shell commands
     * 
     * Registers standard commands like help, ls, cat, ps, etc.
     * This is called during shell construction.
     */
    void initializeBuiltinCommands();
    
    /**
     * @brief Parse a command line into command and arguments
     * @param commandLine Raw command line string from user input
     * @return Vector where [0] is command name and [1..n] are arguments
     */
    std::vector<std::string> parseCommand(const std::string& commandLine);
    
    // Built-in command implementations
    
    /**
     * @brief Implementation of the 'help' command
     * @param args Command arguments (unused for help)
     */
    void helpCommand(const std::vector<std::string>& args);
    
    /**
     * @brief Implementation of the 'exit' command
     * @param args Command arguments (unused for exit)
     */
    void exitCommand(const std::vector<std::string>& args);
    
    /**
     * @brief Implementation of the 'ls' command (list directory)
     * @param args Command arguments: [0]=command, [1]=optional path
     */
    void lsCommand(const std::vector<std::string>& args);
    
    /**
     * @brief Implementation of the 'cat' command (display file contents)
     * @param args Command arguments: [0]=command, [1]=filename
     */
    void catCommand(const std::vector<std::string>& args);
    
    /**
     * @brief Implementation of the 'ps' command (list processes)
     * @param args Command arguments (unused for ps)
     */
    void psCommand(const std::vector<std::string>& args);
    
    /**
     * @brief Implementation of the 'mkdir' command (create directory)
     * @param args Command arguments: [0]=command, [1]=directory path
     */
    void mkdirCommand(const std::vector<std::string>& args);
    
    /**
     * @brief Implementation of the 'touch' command (create empty file)
     * @param args Command arguments: [0]=command, [1]=filename
     */
    void touchCommand(const std::vector<std::string>& args);
};

}