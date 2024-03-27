#pragma once
#include <queue>
#include <set>
#include <unordered_set>
#include <unordered_map>
#include <unionfind.h>
#include <functional>
#include <atomic>
#include <tuple>
#include <stack>

#include <graph.h>

using namespace std;

struct State {
    State(const Graph& graph) {
        heuristicValue = 0;
        remainingNode = graph.nodeCount;
        colorNumber = graph.colorCount;
        isFilled.resize(remainingNode, false);
        isFilled[graph.startNode] = 1;
    }

    bool isComplete() { return remainingNode == 1; }

    void calculate_1(Graph& graph);

    void calculate_2(Graph& graph);

    void calculate_3(Graph& graph);

    bool operator<(const State& b) const { return moves.size() + heuristicValue > b.moves.size() + b.heuristicValue; }

    bool flood(const Graph& graph, color_t next);
    void print() {
        cout<<remainingNode<<endl;
        for(int i:isFilled) {
            cout<<i<<" ";
        } 
        cout<<endl;
    }
    int heuristicValue;
    int remainingNode;
    int colorNumber;
    vector<color_t> moves;
    vector<bool> isFilled;
};

struct Tarjan {
    struct edge {
        int u, v;
    };
    vector<edge> edges;
    vector<vector<int>> g;
    vector<int> dfn, low, is_bridge, id;
    stack<int> stk;
    int n, dcc_cnt, timestamp;

    void tarjan(State &state, Graph &graph);

    void tarjan_(int u, int from);
};


void buildgraph(int row, int col, vector<vector<int>> &data, Graph &graph);

tuple<vector<color_t>, int> solvebyBFS(int row, int col, vector<vector<int>> data, atomic<bool>& timeoutOccurred);

tuple<vector<color_t>, int> solvebyAstar(int row, int col, vector<vector<int>> data, atomic<bool>& timeoutOccurred);

tuple<vector<color_t>, int> solvebyDFS(int row, int col, vector<vector<int>> data, atomic<bool>& timeoutOccurred);

tuple<vector<color_t>, int> solvebyIDAstar(int row, int col, vector<vector<int>> data, atomic<bool>& timeoutOccurred);

void printmoves(vector<vector<int>> data, vector<int> seq, int row, int col);

struct Trie {
    int col = 2;
    vector<vector<int>> nex;
    int cnt = 0;
    vector<int> exist;
    int num = 0;

    void insert(vector<bool>& state, int val) {
        int p = 0;
        for (int i = 0; i < state.size(); i++) {
            int c = state[i];
            while(p >= nex.size()) {
                nex.emplace_back(vector<int>(col, 0));
            }
            if (!nex[p][c]) {
                nex[p][c] = ++cnt;
            }
            p = nex[p][c];
        }
        if(exist.size() <= p) {
            exist.resize(p + 1, 0);
        }
        if(!exist[p]) {
            num++;
        }
        exist[p] = val;
    }

    int find(vector<bool>& state) {
        int p = 0;
        for (int i = 0; i < state.size(); i++) {
            int c = state[i];
            if(p >= nex.size()){
                return 0;
            }
            if (!nex[p][c]) {
                return 0;
            }
            p = nex[p][c];
        }
        return exist[p];
    }

    void clear() {
        nex.clear();
        exist.clear();
        cnt = 0;
        num = 0;
    }
};