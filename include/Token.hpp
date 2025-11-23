#pragma once

#include <string>
#include <stdexcept>
#include <Opcode.hpp>

enum class Category: int {
    CAT_MEMOP = 0,
    CAT_LOADI,
    CAT_ARITHOP,
    CAT_OUTPUT,
    CAT_NOP,
    CAT_CONSTANT,
    CAT_REGISTER,
    CAT_COMMA,
    CAT_INTO,
    CAT_EOF,
    CAT_EOL,
    CAT_INVAL
};

const std::string CategoryNames[12] = {
    "MEMOP",
    "LOADI",
    "ARITHOP",
    "OUTPUT",
    "NOP",
    "CONST",
    "REG",
    "COMMA",
    "INTO",
    "ENDFILE",
    "NEWLINE",
    "INVALID"
};

class Token {
public:
    Category category;
    int lexeme; 

    Token(Category category, int lexeme) : category(category), lexeme(lexeme) {}

    std::string getLexeme() {
        switch (this->category) {
            case Category::CAT_MEMOP:
            case Category::CAT_LOADI:
            case Category::CAT_ARITHOP:
            case Category::CAT_OUTPUT:
            case Category::CAT_NOP:
                return OpcodeNames[lexeme];
            case Category::CAT_CONSTANT:
                return std::to_string(lexeme);
            case Category::CAT_REGISTER:
                return "r" + std::to_string(lexeme);
            case Category::CAT_COMMA:
                return ",";
            case Category::CAT_INTO:
                return "=>";
            case Category::CAT_EOL:
                return "\\n";
            case Category::CAT_EOF:
                return "";
            case Category::CAT_INVAL:
                return "<invalid>";
            default:
                throw std::invalid_argument("Token has invalid category.");
        }
    }

    std::string toString() {
        return "< " + CategoryNames[(int) category] + ", " + "\"" + this->getLexeme() +"\"" + " >";
    }
};
