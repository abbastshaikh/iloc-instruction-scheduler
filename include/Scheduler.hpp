#pragma once

#include <InternalRepresentation.hpp>
#include <DependenceGraph.hpp>
#include <unordered_map>
#include <queue>
#include <list>

struct Schedule {
    std::list<std::pair<Operation, Operation>> cycles;
};

class Scheduler {
public:
    struct OperationPriority {
        int id;
        int priority;

        OperationPriority(int id, int priority) : id(id), priority(priority) {}
    };

    struct CompareOperation {
        bool operator()(const OperationPriority& p1, const OperationPriority& p2) {
            return p1.priority < p2.priority;
        }  
    };

    using OperationPriorityQueue = std::priority_queue<OperationPriority, std::vector<OperationPriority>, CompareOperation>;

    Schedule schedule (InternalRepresentation& rep);

private:
    DependenceGraph buildDependenceGraph (const InternalRepresentation& rep);
    std::unordered_map<int, int> getPriorities(DependenceGraph& graph);
};