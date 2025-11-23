#pragma once

#include <Opcode.hpp>
#include <string>
#include <stdexcept>

struct Operand {
    int SR = -1;
    int VR = -1;
    int PR = -1;
    int NU = -1;
};

struct Operation {
    Opcode opcode;
    Operand op1;
    Operand op2;
    Operand op3;

    std::string printIR () {
        switch (this->opcode) {
            case Opcode::LOAD: 
            case Opcode::STORE: 
                return OpcodeNamesPadded[(int) opcode] + " [ sr" + std::to_string(op1.SR) + " ]," + " [ ]," + " [ sr" + std::to_string(op3.SR) + " ]";
            case Opcode::LOADI:
                return OpcodeNamesPadded[(int) opcode] + " [ val " + std::to_string(op1.SR) + " ]," + " [ ]," + " [ sr" + std::to_string(op3.SR) + " ]";
            case Opcode::ADD: 
            case Opcode::SUB:
            case Opcode::MULT: 
            case Opcode::LSHIFT: 
            case Opcode::RSHIFT: 
                return OpcodeNamesPadded[(int) opcode] + " [ sr" + std::to_string(op1.SR) + " ]," + " [ sr" + std::to_string(op2.SR) + " ]," + " [ sr" + std::to_string(op3.SR) + " ]";
            case Opcode::OUTPUT:
                return OpcodeNamesPadded[(int) opcode] + " [ val " + std::to_string(op1.SR) + " ]," + " [ ]," + " [ ]";
            case Opcode::NOP: 
                return OpcodeNamesPadded[(int) opcode] + " [ ]," + " [ ]," + " [ ]";
            default:
                throw std::invalid_argument("Operation has invalid opcode.");
        }
    }

    std::string printVR () {
        switch (this->opcode) {
            case Opcode::LOAD: 
            case Opcode::STORE: 
                return OpcodeNamesPadded[(int) opcode] + "r" + std::to_string(op1.VR) + " => r" + std::to_string(op3.VR);
            case Opcode::LOADI:
                return OpcodeNamesPadded[(int) opcode] + std::to_string(op1.SR) + " => r" + std::to_string(op3.VR);
            case Opcode::ADD: 
            case Opcode::SUB:
            case Opcode::MULT: 
            case Opcode::LSHIFT: 
            case Opcode::RSHIFT: 
                return OpcodeNamesPadded[(int) opcode] + "r" + std::to_string(op1.VR) + ", r" + std::to_string(op2.VR) + " => r" + std::to_string(op3.VR);
            case Opcode::OUTPUT:
                return OpcodeNamesPadded[(int) opcode] + std::to_string(op1.SR);
            case Opcode::NOP: 
                return OpcodeNamesPadded[(int) opcode];
            default:
                throw std::invalid_argument("Operation has invalid opcode.");
        }
    }

    std::string printPR () {
        switch (this->opcode) {
            case Opcode::LOAD: 
            case Opcode::STORE: 
                return OpcodeNamesPadded[(int) opcode] + "r" + std::to_string(op1.PR) + " => r" + std::to_string(op3.PR);
            case Opcode::LOADI:
                return OpcodeNamesPadded[(int) opcode] + std::to_string(op1.SR) + " => r" + std::to_string(op3.PR);
            case Opcode::ADD: 
            case Opcode::SUB:
            case Opcode::MULT: 
            case Opcode::LSHIFT: 
            case Opcode::RSHIFT: 
                return OpcodeNamesPadded[(int) opcode] + "r" + std::to_string(op1.PR) + ", r" + std::to_string(op2.PR) + " => r" + std::to_string(op3.PR);
            case Opcode::OUTPUT:
                return OpcodeNamesPadded[(int) opcode] + std::to_string(op1.SR);
            case Opcode::NOP: 
                return OpcodeNamesPadded[(int) opcode];
            default:
                throw std::invalid_argument("Operation has invalid opcode.");
        }
    }
};

