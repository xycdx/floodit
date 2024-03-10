#include <graph.h>

void Graph::mergeNode(int u, int v) {
    for(auto t : g[v]) {
        if(t != u) {
            if(find(g[u].begin(), g[u].end(), t) == g[u].end()) {
                g[u].push_back(t);
            }
            g[t].erase(
                find(g[t].begin(), g[t].end(), v)
            );
            if(find(g[t].begin(), g[t].end(), u) == g[t].end()) {
                g[t].push_back(u);
            }
            
        }
    }
    g[u].erase(
        find(g[u].begin(), g[u].end(), v)
    );
    g[v].clear();
    remainingNode--;
    nodeColor[v] = nodeColor[u];
}

bool Graph::flood(color_t last, color_t next) {
    // assert(next < colorCount);
    
    // auto neighbors = g[startNode];
    // int res = 0;
    // bool canFloodBefore = true;
    // for(int v : neighbors) {
    //     if(nodeColor[v] == next) {
    //         res++;
    //         mergeNode(startNode, v);
    //     }
    // }
    // moves.push_back(next);
    // if(res == 0) {
    //     return false;
    // }
    // return true;

    assert(next < colorCount);
    
    auto neighbors = g[startNode];
    int res = 0;
    bool canFloodBefore = true;
    for(int v : neighbors) {
        if(nodeColor[v] == next) {
            res++;
            mergeNode(startNode, v);
        }
    }
    moves.push_back(next);
    if(res == 0) {
        return false;
    }


    return true;

}

void Graph::calculate() {
    print();
    vector<bool> visited(nodeCount, 0);
	vector<unsigned> current, next;
    vector<int> colorCounts(colorCount, 0);

    current.push_back(startNode);
    visited[startNode] = 1;
    for (int i = 0; i < nodeCount; i++) {
		if (g[i].size() != 0 && i != startNode) {
			colorCounts[nodeColor[i]]++;
		}
	}

    int numExposedColors = 0;
	int minMovesLeft = 0;

    vector<int> colorCountsOld;

    while (!current.empty()) {
		if (numExposedColors > 0) {
			minMovesLeft += numExposedColors;
			numExposedColors = 0;
			colorCountsOld = colorCounts;
			for (auto node : current) {
				if (colorCountsOld[nodeColor[node]] == 0) {
					for (auto neighbor : g[node]) {
						if (!visited[neighbor]) {
							next.push_back(neighbor);
							visited[neighbor] = 1;
							if (--colorCounts[nodeColor[neighbor]] == 0) {
                                ++numExposedColors;
                            }
						}
					}
				}
				else {
                    next.push_back(node);
                }
			}
		}
		else {
			++minMovesLeft;
			for (auto node : current) {
				for (auto neighbor : g[node]) {
					if (!visited[neighbor]) {
						next.push_back(neighbor);
						visited[neighbor] = 1;
						if (--colorCounts[nodeColor[neighbor]] == 0) {
                            ++numExposedColors;
                        }
					}
				}
			}
		}
		swap(current, next);
		next.clear();
	}

	heuristicValue = moves.size() + minMovesLeft;
    cout<<minMovesLeft<<" "<<moves.size()<<endl;
    cout<<"-----------"<<endl;
    // vector<int> vis(nodeCount, 0);
    // int val = -1;
    // queue<int> q;
    // q.push(startNode);
    // vis[startNode] = 1;

    // while(!q.empty()) {
    //     val++;
    //     int sz = q.size();
    //     for(int i = 0; i < sz; i++) {
    //         int u = q.front();
    //         q.pop();
    //         for(auto v : g[u]) {
    //             if(!vis[v]) {
    //                 vis[v] = 1;
    //                 q.push(v);
    //             }
    //         }
    //     }
    // }
    
}

void Graph::print() {
    cout<<startNode<<endl;
    for(int i=0;i<nodeCount;i++) {
        cout<<i<<": ";
        for(int u:g[i]) {
            cout<<u<<" ";
        }
        cout<<endl;
    }
    cout<<"color:"<<endl;
    for(int i = 0; i < nodeCount; i++) {
        cout<<i<<" "<<nodeColor[i]<<endl;
    }
    cout<<"moves:";
    for(int i:moves) {
        cout<<i<<" ";
    }
    cout<<endl;
    cout<<"--------------"<<endl;
}
