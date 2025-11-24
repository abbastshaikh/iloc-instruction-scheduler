#pragma once

#include <Operation.hpp>
#include <vector>

struct InternalRepresentation {
    std::vector<Operation> operations;
    int maxSR = -1;
    int maxVR = -1;
    int maxLive = -1;
};