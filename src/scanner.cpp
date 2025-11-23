#include <Scanner.hpp>
#include <TransitionTable.hpp>
#include <fstream>
#include <iostream>
#include <algorithm>

Scanner::Scanner(const std::string filename) {
    file = std::ifstream (filename);
    if (!file.is_open()){
        throw FileNotFoundException("Failed to open file: " + filename);
    }
    TransitionTable table;
    index = 0;
    line = 0;
    eof = false;
    this->readLine();
}

Token Scanner::nextToken() {

    int currState;
    int nextState;
    int first;
    unsigned char currChar;
    unsigned char nextChar;
    
    if (eof) {
        return Token(Category::CAT_EOF, -1); 
    }

    currChar = buffer[index];
    currState = table.table[0][currChar];
    if (currState == -1) {
        std::cerr << "ERROR " << this->line << ": \"" << currChar << "\" is not a valid word." << std::endl;
        index = buffer.size() - 1;
        return Token(Category::CAT_INVAL, -1); 
    }

    while (currState == 0){
        index ++;
        currChar = buffer[index];
        currState = table.table[0][currChar];
    }
    first = index;

    index ++;
    nextChar = buffer[index];
    nextState = table.table[currState][nextChar];

    while (nextState != -1) {
        index ++;
        currChar = nextChar;
        currState = nextState;
        nextChar = buffer[index];
        nextState = table.table[currState][nextChar];
    }

    if (table.accepting.find(currState) != table.accepting.end()) {
        Category category = table.stateToCategory[currState];
        if (category == Category::CAT_EOL) {
            readLine();
        }
        return Token(category, this->getLexeme(category, first));
    } else {
        int length = std::min<int>(index - first + 1, buffer.size() - first - 1);
        std::cerr << "ERROR " << this->line << ": \"" << buffer.substr(first, length) << "\" is not a valid word." << std::endl;
        index = buffer.size() - 1;
        return Token(Category::CAT_INVAL, -1);
    }

}

void Scanner::readLine() {
    if (!std::getline(file, buffer)) {
        eof = true;
        return;
    }
    buffer += "\n";
    index = 0;
    line ++;
}

int Scanner::getLexeme(Category category, int first) {
    switch (category) {
        case Category::CAT_MEMOP:
            if (buffer[first] == 's') {
                return (int) Opcode::STORE; 
            }
            return (int) Opcode::LOAD;
        case Category::CAT_LOADI:
            return (int) Opcode::LOADI;
        case Category::CAT_ARITHOP:
            if (buffer[first] == 'a') {
                return (int) Opcode::ADD; 
            }
            if (buffer[first] == 's') {
                return (int) Opcode::SUB; 
            }
            if (buffer[first] == 'm') {
                return (int) Opcode::MULT; 
            }
            if (buffer[first] == 'l') {
                return (int) Opcode::LSHIFT; 
            }
            return (int) Opcode::RSHIFT; 
        case Category::CAT_OUTPUT:
            return (int) Opcode::OUTPUT; 
        case Category::CAT_NOP:
            return (int) Opcode::NOP; 
        case Category::CAT_CONSTANT:
            return std::stoi(buffer.substr(first, index - first)); 
        case Category::CAT_REGISTER:
            return std::stoi(buffer.substr(first + 1, index - first - 1)); 
        default:
            return -1;
    }
}