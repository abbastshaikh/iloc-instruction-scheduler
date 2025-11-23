#pragma once

#include <Operation.hpp>
#include <unordered_map>
#include <vector>
#include <memory>
#include <sstream>

template<typename T>
class Graph {
public:
    struct Edge {
        int to;
        int weight;
        
        Edge(int to, int weight) : to(to), weight(weight) {}
    };

    struct Node {
        int id;
        T data;
        std::vector<Edge> edges;
        
        Node(int id, const T& data) : id(id), data(data) {}
    };

private:
    int nextId;
    static constexpr int UNDEFINED_ID = -1;

public:
    std::unordered_map<int, std::unique_ptr<Node>> nodes;
    
    Graph() : nextId(1) {
        nodes[UNDEFINED_ID] = std::make_unique<Node>(UNDEFINED_ID, T());
    }

    int getUndefined() const {
        return UNDEFINED_ID;
    }

    int addNode(const T& data) {
        int id = nextId++;
        nodes[id] = std::make_unique<Node>(id, data);
        return id;
    }
    
    bool addEdge(int from, int to, int weight) {
        if (nodes.find(from) == nodes.end() || nodes.find(to) == nodes.end()) {
            return false;
        }
        nodes[from]->edges.emplace_back(to, weight);
        return true;
    }

    std::string print() const {
        std::ostringstream oss;
        for (const auto& [id, node] : nodes) {
            if (id == UNDEFINED_ID) continue;
            oss << "Node " << id << ": " << node->data.printVR() << "\n";
            for (const auto& edge : node->edges) {
                oss << "  -> Node " << edge.to << " (weight " << edge.weight << ")\n";
            }
        }
        return oss.str();
    }
};

class DependenceGraph : public Graph<Operation> {};