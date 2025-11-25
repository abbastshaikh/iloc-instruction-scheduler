#pragma once

#include <unordered_map>
#include <vector>
#include <memory>

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
        std::vector<Edge> inEdges;
        std::vector<Edge> outEdges;
        
        Node(int id, const T& data) : id(id), data(data) {}
    };

private:
    int nextId;
    
public:
    static constexpr int UNDEFINED_ID = -1;
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
    
    void addEdge(int from, int to, int weight) {
        nodes[from]->outEdges.emplace_back(to, weight);
        nodes[to]->inEdges.emplace_back(from, weight);
    }
};

