#pragma once

#include <Token.hpp>
#include <TransitionTable.hpp>
#include <fstream>
#include <string>
#include <exception>

class Scanner {
public:
    Scanner(const std::string filename);
    Token nextToken();

private:
    std::ifstream file;
    TransitionTable table;
    std::string buffer;
    int index;
    int line;
    bool eof;

    void readLine();
    int getLexeme(Category category, int first);
};

class FileNotFoundException : public std::exception {
public:
    FileNotFoundException(const std::string& msg) : message(msg) {}
    const char* what() const noexcept override {
        return message.c_str();
    }

private:         
    std::string message;
};