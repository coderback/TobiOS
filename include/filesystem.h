/**
 * @file filesystem.h
 * @brief Virtual filesystem implementation for TobiOS
 * 
 * This file defines the FileSystem class which implements an in-memory
 * virtual filesystem with UNIX-like directory structure and operations.
 */

#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <memory>

namespace TobiOS {

/**
 * @enum FileType
 * @brief Enumeration of supported file system entry types
 */
enum class FileType {
    REGULAR_FILE,   ///< Standard file containing data
    DIRECTORY       ///< Directory containing other files and directories
};

/**
 * @struct FileNode
 * @brief Represents a single file system entry (file or directory)
 * 
 * This structure implements a tree-based filesystem where each node
 * can be either a file (with content) or a directory (with children).
 * Uses shared_ptr for children and weak_ptr for parent to avoid cycles.
 */
struct FileNode {
    std::string name;       ///< Name of this file/directory (without path)
    FileType type;          ///< Whether this is a file or directory
    size_t size;            ///< Size in bytes (for files) or 0 (for directories)
    std::string content;    ///< File content (empty for directories)
    
    /// Map of child names to child nodes (for directories only)
    std::unordered_map<std::string, std::shared_ptr<FileNode>> children;
    
    /// Weak reference to parent node to avoid circular references
    std::weak_ptr<FileNode> parent;
};

/**
 * @class FileSystem
 * @brief In-memory virtual filesystem with UNIX-like path operations
 * 
 * The FileSystem class provides a complete virtual filesystem implementation
 * that supports:
 * - Hierarchical directory structure starting from root (/)
 * - File creation, reading, writing, and deletion
 * - Directory creation and listing
 * - Path-based navigation (absolute paths only)
 * - Tree visualization for debugging
 * 
 * All operations use absolute paths from the root directory.
 * The filesystem is entirely in-memory and does not persist between runs.
 */
class FileSystem {
public:
    /**
     * @brief Constructor - initializes filesystem with empty root directory
     */
    FileSystem();
    
    /**
     * @brief Destructor - cleans up all filesystem nodes
     */
    ~FileSystem();

    /**
     * @brief Create a new file at the specified path
     * @param path Absolute path where the file should be created
     * @param content Initial content for the file (optional)
     * @return true if file was created successfully, false if path invalid or file exists
     */
    bool createFile(const std::string& path, const std::string& content = "");
    
    /**
     * @brief Create a new directory at the specified path
     * @param path Absolute path where the directory should be created
     * @return true if directory was created successfully, false if path invalid or exists
     */
    bool createDirectory(const std::string& path);
    
    /**
     * @brief Delete a file or directory at the specified path
     * @param path Absolute path to the file/directory to delete
     * @return true if deletion was successful, false if path not found
     */
    bool deleteFile(const std::string& path);
    
    /**
     * @brief Read the contents of a file
     * @param path Absolute path to the file to read
     * @return File contents as string, or empty string if file not found or is directory
     */
    std::string readFile(const std::string& path);
    
    /**
     * @brief Write content to an existing file
     * @param path Absolute path to the file to write
     * @param content New content to write to the file
     * @return true if write was successful, false if file not found or is directory
     */
    bool writeFile(const std::string& path, const std::string& content);
    
    /**
     * @brief List the contents of a directory
     * @param path Absolute path to the directory to list
     * @return Vector of entry names in the directory, empty if directory not found
     */
    std::vector<std::string> listDirectory(const std::string& path);
    
    /**
     * @brief Check if a file or directory exists at the given path
     * @param path Absolute path to check
     * @return true if path exists, false otherwise
     */
    bool exists(const std::string& path);
    
    /**
     * @brief Print a tree representation of the filesystem structure
     * @param path Root path to start printing from (default: "/")
     * @param indent Current indentation level for recursive calls (default: 0)
     */
    void printTree(const std::string& path = "/", int indent = 0);

private:
    std::shared_ptr<FileNode> root_;            ///< Root directory node (represents "/")
    std::shared_ptr<FileNode> currentDirectory_; ///< Current working directory (unused in current implementation)
    
    /**
     * @brief Find a filesystem node by its absolute path
     * @param path Absolute path to search for
     * @return Shared pointer to the node if found, nullptr otherwise
     */
    std::shared_ptr<FileNode> findNode(const std::string& path);
    
    /**
     * @brief Split a filesystem path into its component parts
     * @param path Absolute path to split (e.g., "/home/user/file.txt")
     * @return Vector of path components (e.g., ["home", "user", "file.txt"])
     */
    std::vector<std::string> splitPath(const std::string& path);
};

}