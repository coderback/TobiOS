#pragma once

#include <memory>
#include <string>
#include "memory_manager.h"
#include "process_manager.h"
#include "filesystem.h"
#include "shell.h"
#include "logger.h"

namespace TobiOS {

class Kernel {
public:
    Kernel();
    ~Kernel();

    bool boot();
    void run();
    void shutdown();

    MemoryManager* getMemoryManager() const { return memoryManager_.get(); }
    ProcessManager* getProcessManager() const { return processManager_.get(); }
    FileSystem* getFileSystem() const { return fileSystem_.get(); }
    Shell* getShell() const { return shell_.get(); }
    Logger* getLogger() const { return logger_.get(); }

    static Kernel& getInstance();

private:
    bool initializeSubsystems();
    static void printBootMessage();

    std::unique_ptr<Logger> logger_;
    std::unique_ptr<MemoryManager> memoryManager_;
    std::unique_ptr<ProcessManager> processManager_;
    std::unique_ptr<FileSystem> fileSystem_;
    std::unique_ptr<Shell> shell_;

    bool isRunning_;
    static Kernel* instance_;
};

}