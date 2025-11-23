#include <Renamer.hpp>
#include <Operation.hpp>
#include <algorithm>
#include <vector>

void Renamer::rename(InternalRepresentation& rep){

    int VRName = 0;
    int index = rep.operations.size();
    int maxLive = 0;
    int live = 0;

    int SRToVR [rep.maxSR + 1];
    int LU [rep.maxSR + 1];
    for (int i = 0; i <= rep.maxSR; i ++) {
        SRToVR[i] = -1;
        LU[i] = -1;
    }

    for (auto op = rep.operations.rbegin(); op != rep.operations.rend(); ++op) {

        Operand& o = op->op3;
        if (op->opcode != Opcode::STORE && o.SR != -1) {
            if (SRToVR[o.SR] == -1) {
                SRToVR[o.SR] = VRName++;
                live ++;
            }
            o.VR = SRToVR[o.SR];
            o.NU = LU[o.SR];
            SRToVR[o.SR] = -1;
            LU[o.SR]= -1;
            live --;
        }

        std::vector<Operand*> uses;
        switch (op->opcode) {
            case Opcode::LOAD:
                uses.push_back(&op->op1);
                break;
            case Opcode::STORE:
                uses.push_back(&op->op1);
                uses.push_back(&op->op3);
                break;
            case Opcode::ADD:
            case Opcode::SUB:
            case Opcode::MULT:
            case Opcode::LSHIFT:
            case Opcode::RSHIFT:
                uses.push_back(&op->op1);
                uses.push_back(&op->op2);
                break;
            default:
                break;
        }

        for (Operand* o: uses) {
            if (SRToVR[o->SR] == -1) {
                SRToVR[o->SR] = VRName++;
                live ++;
            }
            o->VR = SRToVR[o->SR];
            o->NU = LU[o->SR];
        }

        for (Operand* o: uses) {
            LU[o->SR] = index;
        }
        index--;

        maxLive = std::max(maxLive, live);
    }

    if (live > 0) {
        throw RenamingFailedException("Input block uses values from registers that have no prior definition.");
    }

    rep.maxVR = VRName;
    rep.maxLive = maxLive;
}