#pragma once

#include <InternalRepresentation.hpp>
#include <string>
#include <exception>

class RenamingFailedException : public std::exception {
public:
    RenamingFailedException(const std::string& msg) : message(msg) {}
    const char* what() const noexcept override {
        return message.c_str();
    }
    
private:         
    std::string message;
};

class Renamer {
public:
    void rename(InternalRepresentation& rep);
};