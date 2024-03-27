#include <solver.h>

const vector<pair<int, int>> direction = {{1, 0}, {0, 1},{-1, 0},{0, -1}};

    
void buildgraph(int row, int col, vector<vector<int>> &data, Graph &graph) {
    vector<vector<int>> vis(row, vector<int>(col, 0));
    UninFind uninfind(row * col);
    auto checkBorder = [&](pair<int, int> x) {
        return x.first >= 0 && x.first < row && x.second >=0 && x.second < col;
    };
    auto toIndex = [&](pair<int, int> x) {
        return x.first * row + x.second;
    };
    auto reduce = [&](int x, int y) {
        queue<pair<int, int>> q;
        q.push({x, y});
        vis[x][y] = 1;
        int color = data[x][y];

        int floodCount = 0;
        while(!q.empty()) {
            auto t = q.front();
            q.pop();
            floodCount++;
            vis[t.first][t.second] = 1;
            for(auto d : direction) {
                pair<int, int> neighbor = {t.first + d.first, t.second + d.second};
                if(checkBorder(neighbor) &&
                    !vis[neighbor.first][neighbor.second] &&
                    color == data[neighbor.first][neighbor.second]) {
                        uninfind.merge(toIndex(t), toIndex(neighbor));
                        q.push(neighbor);
                }
            }
        }
        return floodCount;
    };

    set<int> colors;
    map<pair<int, int>, int> nodeSize;
    for(int i = 0; i < row; i++) {
        for(int j = 0; j < col; j++) {
            colors.insert(data[i][j]);
            if(!vis[i][j]) {
                graph.nodeCount++;
                nodeSize[{i, j}] = reduce(i, j);
            }
        }
    }
    graph.g.resize(graph.nodeCount);
    graph.nodeColor.resize(graph.nodeCount, 0);
    graph.colorCount = colors.size();
    graph.remainingNode = graph.nodeCount;

    set<int> nodeIndex;
    map<int,int> indexMap;
    for(int i = 0; i < row; i++) {
        for(int j = 0; j < col; j++) {
            nodeIndex.insert(uninfind.find(toIndex({i, j})));
        }
    }

    auto it = nodeIndex.begin();
    for(size_t i = 0; i < nodeIndex.size(); i++, it++) {
        indexMap[*it] = i;
    }

    for(int i = 0; i < row; i++) {
        for(int j = 0; j < col; j++) {
            int nodeId = indexMap[uninfind.find(toIndex({i, j}))];
            graph.nodeColor[nodeId] = data[i][j];
        }
    }

    graph.startNode = indexMap[uninfind.find(toIndex({0, 0}))];

    map<pair<int, int>, bool> isEdge;
    for(int i = 0; i < row; i++) {
        for(int j = 0; j < col; j++) {
            for(auto d : direction) {
                pair<int, int> neighbor = {i + d.first, j + d.second};
                if(checkBorder(neighbor) &&
                    data[i][j] != data[neighbor.first][neighbor.second]) {
                    auto u = uninfind.find(toIndex({i, j}));
                    auto v = uninfind.find(toIndex(neighbor));
                    if(!isEdge[{u, v}] && !isEdge[{v, u}]) {
                        graph.addEdge(indexMap[u], indexMap[v]);
                        isEdge[{u, v}] = 1;
                        isEdge[{v, u}] = 1;
                    }
                }
            }
        }
    }
}

bool State::flood(const Graph& graph, color_t next) {
    
    bool canFill = false;
    vector<bool> nextFill(isFilled.size(), false);

    for(size_t i = 0; i < isFilled.size(); i++) {
        if(isFilled[i]) {
            for(int u : graph.g[i]) {
                if(!isFilled[u] && graph.nodeColor[u] == next) {
                    nextFill[u] = true;
                    canFill = true;
                }
            }
        }
    }
    for(size_t i = 0; i < isFilled.size(); i++) {
        if(nextFill[i]) {
            isFilled[i] = true;
            remainingNode--;
        }
    }
    if(!canFill) {
        return false; 
    }
    moves.push_back(next);
    assert(remainingNode != 0);
    // calculate(graph);
    return true;
}

