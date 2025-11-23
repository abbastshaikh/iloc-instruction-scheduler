#pragma once

#include <Operation.hpp>
#include <list>

struct InternalRepresentation {
    std::list<Operation> operations;
    int maxSR = -1;
    int maxVR = -1;
    int maxLive = -1;
};