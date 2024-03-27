#pragma once
#include <graph.h>
#include <solver.h>
#include <vector>
#include <stack>
using namespace std;

struct Tarjan {
    struct edge {
        int u,v;
    };
    int n,m;
    vector<edge> edges;
    vector<vector<int>> g;
    int dcc_cnt;
    vector<int> dfn, low, timestamp, is_bridge, id, d;
    stack<int> stk;
    int timestamp;

    void tarjan(State &state, Graph &graph);

    void tarjan_(int u, int from);
};