void State::calculate_2(Graph& graph) {
	vector<bool> visited = isFilled;
	vector<unsigned> current, next;
	current.reserve(isFilled.size());
	next.reserve(isFilled.size());

	vector<color_t> colorCounts(graph.nodeCount, 0);
	for (unsigned index = 0; index < isFilled.size(); ++index) {
		if (isFilled[index]) {
			current.push_back(index);
		} else {
            colorCounts[graph.nodeColor[index]]++;
        }
	}
    vector<bool> exposedColors(graph.colorCount, 0);
	unsigned minMovesLeft = 0;

	while (!current.empty()) {
        vector<bool> visitedBackUp = visited;
        vector<unsigned> colorCountsBackUp = colorCounts;
        int numExposedColors = 0;
        bool canExposeColors = false;
        
        for (unsigned node : current) {
            for (unsigned neighbor : graph.g[node]) {
                if (!visitedBackUp[neighbor]) {
                    visitedBackUp[neighbor] = true;
                    if (--colorCountsBackUp[graph.nodeColor[neighbor]] == 0) {
                        numExposedColors++;
                        exposedColors[graph.nodeColor[neighbor]] = 1;
                        canExposeColors = true;
                    }
                }
            }
        }
		if (canExposeColors) {
			minMovesLeft += numExposedColors;
            next = current;
			for (unsigned node : current) {
                for (unsigned neighbor : graph.g[node]) {
                    if (!visited[neighbor] && exposedColors[graph.nodeColor[neighbor]]) {
                        colorCounts[graph.nodeColor[neighbor]]--;
                        next.push_back(neighbor);
                        visited[neighbor] = true;
                    }
                }
			}
		}
		else {
			for (unsigned node : current) {
				for (unsigned neighbor : graph.g[node]) {
					if (!visited[neighbor]) {
                        colorCounts[graph.nodeColor[neighbor]]--;
						next.push_back(neighbor);
						visited[neighbor] = true;
					}
				}
			}
            if(next.empty()) {
                break;
            }
            minMovesLeft++;
		}
        fill(exposedColors.begin(), exposedColors.end(), 0);
		swap(current, next);
		next.clear();
	}
	heuristicValue = minMovesLeft;
}

void State::calculate_1(Graph& graph) {
    vector<bool> visited = isFilled;
	vector<unsigned> current, next;

	for (unsigned index = 0; index < isFilled.size(); ++index) {
		if (isFilled[index]) {
			current.push_back(index);
		}
	}
	unsigned minMovesLeft = 0;

	while (true) {
        for (unsigned node : current) {
            for (unsigned neighbor : graph.g[node]) {
                if (!visited[neighbor]) {
                    next.push_back(neighbor);
                    visited[neighbor] = true;
                }
            }
        }
        if(next.empty()) {
            break;
        }
        swap(current, next);
        minMovesLeft++;
		next.clear();
	}
    // graph.print();
    // print();
    // cout<<"heuristicValue: "<<moves.size() + minMovesLeft<<endl;
	heuristicValue = minMovesLeft;
}

void Tarjan::tarjan(State &state, Graph &graph) {
    n = graph.nodeCount;
    dcc_cnt = 0;
    timestamp = 0;
    map<pair<int, int>, bool> isEdgeVisited;
    for(int i = 0; i < state.isFilled.size(); i++) {
        if(!state.isFilled[i]) {
            for(int v : graph.g[i]) {
                if(state.isFilled[v] && !isEdgeVisited[{i, graph.startNode}] && !isEdgeVisited[{graph.startNode, i}]) {
                    edges.push_back({i, graph.startNode});
                    edges.push_back({graph.startNode, i});
                    isEdgeVisited[{i, graph.startNode}] = isEdgeVisited[{graph.startNode, i}] = true;
                } 
                if(!state.isFilled[v] && !isEdgeVisited[{i, v}] && !isEdgeVisited[{v, i}]) {
                    edges.push_back({i, v});
                    edges.push_back({v, i});
                    isEdgeVisited[{i, v}] = isEdgeVisited[{v, i}] = true;
                }
            }
        }
    }
    g.resize(n);
    dfn.resize(n, 0);
    low.resize(n, 0);
    is_bridge.resize(edges.size(), 0);
    id.resize(n, 0);
    for(int i = 0; i < edges.size(); i+=2) {
        g[edges[i].u].push_back(i);
        g[edges[i].v].push_back(i + 1);
    }

    tarjan_(graph.startNode, -1);
}

