#pragma once

#include <Scanner.hpp>
#include <InternalRepresentation.hpp>
#include <string>
#include <exception>
#include <list>

class InvalidTokenException : public std::exception {
public:
    InvalidTokenException(Token t, const std::string& msg) : token(t), message(msg) {}
    const char* what() const noexcept override {
        return message.c_str();
    }
    Token getToken() const {
        return token;
    }

private:
    Token token;          
    std::string message;
};

class ParseFailedException : public std::exception {
public:
    ParseFailedException(const std::string& msg) : message(msg) {}
    const char* what() const noexcept override {
        return message.c_str();
    }
    
private:         
    std::string message;
};

class UnexpectedEOFException : public std::exception {
public:
    UnexpectedEOFException(const std::string& msg) : message(msg) {}
    const char* what() const noexcept override {
        return message.c_str();
    }

private:         
    std::string message;
};

class Parser {
public:
    Parser(Scanner& scanner) : scanner(scanner), line (0) {}
    InternalRepresentation parse();

private:
    Scanner& scanner;
    int line;
    Operation finishMEMOP(Opcode opcode);
    Operation finishLOADI(Opcode opcode);
    Operation finishARITHOP(Opcode opcode);
    Operation finishOUTPUT(Opcode opcode);
    Operation finishNOP(Opcode opcode);
    void readToNextLine();
    void handleInvalidToken(const InvalidTokenException& e);
};