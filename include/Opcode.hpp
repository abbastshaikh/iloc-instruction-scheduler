#pragma once

#include <string>

enum class Opcode: int {
    LOAD = 0,
    STORE,
    LOADI,
    ADD,
    SUB,
    MULT,
    LSHIFT,
    RSHIFT,
    OUTPUT,
    NOP
};

const std::string OpcodeNames[10] = {
    "load",
    "store",
    "loadI",
    "add",
    "sub",
    "mult",
    "lshift",
    "rshift",
    "output",
    "nop"
};

const std::string OpcodeNamesPadded[10] = {
    "load   ",
    "store  ",
    "loadI  ",
    "add    ",
    "sub    ",
    "mult   ",
    "lshift ",
    "rshift ",
    "output ",
    "nop    "
};

const int Latency[10] = {
    6, // LOAD
    6, // STORE
    1, // LOADI
    1, // ADD
    1, // SUB
    3, // MULT
    1, // LSHIFT
    1, // RSHIFT
    1, // OUTPUT
    1  // NOP
};