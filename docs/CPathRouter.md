# CPathRouter

This abstract class provides the interface for implementing the CDijkstaPathRouter. It defines functions that allow us retrieve informations about nodes, construct graphs by creating edges, and find the shortest path in the graph.

## Constructor

This abstract class has no explicit construcotr

## Functions

#### **virtual std::size_t VertexCount() const noexcept = 0;**
This function returns the number of vertices stored in the graph.  

#### **virtual TVertexID AddVertex(std::any tag) noexcept = 0;**
This function accepts a tag of type std::any, assigning that tag to a new node. It would then store that node internally, allowing the program to retrieve it later on. 

#### **virtual std::any GetVertexTag(TVertexID id) const noexcept = 0;**
This function accepts an id of type TVertexID, which is defined to be of size_t. It retrieves the internally stored node and returns it corresponding tag.

#### **virtual bool AddEdge(TVertexID src, TVertexID dest, double weight, bool bidir = false) noexcept = 0;**
This functions accepts two node IDs, a weight of type double, and a boolean value indicating whether the edge is biconditional. It constructs the edge and its weight and stores it internally, creating another edge in the other direction if bidir is marked true.

#### **virtual bool Precompute(std::chrono::steady_clock::time_point deadline) noexcept = 0;**
This functions does some precomputation to make the program running faster, promising to do so under the given deadline parameter. Upon success, it should return true

#### **virtual double FindShortestPath(TVertexID src, TVertexID dest, std::vector<TVertexID> &path) noexcept = 0;**
This function accepts two node IDs and a vector of node IDs that construct a path. Within the graph its stored, it finds the shortest path between the two given node, returning the corresponding distance/weight as its output. 