void Tarjan::tarjan_(int u, int from)
{
    timestamp++;
    dfn[u] = timestamp;
    low[u] = timestamp;
    
    stk.push(u);
    for(int e:g[u])
    {
        int v=edges[e].v;
        if(!dfn[v])
        {
            tarjan_(v,e);
            low[u]=min(low[u],low[v]);
            
            if(dfn[u]<low[v])
            {
                is_bridge[e]=1;
                is_bridge[e^1]=1;
            }
        }
        else if(e!=(from^1))
            low[u]=min(low[u],dfn[v]);
    }
    if(dfn[u]==low[u])
    {
        dcc_cnt++;
        while(true)
        {
            int t=stk.top();
            stk.pop();
            id[t]=dcc_cnt-1;
            if(t==u)
                break;
        }
    }
}

void State::calculate_3(Graph& graph) {
    Tarjan T;
    // graph.print();
    T.tarjan(*this, graph);
    // for(int i = 0; i < T.is_bridge.size(); i++) {
    //     if(T.is_bridge[i]) {
    //         cout<<T.edges[i].u<<" "<<T.edges[i].v<<endl;
    //     }
    // }
    
    vector<vector<int>> tr(T.dcc_cnt);
    for(auto e : T.edges) {
        if(T.id[e.u] != T.id[e.v]) {
            tr[T.id[e.u]].push_back(T.id[e.v]);
            tr[T.id[e.v]].push_back(T.id[e.u]);
        }
    }

    vector<int> dcc_cost(T.dcc_cnt, 0), dcc_size(T.dcc_cnt, 0), startNodes;

    function<void(int, int)> dfs0 = [&] (int u, int fa) {
        startNodes.push_back(u);
        vector<bool> vis(graph.nodeCount, 0);
        vector<int> nxt;
        function<void(int)> dfs1 = [&] (int w) {
            vis[w] = 1;
            dcc_size[T.id[w]]++;
            for(auto v : graph.g[w]) {
                if(!vis[v]) {
                    if(T.id[w] != T.id[v]) {
                        nxt.push_back(v);
                    } else {
                        dfs1(v);
                    }
                }
            }
        };
        dfs1(u);
        for(auto v : nxt) {
            if(v != fa) {
                dfs0(v, u);
            }
        }
    };

    dfs0(graph.startNode, 0);

    function<void(int)> calculate = [&] (int u) {
        vector<bool> visited = isFilled;
        visited[u] = 1;
        vector<unsigned> current, next;
        int sz = dcc_size[T.id[u]], dcc_id = T.id[u];

        vector<color_t> colorCounts(sz, 0);
        for (unsigned index = 0; index < isFilled.size(); ++index) {
            if(T.id[index] == dcc_id) {
                if (isFilled[index]) {
                    current.push_back(index);
                } else {
                    colorCounts[graph.nodeColor[index]]++;
                }
            }
        }
        vector<bool> exposedColors(graph.colorCount, 0);
        unsigned minMovesLeft = 0;

        while (!current.empty()) {
            vector<bool> visitedBackUp = visited;
            vector<unsigned> colorCountsBackUp = colorCounts;
            int numExposedColors = 0;
            bool canExposeColors = false;
            
            for (unsigned node : current) {
                for (unsigned neighbor : graph.g[node]) {
                    if(T.id[neighbor] == dcc_id) {
                        if (!visitedBackUp[neighbor]) {
                            visitedBackUp[neighbor] = true;
                            if (--colorCountsBackUp[graph.nodeColor[neighbor]] == 0) {
                                numExposedColors++;
                                exposedColors[graph.nodeColor[neighbor]] = 1;
                                canExposeColors = true;
                            }
                        }
                    }
                }
            }
            if (canExposeColors) {
                minMovesLeft += numExposedColors;
                next = current;
                for (unsigned node : current) {
                    for (unsigned neighbor : graph.g[node]) {
                        if(T.id[neighbor] == dcc_id) {
                            if (!visited[neighbor] && exposedColors[graph.nodeColor[neighbor]]) {
                                colorCounts[graph.nodeColor[neighbor]]--;
                                next.push_back(neighbor);
                                visited[neighbor] = true;
                            }
                        }
                    }
                }
            }
            else {
                for (unsigned node : current) {
                    for (unsigned neighbor : graph.g[node]) {
                        if(T.id[neighbor] == dcc_id) {
                            if (!visited[neighbor]) {
                                colorCounts[graph.nodeColor[neighbor]]--;
                                next.push_back(neighbor);
                                visited[neighbor] = true;
                            }
                        }
                    }
                }
                if(next.empty()) {
                    break;
                }
                minMovesLeft++;
            }
            fill(exposedColors.begin(), exposedColors.end(), 0);
            swap(current, next);
            next.clear();
        }
        dcc_cost[dcc_id] = minMovesLeft;
    };

    for(auto i : startNodes) {
        calculate(i);
    }

    function<void(int, int)> dfs2 = [&] (int u, int fa) {
        dcc_cost[u] += dcc_cost[fa] + 1;
        for(int v : tr[u]) {
            if(v != fa) {
                dfs2(v, u);
            }
        }
    };

    dfs2(T.id[graph.startNode], 0);

    heuristicValue = 0;
    for(int e : dcc_cost) {
        heuristicValue = max(heuristicValue, e);
    }
}

