/**
 * @file filesystem.cpp
 * @brief Implementation of the virtual filesystem for TobiOS
 * 
 * This file contains the implementation of the FileSystem class which
 * provides in-memory filesystem operations with UNIX-like directory structure.
 */

#include "filesystem.h"
#include <iostream>
#include <sstream>

namespace TobiOS {

FileSystem::FileSystem() {
    // Initialize the root directory node
    root_ = std::make_shared<FileNode>();
    root_->name = "";                    // Root has empty name
    root_->type = FileType::DIRECTORY;
    root_->size = 0;
    currentDirectory_ = root_;           // Start at root directory
}

FileSystem::~FileSystem() {
    // Smart pointers will automatically clean up the filesystem tree
}

bool FileSystem::createFile(const std::string& path, const std::string& content) {
    // Split the path into components and extract filename
    auto pathParts = splitPath(path);
    if (pathParts.empty()) return false;
    
    std::string filename = pathParts.back();  // Get the filename
    pathParts.pop_back();                     // Remove filename from path parts
    
    // Find the parent directory where we want to create the file
    auto parentDir = findNode("/" + [&]() {
        std::string result;
        for (size_t i = 0; i < pathParts.size(); ++i) {
            if (i > 0) result += "/";
            result += pathParts[i];
        }
        return result;
    }());
    
    // Verify parent directory exists and is actually a directory
    if (!parentDir || parentDir->type != FileType::DIRECTORY) {
        return false;
    }
    
    // Check if file with same name already exists
    if (parentDir->children.find(filename) != parentDir->children.end()) {
        return false;
    }
    
    // Create the new file node
    auto newFile = std::make_shared<FileNode>();
    newFile->name = filename;
    newFile->type = FileType::REGULAR_FILE;
    newFile->content = content;
    newFile->size = content.size();
    newFile->parent = parentDir;
    
    // Add the file to its parent directory
    parentDir->children[filename] = newFile;
    return true;
}

bool FileSystem::createDirectory(const std::string& path) {
    auto pathParts = splitPath(path);
    if (pathParts.empty()) return false;
    
    std::string dirname = pathParts.back();
    pathParts.pop_back();
    
    auto parentDir = findNode("/" + [&]() {
        std::string result;
        for (size_t i = 0; i < pathParts.size(); ++i) {
            if (i > 0) result += "/";
            result += pathParts[i];
        }
        return result;
    }());
    
    if (!parentDir || parentDir->type != FileType::DIRECTORY) {
        return false;
    }
    
    if (parentDir->children.find(dirname) != parentDir->children.end()) {
        return false;
    }
    
    auto newDir = std::make_shared<FileNode>();
    newDir->name = dirname;
    newDir->type = FileType::DIRECTORY;
    newDir->size = 0;
    newDir->parent = parentDir;
    
    parentDir->children[dirname] = newDir;
    return true;
}

bool FileSystem::deleteFile(const std::string& path) {
    auto node = findNode(path);
    if (!node) return false;
    
    auto parent = node->parent.lock();
    if (parent) {
        parent->children.erase(node->name);
        return true;
    }
    
    return false;
}

std::string FileSystem::readFile(const std::string& path) {
    auto node = findNode(path);
    if (node && node->type == FileType::REGULAR_FILE) {
        return node->content;
    }
    return "";
}

bool FileSystem::writeFile(const std::string& path, const std::string& content) {
    auto node = findNode(path);
    if (node && node->type == FileType::REGULAR_FILE) {
        node->content = content;
        node->size = content.size();
        return true;
    }
    return false;
}

std::vector<std::string> FileSystem::listDirectory(const std::string& path) {
    auto node = findNode(path);
    std::vector<std::string> result;
    
    if (node && node->type == FileType::DIRECTORY) {
        for (const auto& child : node->children) {
            result.push_back(child.first);
        }
    }
    
    return result;
}

bool FileSystem::exists(const std::string& path) {
    return findNode(path) != nullptr;
}

void FileSystem::printTree(const std::string& path, int indent) {
    auto node = findNode(path);
    if (!node) return;
    
    for (int i = 0; i < indent; ++i) {
        std::cout << "  ";
    }
    
    std::cout << node->name;
    if (node->type == FileType::DIRECTORY) {
        std::cout << "/";
    } else {
        std::cout << " (" << node->size << " bytes)";
    }
    std::cout << std::endl;
    
    if (node->type == FileType::DIRECTORY) {
        for (const auto& child : node->children) {
            printTree(path + "/" + child.first, indent + 1);
        }
    }
}

std::shared_ptr<FileNode> FileSystem::findNode(const std::string& path) {
    // Handle root directory special case
    if (path == "/") return root_;
    
    // Split path and traverse the filesystem tree
    auto parts = splitPath(path);
    auto current = root_;
    
    // Navigate through each path component
    for (const auto& part : parts) {
        // Check if the current directory contains the next path component
        if (current->children.find(part) == current->children.end()) {
            return nullptr;  // Path component not found
        }
        current = current->children[part];
    }
    
    return current;
}

std::vector<std::string> FileSystem::splitPath(const std::string& path) {
    // Split a filesystem path by '/' delimiter
    std::vector<std::string> parts;
    std::stringstream ss(path);
    std::string part;
    
    // Split by '/' and ignore empty parts (handles leading/trailing slashes)
    while (std::getline(ss, part, '/')) {
        if (!part.empty()) {
            parts.push_back(part);
        }
    }
    
    return parts;
}

}