cmake_minimum_required(VERSION 3.20)
project(TobiOS VERSION 1.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Source files
set(SOURCES
    main.cpp
    src/kernel/kernel.cpp
    src/kernel/logger.cpp
    src/memory/memory_manager.cpp
    src/process/process_manager.cpp
    src/filesystem/filesystem.cpp
    src/shell/shell.cpp
)

# Create executable
add_executable(TobiOS ${SOURCES})

# Set target properties
target_include_directories(TobiOS PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Set C++ standard for target
target_compile_features(TobiOS PRIVATE cxx_std_20)