tuple<vector<color_t>, int> solvebyBFS(int row, int col, vector<vector<int>> data, atomic<bool>& timeoutOccurred) {
    Graph graph;
    map<vector<bool>, int> vis;
    buildgraph(row, col, data, graph);
    
    queue<State> q;
    q.push(graph);
    vis[q.front().isFilled] = true;

    while(!q.empty()) {
        if(timeoutOccurred.load()) {
            break;
        }
        auto t = move(q.front());
        q.pop();
        // t.print();

        if(t.isComplete()) {
            return {t.moves, vis.size()};
        }
        for(int color = 0; color < graph.colorCount; color++) {
            State newState = t;
            if(newState.flood(graph, color) && !vis[newState.isFilled]) {
                vis[newState.isFilled] = true;
                q.push(move(newState));
            }
        }
    }
    return {};
}


tuple<vector<color_t>, int> solvebyAstar(int row, int col, vector<vector<int>> data, atomic<bool>& timeoutOccurred) {
    Graph graph;
    Trie vis;
    priority_queue<State> q;
    buildgraph(row, col, data, graph);
    State state(graph);
    state.calculate_3(graph);
    // q.push(state);

    // while(!q.empty()) {
    //     if(timeoutOccurred.load()) {
    //         break;
    //     }
    //     auto t = move(q.top());
    //     q.pop();
    //     if(t.isComplete()) {
    //         cout<<"number of states: "<<vis.num<<endl;
    //         return {t.moves, vis.num};
    //     }

    //     for(int color = 1; color <= t.colorNumber; color++) {
    //         State newState = t;
    //         if(newState.flood(graph, color) ) {
    //             int dst = vis.find(newState.isFilled);
    //             if(dst == 0 || dst > newState.moves.size()) {
    //                 vis.insert(newState.isFilled, newState.moves.size());
    //                 newState.calculate_2(graph);
    //                 q.push(move(newState));
    //             }
    //         }
    //     }
    // }
    return {};
}

