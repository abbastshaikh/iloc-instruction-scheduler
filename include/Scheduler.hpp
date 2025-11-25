#pragma once

#include <InternalRepresentation.hpp>
#include <Graph.hpp>
#include <unordered_map>
#include <queue>
#include <vector>

/* Dependence Graph and Related Types */
enum class Status {
    NOT_READY,
    READY,
    ACTIVE,
    RETIRED
};

struct OperationData {
    Operation op;
    Status status;
};

using DependenceGraph = Graph<OperationData>;

/* Operation Priority Queue */
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

/* Schedule and Scheduler */
struct Schedule {
    std::vector<std::pair<Operation, Operation>> cycles;
};

class Scheduler {
public:
    Schedule schedule (InternalRepresentation& rep);

private:
    DependenceGraph buildDependenceGraph (const InternalRepresentation& rep);
    std::unordered_map<int, int> getPriorities(DependenceGraph& graph);
};