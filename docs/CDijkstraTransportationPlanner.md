# CDijkstraTransportationPlanner
This class implements the CTransportationPlanner abstract interface. It utilizes COpenStreetMap, CCSVBusSystem, CBusSystemIndexer, and CDijkstraPathRouter to find the shortest and fastest paths across a street map with multiple modes of transportation. It constructs three separate path graphs: one weighted by distance for shortest paths, one weighted by time for pure bike trips, and one weighted by time for walk and bus trips.

Before the constructor, the class defines a variety of variables, including:

DConfiguration, which holds the shared pointer to the configuration object containing the street map, bus system, and speed settings

DSortedNodes, a vector of street map nodes sorted by ID

DShortestPathRouter, DFastestBikeRouter, and DFastestBusRouter, which are type CDijkstraPathRouter graphs with edges weighted by distance in miles for shortest and time in hours for fastest (BikeRouter assumes only walk/bike and BusRouter assuems only walk/bus)

DEdgeStreetName, which maps each directed node pair to its street name

DEdgeSpeedLimit, which maps each directed node pair to its speed limit

And SEdgePairHash and SNodeIDPairHash, which are hash functions using a prime number (1000003) to minimize collisions.

## Constructor

### CDijkstraTransportationPlanner(std::shared_ptr<SConfiguration> config);
The constructor accepts a shared pointer to an SConfiguration object. It builds two separate fastest path graphs, because we cannot take a bike path if we use the bus. The bike router contains bike edges that are limited by one-way restrictions. The bus router contains both walk edges, which are limited by one-way restrictions, and bus edges which are limited by the road speed limit.

## Destructor

### ~CDijkstraTransportationPlanner();
The destructor for CDijkstraTransportationPlanner. Empty as cleanup is handled by the unique_ptr.

## Function Implementation
The class defines various inherited functions from the CTransportationPlanner class, which are implemented in DijkstraTransportationPlanner.cpp. See CTransportationPlanner.md for descriptions of the functions.

### std::size_t NodeCount() const noexcept override;
Returns DSortedNodes.size(), which is the number of nodes from the street map.

### std::shared_ptr<CStreetMap::SNode> SortedNodeByIndex(std::size_t index) const noexcept override;
Checks if index is greater than or equal to StopCount() and returns nullptr if so. Otherwise returns DSortedNodes[index].

### double FindShortestPath(TNodeID src, TNodeID dest, std::vector<TNodeID> &path) override;
The function first checks if both src and dest exist in DShortestNodeToVertex, and returns CPathRouter::NoPathExists if either is missing. Then, it translates both node IDs to vertex IDs and calls DShortestPathRouter.FindShortestPath. If no path is found, it returns CPathRouter::NoPathExists. Otherwise, the function iterates over the returned vertex path and uses GetVertexTag to convert each vertex ID back to a node ID, filling the path vector. Returns the total distance in miles.

### double FindFastestPath(TNodeID src, TNodeID dest, std::vector<TTripStep> &path) override;
The function first checks if both src and dest exist in DFastestNodeToVertex, and returns CPathRouter::NoPathExists if either is missing.  Then, it translates both node IDs to vertex IDs and calls the FindShortestPath() function to DFastestBikeRouter and DFastestBusRouter. It compares the times of the two, and chooses the faster path. Then, it uses a for loop to go through each vertex of the path, and reconstructs the map by looking up the mode transportation for each edge in the corresponding map (DFastestBikeEdgeModes if the bike route was faster, or DFastestBusEdgeModes if the bus route was faster). 


### bool GetPathDescription(const std::vector<TTripStep> &path, std::vector<std::string> &desc) const override;
The function first clears desc, then eturns false if path is empty or the starting node is invalid. Then, it pushes a string of "Start at " and the coordinates of a node's location. 

For bus segments, the function collects all consecutive bus steps to find the full board and exit nodes. Then, it looks up the corresponding bus stops via DBusSystemIndexer.StopByNodeID. Then, it serarches all routes to find the one that contains the board and exit segment in order and continues furthest past the exit stop. If multiple routes are taken, the bus with the earliest sorted name is taken. Finally, it pushes a "Take Bus (route) from stop (id) to stop (id)" string.

For walk/bike segments, the function groups consecutive steps of the same mode and street name together. Then, it calculates the total group distance and the bearing from the group start to end using CalculateBearing and BearingToDirection. If the street is named, the function pushes "Walk/Bike (dir) along name for (dist). If unnamed, it looks ahead for the next named street and pushes "Walk/Bike (dir) toward (name) for (dist)". If Distance below 0.1 mile, it is shown in feet, otherwise in miles with 2 significant figures.
 





