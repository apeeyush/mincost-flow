#include <iostream>
#include <limits.h>
#include <list>
#include <algorithm>
#include <vector>
#include <iomanip>
#include <string>
#include <sstream>
#include <queue>

using namespace std;

struct Edge{
    int destination;
    int capacity;
    int residualFlow;
    int cost;
    Edge* counterEdge;
};

struct Graph{
    int numVertices;
    vector<Edge*> *adj;
};

Graph g;
Graph resGraph;

Edge* genEdge(int destination, int capacity, int residualFlow, int cost){
    Edge* e1 = new Edge;
    e1->destination = destination;
    e1->capacity = capacity;
    e1->residualFlow = residualFlow;
    e1->cost = cost;
    return e1;
}

int printGraph(Graph g){
    for(int i=0; i<g.numVertices; i++){
        for(int j=0; j<g.adj[i].size(); j++){
            cout << i+1 << " " << g.adj[i][j]->destination+1 << " " << (g.adj[i][j]->capacity - g.adj[i][j]->residualFlow) << " " << g.adj[i][j]->cost << endl;
        }
    }
    return 0;
}

bool BF(Graph resGraph, int source, int sink, int parentVertex[], Edge* parentEdge[]){
    // Initialize variables that will be needed
    int numVertices = resGraph.numVertices;
    vector<Edge*> *adj = resGraph.adj;
    int distance[numVertices];
    // Initialize visited, parentVertex and distance
    for(int i=0; i<numVertices; i++){
        parentVertex[i] = -1;
        distance[i] = INT_MAX;
    }
    // BF
    distance[source] = 0;
    for(int i=0; i<numVertices-1; i++){
        // loop on all edges
        for(int u=0; u<numVertices; u++){
            for(int e=0; e<adj[u].size(); e++){
                if(adj[u][e]->residualFlow > 0){
                    int v = adj[u][e]->destination;
                    int w = adj[u][e]->cost;
                    if(distance[v]>distance[u]+w){
                        distance[v] = distance[u]+w;
                        parentVertex[v] = u;
                        parentEdge[v] = adj[u][e];
                    }
                }
            }
        }
    }
    if(parentVertex[sink] == -1){
        return false;
    }else{
        return true;
    }
}


int calcMinCostFlow(int s, int t, int requiredFlow){
    int u, v, currFlow = 0;
    Edge *te1, *te2;
    int parent[g.numVertices];
    Edge* parentEdge[g.numVertices];
    while (BF(resGraph, s, t, parent, parentEdge)){
        int path_flow = INT_MAX;
        for (v=t; v!=s; v=parent[v]){
            u = parent[v];
            te1 = parentEdge[v];
            path_flow = min(path_flow, te1->residualFlow);
        }
        path_flow = min(path_flow, requiredFlow-currFlow);
        for (v=t; v != s; v=parent[v]){
            u = parent[v];
            te1 = parentEdge[v];
            te2 = te1->counterEdge;
            te1->residualFlow -= path_flow;
            te2->residualFlow += path_flow;
        }
        currFlow += path_flow;
        if(currFlow == requiredFlow){
            break;
        }
    }
    return currFlow;
}

int main(){
    int numVertices, numEdges, source, destination, flow;
    int tu, tv, tcap, tcost;

    cin >> numVertices >> numEdges;
    cin >> flow;
    cin >> source >> destination;
    source--; destination--;                // as indexing starts from 0

    // Initialize the graphs
    g.numVertices = numVertices;
    g.adj = new vector<Edge*>[numVertices];
    resGraph.numVertices = numVertices;
    resGraph.adj = new vector<Edge*>[numVertices];

    // Input graph parameters
    for(int i=0; i<numEdges; i++){
        cin >> tu >> tv >> tcap >> tcost;
        tu--; tv--;                         // as indexing starts from 0

        // Add edge to graph and edge and counter-edge to residual graph
        Edge* tmpEdge1 = genEdge(tv, tcap, tcap, tcost);
        Edge* tmpEdge2 = genEdge(tu, tcap, 0, -1*tcost);
        tmpEdge1->counterEdge = tmpEdge2;
        tmpEdge2->counterEdge = tmpEdge1;
        g.adj[tu].push_back(tmpEdge1);
        resGraph.adj[tu].push_back(tmpEdge1);
        resGraph.adj[tv].push_back(tmpEdge2);
    }

    cout << "Initial Edges:\n";
    printGraph(g);
    calcMinCostFlow(source, destination, flow);
    cout << "Final Edges:\n";
    printGraph(g);
    return 0;
}
