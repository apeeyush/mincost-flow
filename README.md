# Minimum Cost Flow

An implementation of minimum cost flow algorithm.


## Implementation Idea

    Transform network G to a residual network and add source and sink
    Detect and remove negative cycles using Bellman Ford if there is some flow in the network
    Successive Shortest Path:
      while ( Gx contains a path from s to t and the required flow is not obtained ) do
        Find any shortest path P from s to t using Bellman Ford
        Augment flow along P
        update Gx

## Input Format

First line contains two integers denoting the number of vertices and the number of edges in the network.
Next line contains an integer denoting the additional flow required from source to sink.
Next line contains two integers denoting the source and sink in the network.
This is followed by multiple lines (equal to the number of edges to be specific) with each line describing an edge. Each edge is described as contains 5 integers denoting `s`, `t`, `capacity`, `cost` and `current flow`.
