#pragma once
#include <vector>
using namespace std;

class UninFind {
public:
    UninFind() {}
    UninFind(int len) {
        fa.resize(len);
        for (int i = 0; i < len; i++) {
            fa[i] = i;
        }
    }

    int find(int x) {
        if(fa[x] == x) {
            return x;
        }
        return fa[x] = find(fa[x]);
    }
    

    void merge(int x, int y) { fa[find(x)] = find(y); }

private:
    vector<int> fa;
};