tuple<vector<color_t>, int> solvebyDFS(int row, int col, vector<vector<int>> data, atomic<bool>& timeoutOccurred) {
    Graph graph;
    Trie vis;
    buildgraph(row, col, data, graph);
    
    size_t minMoves = 1e9;
    vector<color_t> res;
    function<void(State&)> dfs = [&](State& state) {
        if(timeoutOccurred) {
            return;
        }
        if(state.moves.size() > minMoves) {
            return;
        }
        if(state.isComplete()) {
            minMoves = state.moves.size();
            res = state.moves;
            return;
        }

        vector<int> neighborNumber(graph.colorCount, 0);
        for(int i = 0; i < state.isFilled.size(); i++) {
            if(state.isFilled[i]) {
                for(int u : graph.g[i]) {
                    if(!state.isFilled[u]) {
                        neighborNumber[graph.nodeColor[u]]++;
                    }
                }
            }
        }
        vector<pair<int, color_t>> colorToChoose;
        for(size_t i = 0; i < graph.colorCount; i++) {
            if(neighborNumber[i] != 0) {
                colorToChoose.push_back({neighborNumber[i], i});
            }
        }
        sort(colorToChoose.begin(), colorToChoose.end(), greater<pair<int, color_t>>());
        for(auto e : colorToChoose) {
            State newState = state;
            newState.flood(graph, e.second);
            int dst = vis.find(newState.isFilled);
            if(dst == 0 || dst > newState.moves.size()) {
                vis.insert(newState.isFilled, newState.moves.size());
                dfs(newState);
            }
        }
    };
    State state(graph);
    dfs(state);

    return {res, vis.num};
}

tuple<vector<color_t>, int> solvebyIDAstar(int row, int col, vector<vector<int>> data, atomic<bool>& timeoutOccurred) {
    Graph graph;
    Trie vis;
    buildgraph(row, col, data, graph);
    size_t minMoves = 1e9;
    vector<color_t> res;
    function<bool(State&, int)> dfs = [&](State& state, int dep) {
        if(timeoutOccurred) {
            return false;
        }
        if(state.heuristicValue + state.moves.size() > dep) {
            return false;
        }
        if(state.isComplete()) {
            minMoves = state.moves.size();
            res = state.moves;
            return true;
        }

        vector<int> neighborNumber(graph.colorCount + 1, 0);
        for(int i = 0; i < state.isFilled.size(); i++) {
            if(state.isFilled[i]) {
                for(int u : graph.g[i]) {
                    if(!state.isFilled[u]) {
                        neighborNumber[graph.nodeColor[u]]++;
                    }
                }
            }
        }
        vector<pair<int, color_t>> colorToChoose;
        for(size_t i = 1; i <= graph.colorCount; i++) {
            if(neighborNumber[i] != 0) {
                colorToChoose.push_back({neighborNumber[i], i});
            }
        }
        sort(colorToChoose.begin(), colorToChoose.end(), greater<pair<int, color_t>>());
        for(auto e : colorToChoose) {
            State newState = state;
            newState.flood(graph, e.second);
            int dst = vis.find(newState.isFilled);
            if(dst == 0 || dst > newState.moves.size()) {
                vis.insert(newState.isFilled, newState.moves.size());
                newState.calculate_2(graph);
                if(dfs(newState, dep)) {
                    return true;
                }
            }
        }
        return false;
    };
    State state(graph);
    state.calculate_2(graph);
    int dep = state.heuristicValue;
    
    while(!dfs(state, dep)) {
        if(timeoutOccurred) {
            break;
        }
        dep++;
        vis.clear();
    }

    return {res, vis.num};
}

void printmoves(vector<vector<int>> data, vector<int> seq, int row, int col) {
    auto bfs = [&] (int c) {
        int cs = data[0][0];
        queue<pair<int, int>> q;
        map<pair<int, int>, int> vis;
        q.push({0, 0});
        data[0][0] = c;
        vis[{0, 0}] = 1;
        while ((!q.empty())) {
            auto t = q.front();
            q.pop();
            for(auto d : direction) {
                int x = t.first + d.first, y = t.second + d.second;
                if(x >= 0 && x < row && y >= 0 && y < col && !vis[{x, y}] && data[x][y] == cs) {
                    vis[{x, y}] = 1;
                    data[x][y] = c;
                    q.push({x, y});
                }
            }
        }
    };

    for(auto i : seq) {
        bfs(i);
        for(int i = 0; i < row; i++) {
            for(int j = 0; j < col; j++) {
                cout << data[i][j] << " ";
            }
            cout << endl;
        }
        cout << endl;
    }
}