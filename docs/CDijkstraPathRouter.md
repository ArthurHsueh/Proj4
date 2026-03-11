# CDijkstraPathRouter

This class implements a Transportation Planner that stores a graph and provides various functions for constructing nodes and edges, retrieving nodes and their tags, and finding the shortest path between them. It inherits from PathRouter.h, an abstract class that provides the interface for implementing and accessing the graph. 

## Constructor

#### **CDijkstraPathRouter::CDijkstraPathRouter()**

The router accepts no parameters in its constructor and creates the object directly

## Functions

#### **std::size_t VertexCount() const noexcept**
This function returns the number of vertices stored in the graph. It does this by storing each vertex in a vector and returning the size of this vector.

#### **TVertexID AddVertex(std::any tag) noexcept**
This function accepts a tag of type std::any, assigning that tag to a new vertex. It then assigns it an ID based on the number of vertices currently stored in the vector and adds the vertex to the vector.

#### **std::any GetVertexTag(TVertexID id) const noexcept**
This function accepts an Id of type TVertexID, which is defined to be of size_t. It first checks if the ID is valid, returning en empty std::any if it's not. Else, it returns the tag indexed by the corresponding id.

#### **bool AddEdge(TVertexID src, TVertexID dest, double weight, bool bidir = false) noexcept**
This functions accepts two node IDs, a weight of type double, and a boolean value indicating whether the edge is biconditional. It first checks if the ID's are valid. It then creates a weighted edge between the two nodes, storing it in a vector. It returns true on success and false otherwise.

#### **bool Precompute(std::chrono::steady_clock::time_point deadline) noexcept**
This functions does some precomputation to make the program running faster, promising to do so under the given deadline parameter. Upon success, it should return true

#### **double FindShortestPath(TVertexID src, TVertexID dest, std::vector<TVertexID> &path) noexcept**
This function accepts two node IDs and a vector of node IDs that construct a path. It then runs Dijkstra's Algorithm, creating two internal vectors to store the distances and parents of each node as well as a priority queue to retrieve the next unvisited node with the shorest distance in O(log n) time (popping and heapifying). It then iterates until the queue is empty, popping the node with the shortest distance. Because the implementation is lazy (rater than updating the value within the queue, we just push another entry in), we check if the popped node's distance is actually smaller than the one currently stored in the distance vector; if not, we continue. It then relaxes its neighboring nodes, updating their distances and parent if the new  path is shorter. It then pushes a new entry into the queue, repeating until the queue is empty. If the dest node is popped early on the traversal, we can break and end early. Finally, it reconstructs the path by traversing backwards through the parent array, starting from dest and looping until we find src. The function returns the distance index by dest, the shortest distance from src to dest.
