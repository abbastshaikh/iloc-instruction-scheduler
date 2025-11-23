#include <Scheduler.hpp>
#include <DependenceGraph.hpp>
#include <Operation.hpp>
#include <unordered_map>
#include <iostream>
#include <queue>
#include <vector>

void Scheduler::schedule(InternalRepresentation& rep) {

    // Construct dependence graph
    DependenceGraph graph = buildDependenceGraph(rep);

    // Compute priorities using maximum latency-weighted path
    std::unordered_map<int, int> priorities = getPriorities(graph);

    // Schedule operations based on priorities
    // ... (scheduling logic to be implemented) ...

    // TEMP: print outputs
    std::cout << "Graph:\n" << graph.print() << std::endl;
    std::cout << "Priorities:\n";
    for (const auto& [id, priority] : priorities) {
        std::cout << "Node " << id << ": Priority " << priority << std::endl;
    }
}

DependenceGraph Scheduler::buildDependenceGraph(const InternalRepresentation& rep) {
    
    // Build dependence graph
    DependenceGraph graph;
    std::unordered_map<int, int> defs;
    int lastStore = -1;
    int lastOutput = -1;

    // For each operation
    for (Operation op : rep.operations) {

        // Create a node
        int node = graph.addNode(op);

        // For each name defined by this operation
        Operand& o = op.op3;
        if (op.opcode != Opcode::STORE && o.VR != -1) {
            
            // Add node to defs
            defs[o.VR] = node;
        }
        
        // Gather uses
        std::vector<Operand*> uses;
        switch (op.opcode) {
            case Opcode::LOAD:
                uses.push_back(&op.op1);
                break;
            case Opcode::STORE:
                uses.push_back(&op.op1);
                uses.push_back(&op.op3);
                break;
            case Opcode::ADD:
            case Opcode::SUB:
            case Opcode::MULT:
            case Opcode::LSHIFT:
            case Opcode::RSHIFT:
                uses.push_back(&op.op1);
                uses.push_back(&op.op2);
                break;
            default:
                break;
        }

        // For each name used by this operation
        for (Operand* o: uses) {
            
            // If use is undefined
            if (defs.find(o->VR) == defs.end()) {
                defs[o->VR] = graph.getUndefined();
            }

            // Add edge to graph
            graph.addEdge(node, defs[o->VR], Latency[(int) op.opcode]);
        }

        // Add conflict edges for loads
        if (op.opcode == Opcode::LOAD) {
            if (lastStore != -1) { 
                graph.addEdge(node, lastStore, Latency[(int) op.opcode]);
            }
        }

        // Add conflict and serialization edges for outputs
        if (op.opcode == Opcode::OUTPUT) {
            if (lastStore != -1) { 
                graph.addEdge(node, lastStore, Latency[(int) op.opcode]);
            } 
            if (lastOutput != -1) { 
                graph.addEdge(node, lastOutput, 1);
            }
        }

        // Add serialization edges for stores
        if (op.opcode == Opcode::STORE) {
            if (lastStore != -1) {
                graph.addEdge(node, lastStore, 1);
            }

            // Add edges to all previous loads and outputs
            for (const auto& [id, n] : graph.nodes) {
                if (id != graph.getUndefined() && id != node
                    && (n->data.opcode == Opcode::LOAD || n->data.opcode == Opcode::OUTPUT)) {
                    graph.addEdge(node, id, 1);
                }
            }
        }

        // Update last store/output
        if (op.opcode == Opcode::STORE) {
            lastStore = node;
        } else if (op.opcode == Opcode::OUTPUT) {
            lastOutput = node;
        }
    }

    return graph;
}

std::unordered_map<int, int> Scheduler::getPriorities(DependenceGraph& graph) {

    // Get topological order of nodes in dependence graph
    std::unordered_map<int, int> in_degree;
    for (const auto& [id, node] : graph.nodes) {
        in_degree[id] = 0;
    }
    for (const auto& [id, node] : graph.nodes) {
        for (const auto& edge : node->edges) {
            in_degree[edge.to]++;
        }
    }

    std::queue<int> queue = std::queue<int>();
    for (const auto& [id, degree] : in_degree) {
        if (degree == 0 && id != graph.getUndefined()) {
            queue.push(id);
        }
    }

    std::vector<int> topological_order = std::vector<int>();
    while (!queue.empty()) {
        int node_id = queue.front();
        queue.pop();
        topological_order.push_back(node_id);
        for (const auto& edge : graph.nodes[node_id]->edges) {
            in_degree[edge.to]--;
            if (in_degree[edge.to] == 0) {
                queue.push(edge.to);
            }
        }
    }

    // Initialize priorities
    std::unordered_map<int, int> priorities;
    for (const auto& [id, node] : graph.nodes) {
        if (id == graph.getUndefined()) {
            continue;
        } else if (in_degree[id] == 0) {
            priorities[id] = 0;
        } else {
            priorities[id] = -1;
        }
    }

    // Compute priorities as maximum latency-weighted distance
    for (int id: topological_order) {
        for (const auto& edge : graph.nodes[id]->edges) {
            priorities[edge.to] = std::max(priorities[edge.to], priorities[id] + edge.weight);
        }
    }

    return priorities;
}