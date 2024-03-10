#pragma once
#include <iostream>
#include <vector>
#include <algorithm>
#include <map>
#include <queue>
#include <assert.h>
using namespace std;

typedef unsigned int color_t;

class Graph {
public:

    Graph() { nodeCount = 0; }

    int getStartNode() { return startNode; }

    bool isComplete() { return remainingNode == 1; }

    void mergeNode(int u, int v);

    void addEdge(int u, int v) {
        g[u].push_back(v);
        g[v].push_back(u);
    }

    bool flood(color_t last, color_t next);

    bool operator<(const Graph& b) const { return moves.size() + heuristicValue > b.moves.size() + b.heuristicValue; }

    void calculate();

    void print();

    int nodeCount;
    int colorCount;
    int startNode;
    int heuristicValue;
    int remainingNode;
    vector<vector<int>> g;
    vector<int> moves;
    vector<color_t> nodeColor;
};