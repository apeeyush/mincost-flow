#include <iostream>
#include <list>
#include <algorithm>
#include <vector>
#include <iomanip>
#include <string>
#include <sstream>
#include <queue>

using namespace std;

int INT_MAX = 1000000;

// An edge is represented as a struct
// Fields:
//      destination   -  denotes the ending node of an edge. For example, 'v' in u-->v
//      capacity      -  the maximum capacity of an edge
//      residualFlow  -  the residual amount of flow that can flow through the edge
//      counterEdge   -  a pointer to the counter edge in residual graph for performance optimization
struct Edge{
    int destination;
    int capacity;
    int residualFlow;
    int cost;
    Edge* counterEdge;
};

// A graph is represented as a struct
// Fields:
//      numVertices - denotes the number of vertices in the graph
//      adj         - Adjacency list : Collection of unordered lists one for each vertex
struct Graph{
    int numVertices;
    vector<Edge*> *adj;
};

Graph g;
Graph resGraph;

// Generates a new edge (allocating space dynamically) and returns a pointed to the edge
Edge* genEdge(int destination, int capacity, int residualFlow, int cost){
    Edge* e1 = new Edge;
    e1->destination = destination;
    e1->capacity = capacity;
    e1->residualFlow = residualFlow;
    e1->cost = cost;
    return e1;
}

// Prints all the edges in the graph
// Output:
//      List of edges where each edge is represented by
//          u(start node)   v(end node)   flow   capacity
int printGraph(Graph g){
    for(int i=0; i<g.numVertices; i++){
        for(int j=0; j<g.adj[i].size(); j++){
            cout << i+1 << " " << g.adj[i][j]->destination+1 << " " << (g.adj[i][j]->capacity - g.adj[i][j]->residualFlow) << " " << g.adj[i][j]->cost << endl;
        }
    }
    return 0;
}

int printParams(int parentVertex[], int distance[]){
    cout << "Parents Vertex:\n";
    for(int i=0; i<g.numVertices; i++){
        cout << parentVertex[i] << " ";
    }
    cout << endl;
    cout << "Distance Vertex:\n";
    for(int i=0; i<g.numVertices; i++){
        cout << distance[i] << " ";
    }
    cout << endl;
}

// Detects the presence of negative cycles in graph
// Output:
//      -1          if no negative cycles present
//      node_num    index of a node in negative cycle
// Lists parentVertex and parentEdge are updated and can be used to reconstruct the negative cycle
int BFCycleDetection(Graph resGraph, int source, int parentVertex[], Edge* parentEdge[]){
    // Initialize variables that will be needed
    int cycle_node = -1;
    int numVertices = resGraph.numVertices;
    vector<Edge*> *adj = resGraph.adj;
    int distance[numVertices];
    // Initialize visited, parentVertex and distance
    for(int i=0; i<numVertices; i++){
        parentVertex[i] = -1;
        distance[i] = INT_MAX;
    }
    // BF - Relax edges repeatedly
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
    // Check for negative weight cycle - loop on all edges
    for(int u=0; u<numVertices; u++){
        for(int e=0; e<adj[u].size(); e++){
            if(adj[u][e]->residualFlow > 0){
                int v = adj[u][e]->destination;
                int w = adj[u][e]->cost;
                if(distance[v]>distance[u]+w){
                    return v;       // Negative cycle detected!
                }
            }
        }
    }
    return cycle_node;
}

// Cancels all negative cycles
// Output:
//      costSaved       amount of cost saved by cycle detection and cancellation
int cancelNegativeCycles(Graph resGraph){
    int costSaved=0, cyclePossible=1, u, v;
    Edge *te1, *te2;
    int numVertices = resGraph.numVertices;
    while(cyclePossible){
        cyclePossible=0;
        for(int i=0; i<numVertices; i++){
            int parent[g.numVertices];
            Edge* parentEdge[g.numVertices];
            int node_num = BFCycleDetection(resGraph, i, parent, parentEdge);
            if(node_num!=-1){               // A cycle is detected
                cyclePossible=1;
                // Calculate path flow
                int path_flow = INT_MAX;
                v=node_num; u = parent[v]; te1 = parentEdge[v];
                path_flow = min(path_flow, te1->residualFlow);
                for (v=u; v!=node_num; v=parent[v]){
                    u = parent[v];
                    te1 = parentEdge[v];
                    path_flow = min(path_flow, te1->residualFlow);
                }
                // Update graph by removing the cycle
                v=node_num; u = parent[v];
                te1 = parentEdge[v];
                te2 = te1->counterEdge;
                te1->residualFlow -= path_flow;
                te2->residualFlow += path_flow;
                costSaved += path_flow*(te1->cost);
                for (v=u; v != node_num; v=parent[v]){
                    u = parent[v];
                    te1 = parentEdge[v];
                    te2 = te1->counterEdge;
                    te1->residualFlow -= path_flow;
                    te2->residualFlow += path_flow;
                    costSaved += path_flow*(te1->cost);
                }
            }
        }
    }
    return -1*costSaved;
}


// Finds the shortest path from source to sink
// Output:
//      0           if no path exists from source to sink
//      1           if there is a path from source to sink
// Lists parentVertex and parentEdge are updated and can be used to reconstruct the shortest path
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
    // printParams(parentVertex, distance);
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
        // printParams(parentVertex, distance);
    }
    if(parentVertex[sink] == -1){
        return false;
    }else{
        return true;
    }
}

// Calculates the cost of flow 'requiredFlow' from 's' to 't'
// Returns 'INT_MAX' if such a flow is not possible
int calcMinCostFlow(int s, int t, int requiredFlow){
    int u, v, currFlow=0, runningCost=0;
    Edge *te1, *te2;
    // Detect negative cycles and remove
    int benifit = cancelNegativeCycles(resGraph);
    if(benifit){
        cout << "Negative cycle detected and removed. Resulting cost benifit is " << benifit << endl;
    }
    // Run shortest path augmentation
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
            runningCost += path_flow*(te1->cost);
        }
        currFlow += path_flow;
        if(currFlow == requiredFlow){
            break;
        }
    }
    if(currFlow == requiredFlow){
        return runningCost;
    }else{
        return INT_MAX;
    }
}


int main(){
    int numVertices, numEdges, source, destination, flow;
    int tu, tv, tcap, tcost, tflow;

    // Scan the graph and question configurations
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
        cin >> tu >> tv >> tcap >> tcost >> tflow;
        tu--; tv--;                         // as indexing starts from 0
        // Add edge to graph and edge and counter-edge to residual graph
        Edge* tmpEdge1 = genEdge(tv, tcap, tcap-tflow, tcost);
        Edge* tmpEdge2 = genEdge(tu, tcap, tflow, -1*tcost);
        tmpEdge1->counterEdge = tmpEdge2;
        tmpEdge2->counterEdge = tmpEdge1;
        g.adj[tu].push_back(tmpEdge1);
        resGraph.adj[tu].push_back(tmpEdge1);
        resGraph.adj[tv].push_back(tmpEdge2);
    }

    // Print initial state of graph
    cout << "Initial Graph:\n";
    printGraph(g);
    //cout << "hello\n";

    // Run min-cost flow algorithm
    int mincost = calcMinCostFlow(source, destination, flow);

    // Print final state of graph
    if(mincost == INT_MAX){
        cout << "The given flow is not possible!\n";
    }else{
        cout << "The minimum cost for the additional flow is " << mincost << endl;
        cout << "Final Graph:\n";
        printGraph(g);
    }
    return 0;
}
