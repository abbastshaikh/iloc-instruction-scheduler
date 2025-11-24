#include <Scheduler.hpp>
#include <DependenceGraph.hpp>
#include <Operation.hpp>
#include <unordered_map>
#include <unordered_set>
#include <iostream>
#include <queue>
#include <vector>

Schedule Scheduler::schedule(InternalRepresentation& rep) {

    // Construct dependence graph
    DependenceGraph graph = buildDependenceGraph(rep);

    // Compute priorities using maximum latency-weighted path
    std::unordered_map<int, int> priorities = getPriorities(graph);

    // // TEMP: print outputs
    // std::cout << "Graph:\n" << graph.print() << std::endl;
    // std::cout << "Priorities:\n";
    // for (const auto& [id, priority] : priorities) {
    //     std::cout << "Node " << id << ": Priority " << priority << std::endl;
    // }

    int cycle = 1;
    std::unordered_set<int> active;
    std::unordered_set<int> completed;
    OperationPriorityQueue ready;

    // Add leaves to ready queue
    for (const auto& [id, node] : graph.nodes) {
        if (node->outEdges.empty() && id != graph.getUndefined()) {
            ready.push({id, priorities[id]});
        }
    }

    // Schedule operations based on priorities
    std::unordered_map<int, int> scheduledCycle;
    Schedule schedule;
    while (!ready.empty() || !active.empty()) {

        // // TEMP: print current state
        // std::cout << "Cycle " << cycle << std::endl;
        // std::cout << "Active Set: ";
        // for (int id : active) {
        //     std::cout << id << " ";
        // }
        // std::cout << std::endl;
        // std::cout << "Ready Queue: ";
        // OperationPriorityQueue tempReady = ready;
        // while (!tempReady.empty()) {
        //     OperationPriority op = tempReady.top();
        //     tempReady.pop();
        //     std::cout << op.id << "(" << op.priority << ") ";
        // }
        // std::cout << (ready.empty() ? "empty" : "");
        // std::cout << std::endl;
        // std::cout << "Scheduled Cycles:" << std::endl;
        // for (std::pair<Operation, Operation> c : schedule.cycles) {
        //     std::cout << c.first.printVR() << " | " << c.second.printVR() << std::endl;
        // }
        // std::cout << std::endl;

        // Pick an operation for each functional unit
        int f0 = -1, f1 = -1;
        bool output = false;
        std::list<int> defer;

        while (f0 == -1 || f1 == -1) {

            // If no ready operations, break
            if (ready.empty()) {
                break;
            }

            // Get the highest priority operation
            OperationPriority op = ready.top();
            ready.pop();

            Opcode opcode = graph.nodes[op.id]->data.opcode;

            // Use functional unit f0 for LOAD and STORE
            if (opcode == Opcode::LOAD || opcode == Opcode::STORE) {
                if (f0 == -1) {
                    f0 = op.id;   
                } else if (f1 == -1 && graph.nodes[f0]->data.opcode != Opcode::LOAD && graph.nodes[f0]->data.opcode != Opcode::STORE){
                    f1 = f0;
                    f0 = op.id;
                } else {
                    defer.push_back(op.id);
                }
            } 
            
            // Use functional unit f1 for MULT
            else if (opcode == Opcode::MULT) {
                if (f1 == -1) {
                    f1 = op.id;
                } else if (f0 == -1 && graph.nodes[f1]->data.opcode != Opcode::MULT) {
                    f0 = f1;
                    f1 = op.id;
                } else {
                    defer.push_back(op.id);
                }
            } 
            
            // Only OUTPUT per cycle
            else if (opcode == Opcode::OUTPUT) {
                if (!output) {
                    if (f0 == -1) f0 = op.id;
                    else if (f1 == -1) f1 = op.id;
                    output = true;
                } else {
                    defer.push_back(op.id);
                }
            } 
            
            // Use any available functional unit for other operations
            else {
                if (f0 == -1) f0 = op.id;
                else if (f1 == -1) f1 = op.id;
                else defer.push_back(op.id);
            }
        }

        // Reinsert deferred operations
        for (int id : defer) {
           ready.push({id, priorities[id]});
        }

        // Insert operations into active set and schedule
        Operation op0, op1;
        if (f0 != -1) { 
            scheduledCycle[f0] = cycle;
            active.insert(f0);
            op0 = graph.nodes[f0]->data;
        } else {
            op0 = {Opcode::NOP, {}, {}, {}};
        }

        if (f1 != -1) { 
            scheduledCycle[f1] = cycle; 
            active.insert(f1); 
            op1 = graph.nodes[f1]->data;
        } else {
            op1 = {Opcode::NOP, {}, {}, {}};
        }

        // Add scheduled operations to output list
        schedule.cycles.push_back({op0, op1});

        // Next cycle        
        cycle++;

        // For each active operation
        for (auto it = active.begin(); it != active.end(); ) {
            int id = *it;
            Opcode opcode = graph.nodes[id]->data.opcode;

            // If the operation has completed:
            if (scheduledCycle[id] + Latency[(int) opcode] <= cycle) {

                // Remove it from the active set
                it = active.erase(it);
                completed.insert(id);

                // For each operation dependent on this one
                for (const auto& inEdge : graph.nodes[id]->inEdges) {

                    // If the operation has not yet been scheduled
                    if (completed.find(inEdge.to) == completed.end() && active.find(inEdge.to) == active.end()) {

                        // If all dependencies are satisfied, add to ready queue
                        bool isReady = true;
                        for (const auto& outEdge : graph.nodes[inEdge.to]->outEdges) {
                            if (completed.find(outEdge.to) == completed.end()) {
                                isReady = false;
                                break;
                            }
                        }
                        if (isReady) {
                            ready.push({inEdge.to, priorities[inEdge.to]});
                        }
                    }
                }  
            } 

            // The operation is an active multi-cycle operation
            else {

                // Next iteration
                ++it;
            }
        }

        // For each multi-cycle operation
        
    }

    return schedule;
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

                    // Don't add duplicate edges
                    bool hasEdge = false;
                    for (const auto& inEdge : n->inEdges) {
                        if (inEdge.to == node) {
                            hasEdge = true;
                            break;
                        }
                    }

                    if (!hasEdge) {
                        graph.addEdge(node, id, 1);
                    }
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
        in_degree[id] = node->inEdges.size();
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
        for (const auto& edge : graph.nodes[node_id]->outEdges) {
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
        for (const auto& edge : graph.nodes[id]->outEdges) {
            priorities[edge.to] = std::max(priorities[edge.to], priorities[id] + edge.weight);
        }
    }

    return priorities;
}