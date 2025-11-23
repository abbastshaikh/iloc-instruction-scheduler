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