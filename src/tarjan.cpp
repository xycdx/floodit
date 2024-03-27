#include <tarjan.h>

void Tarjan::tarjan(State &state, Graph &graph) {

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
            id[t]=dcc_cnt;
            if(t==u)
                break;
        }
    }
}