# TobiOS - Operating System Simulation

```
████████  ██████  ██████  ██  ██████  ███████ 
   ██    ██    ██ ██   ██ ██ ██    ██ ██      
   ██    ██    ██ ██████  ██ ██    ██ ███████ 
   ██    ██    ██ ██   ██ ██ ██    ██      ██ 
   ██     ██████  ██████  ██  ██████  ███████ 
```

A comprehensive operating system simulation written in C++ that demonstrates core OS concepts including memory management, process scheduling, file systems, and shell interfaces.

## Features

### Core OS Components
- **Kernel Framework**: Complete boot sequence with system initialization
- **Memory Management**: Virtual memory simulation with dynamic allocation/deallocation
- **Process Management**: Process Control Blocks (PCB), scheduling, and state management
- **File System**: Virtual filesystem with directories, files, and UNIX-like operations
- **Interactive Shell**: Full command-line interface with built-in commands
- **Logging System**: Multi-level colored logging with timestamps

### Educational Value
- Demonstrates real OS concepts in a safe, simulated environment
- Clean, modular architecture following software engineering best practices
- Extensive logging for understanding system operations
- Interactive exploration of OS internals

## Building TobiOS

### Prerequisites
- C++20 compatible compiler (GCC 10+ or Clang 12+)
- CMake 3.20+ (optional)
- Make or Ninja build system

### Quick Build
```bash
# Using G++ directly
g++ -std=c++20 -I include -o TobiOS main.cpp src/kernel/*.cpp src/memory/*.cpp src/process/*.cpp src/filesystem/*.cpp src/shell/*.cpp

# Or using CMake (if available)
mkdir build && cd build
cmake .. && make
```

### Project Structure
```
TobiOS/
├── main.cpp              # Entry point
├── CMakeLists.txt         # Build configuration
├── include/               # Header files
│   ├── kernel.h
│   ├── logger.h
│   ├── memory_manager.h
│   ├── process_manager.h
│   ├── filesystem.h
│   └── shell.h
├── src/                   # Implementation files
│   ├── kernel/
│   ├── memory/
│   ├── process/
│   ├── filesystem/
│   └── shell/
└── tests/                 # Test files (future)
```

## Running TobiOS

### Start the OS
```bash
./TobiOS
```

### Sample Session
```bash
$ ./TobiOS

████████  ██████  ██████  ██  ██████  ███████ 
   ██    ██    ██ ██   ██ ██ ██    ██ ██      
   ██    ██    ██ ██████  ██ ██    ██ ███████ 
   ██    ██    ██ ██   ██ ██ ██    ██      ██ 
   ██     ██████  ██████  ██  ██████  ███████ 

TobiOS v1.0 - Operating System Simulation
Booting...

[INFO] TobiOS Kernel starting...
[INFO] All subsystems initialized successfully

=== TobiOS Ready ===
TobiOS Shell started. Type 'help' for available commands.

$ help
Available commands:
  help - Show available commands
  status - Show system status
  ls - List directory contents
  cat - Display file contents
  mkdir - Create directory
  touch - Create empty file
  ps - List running processes
  exit - Exit the shell

$ ls /
bin
tmp
home

$ cat /home/welcome.txt
Welcome to TobiOS!
This is a simulated operating system.

$ mkdir /home/user
Directory created: /home/user

$ ps
=== Process List ===
PID	Name		State		Priority
---	----		-----		--------
1	init		READY		0

$ exit
TobiOS has been shut down. Goodbye!
```

## Available Commands

| Command | Description | Usage Example |
|---------|-------------|---------------|
| `help` | Show all available commands | `help` |
| `status` | Display system status | `status` |
| `ls [path]` | List directory contents | `ls /home` |
| `cat <file>` | Display file contents | `cat /home/welcome.txt` |
| `mkdir <dir>` | Create a new directory | `mkdir /home/newdir` |
| `touch <file>` | Create an empty file | `touch /home/test.txt` |
| `ps` | List running processes | `ps` |
| `exit` | Shutdown TobiOS | `exit` |

## Architecture Overview

### Kernel (`src/kernel/`)
- **Singleton pattern** for system-wide access
- **Boot sequence** with subsystem initialization
- **Graceful shutdown** with resource cleanup
- **Centralized logging** with colored output

### Memory Manager (`src/memory/`)
- **1MB virtual memory** simulation
- **Dynamic allocation** with first-fit algorithm
- **Memory mapping** and fragmentation handling
- **Process-based memory tracking**

### Process Manager (`src/process/`)
- **Process Control Blocks (PCB)** with full state tracking
- **Process states**: NEW, READY, RUNNING, BLOCKED, TERMINATED
- **Round-robin scheduling** (ready for priority-based)
- **Parent-child relationships**

### File System (`src/filesystem/`)
- **Virtual filesystem** with in-memory storage
- **UNIX-like directory structure** (/, /home, /tmp, /bin)
- **File operations**: create, read, write, delete
- **Directory operations**: mkdir, ls, tree traversal

### Shell (`src/shell/`)
- **Command parsing** and execution
- **Extensible command system** with lambda handlers
- **Interactive user interface**
- **Error handling** and help system

## Learning Objectives

This project demonstrates:

1. **Operating System Concepts**
   - Kernel architecture and boot process
   - Memory management and virtual memory
   - Process lifecycle and scheduling
   - File system organization
   - System calls and user interfaces

2. **Software Engineering Practices**
   - Modular design and separation of concerns
   - RAII and smart pointer usage
   - Error handling and logging
   - Clean interfaces and abstraction

## Extending TobiOS

### Planned Enhancements
- **System Calls Interface**: Formal syscall dispatcher and API
- **Hardware Simulation**: CPU registers, interrupts, and timers
- **Network Stack**: TCP/IP simulation for IPC
- **Security Model**: User permissions and access control
- **Performance Monitoring**: System metrics and profiling

### Adding New Commands
```cpp
// In shell.cpp constructor
registerCommand("mycommand", "Description of my command",
    [this](const std::vector<std::string>& args) {
        // Command implementation
        std::cout << "Hello from my command!" << std::endl;
    });
```


### Common Issues

**Compilation Errors**
```bash
# Ensure C++20 support
g++ --version  # Should be GCC 10+ or Clang 12+

# Check include paths
g++ -std=c++20 -I include ...
```

**Runtime Issues**
- Check that all source files are compiled
- Verify memory allocation (1MB default)
- Review console output for error messages

**Shell Not Responding**
- Ensure proper command syntax
- Use `help` to see available commands
- Check for typos in command names
