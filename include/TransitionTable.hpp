#pragma once
#include <unordered_set>
#include <map>
#include <Token.hpp>

class TransitionTable{
private: 
    static const int NUM_STATES = 42;
    static const int NUM_CHARS = 128; 
    
public:
    int table[NUM_STATES][NUM_CHARS] = {{0}};
    std::unordered_set<int> accepting;
    std::map<int, Category> stateToCategory;

    TransitionTable() {

        for (int i = 0; i < NUM_STATES; i++) {
            for (int j = 0; j < NUM_CHARS; j++) {
                table[i][j] = -1;
            }
        }

        // STORE
        table[0]['s'] = 1;
        table[1]['t'] = 2;
        table[2]['o'] = 3;
        table[3]['r'] = 4;
        table[4]['e'] = 5;
        
        accepting.insert(5);
        stateToCategory[5] = Category::CAT_MEMOP;

        // SUB
        table[1]['u'] = 6;
        table[6]['b'] = 7;
        
        accepting.insert(7);
        stateToCategory[7] = Category::CAT_ARITHOP;

        // LOAD + LOADI
        table[0]['l'] = 8;
        table[8]['o'] = 9;
        table[9]['a'] = 10;
        table[10]['d'] = 11;
        table[11]['I'] = 12;
        
        accepting.insert(11);
        stateToCategory[11] = Category::CAT_MEMOP;
        
        accepting.insert(12);
        stateToCategory[12] = Category::CAT_LOADI;

        // RSHIFT + LSHIFT
        table[0]['r'] = 13;
        table[13]['s'] = 14;
        table[8]['s'] = 14;
        table[14]['h'] = 15;
        table[15]['i'] = 16;
        table[16]['f'] = 17;
        table[17]['t'] = 18;

        accepting.insert(18);
        stateToCategory[18] = Category::CAT_ARITHOP;

        // MULT
        table[0]['m'] = 19;
        table[19]['u'] = 20;
        table[20]['l'] = 21;
        table[21]['t'] = 18;

        // ADD
        table[0]['a'] = 22;
        table[22]['d'] = 23;
        table[23]['d'] = 24;

        accepting.insert(24);
        stateToCategory[24] = Category::CAT_ARITHOP;

        // NOP
        table[0]['n'] = 25;
        table[25]['o'] = 26;
        table[26]['p'] = 27;

        accepting.insert(27);
        stateToCategory[27] = Category::CAT_NOP;

        // OUTPUT
        table[0]['o'] = 28;
        table[28]['u'] = 29;
        table[29]['t'] = 30;
        table[30]['p'] = 31;
        table[31]['u'] = 32;
        table[32]['t'] = 33;

        accepting.insert(33);
        stateToCategory[33] = Category::CAT_OUTPUT;

        // INTO
        table[0]['='] = 34;
        table[34]['>'] = 35;

        accepting.insert(35);
        stateToCategory[35] = Category::CAT_INTO;

        // COMMA
        table[0][','] = 36;

        accepting.insert(36);
        stateToCategory[36] = Category::CAT_COMMA;

        // EOL
        table[0]['\n'] = 37;

        accepting.insert(37);
        stateToCategory[37] = Category::CAT_EOL;

        // CONSTANT ('0' = 48 -> '9' = 57)
        for (int i = 48; i <= 57; i++) {
            table[0][i] = 38;
            table[38][i] = 38;
        }

        accepting.insert(38);
        stateToCategory[38] = Category::CAT_CONSTANT;

        // REGISTER
        for (int i = 48; i <= 57; i++) {
            table[13][i] = 39;
            table[39][i] = 39;
        }

        accepting.insert(39);
        stateToCategory[39] = Category::CAT_REGISTER;

        // COMMENT
        table[0]['/'] = 40;
        table[40]['/'] = 41;
        for (int i = 0; i < NUM_CHARS; i++) {
            table[41][i] = 41;
        }
        table[41]['\n'] = 37;

        // WHITESPACE
        table[0][' '] = 0;
        table[0]['\t'] = 0;
        table[0]['\r'] = 0;
    }
};