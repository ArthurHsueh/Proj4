# CTransportationPlanner
This abstract class defines the interface for a transportation planner. It provides a blueprint for any transportation planner implementation. It integrates with CStreetMap, CBusSystem, and CPathRouter to support multiple modes of transportation including walking, biking, and taking the bus. It also defines types like ETransportationMode, TTripStep, and TNodeID, which are used for the DijkstraTransportationPlanner and CTransportationPlannerCommandLine implementation.

## Typedefs

### TNodeID

Defined as CStreetMap::TNodeID. Used to identify nodes on the street map.

### ETransportationMode

An enum class with three values, Walk, Bike, and Bus. 

### TTripStep

Defined as std::pair<ETransportationMode, TNodeID>. It represents a step in a trip, expressing how you got to a node. The first element represents what transportation you used at the node (Walk Bike or Bus), and the second element is the node ID you are at.

## Struct SConfiguration
SConfiguration is an abstract or pure virtual struct that provides configuration values for the transportation planner. It has a virtual destructor and several pure virtual functions.

### virtual ~SConfiguration(){};
The destructor for SConfiguration.

### virtual std::shared_ptr<CStreetMap> StreetMap() const noexcept = 0;
Returns the street map used by the planner.

### virtual std::shared_ptr<CBusSystem> BusSystem() const noexcept = 0;
Returns the bus system used by the planner.

### virtual double WalkSpeed() const noexcept = 0;
Returns the walking speed in miles per hour. Default is 3 mph.

### virtual double BikeSpeed() const noexcept = 0;
Returns the biking speed in miles per hour. Default is 8 mph.

### virtual double DefaultSpeedLimit() const noexcept = 0;
Returns the default speed limit in miles per hour used when a road does not specify one. Default is 25 mph.

### virtual double BusStopTime() const noexcept = 0;
Returns the time in seconds spent at each bus stop when boarding.

### virtual int PrecomputeTime() const noexcept = 0;
Returns the maximum time in seconds allowed for precomputation before queries begin.

## Functions
### virtual ~CTransportationPlanner(){};
The virtual destructor for CTransportationPlanner.

### virtual std::size_t NodeCount() const noexcept = 0;
Returns the total number of nodes in the street map.

### virtual std::shared_ptr<CStreetMap::SNode> SortedNodeByIndex(std::size_t index) const noexcept = 0;
Returns the street map node at the given index. Nodes are sorted by their node ID (lowest to highest). Returns nullptr if the index is greater than or equal to NodeCount().

### virtual double FindShortestPath(TNodeID src, TNodeID dest, std::vector<TNodeID> &path) = 0;
Returns the distance in miles of the shortest path from src to dest. Fills path with the node IDs along the route. Returns CPathRouter::NoPathExists if no path exists.

### virtual double FindFastestPath(TNodeID src, TNodeID dest, std::vector<TTripStep> &path) = 0;
Returns the time in hours of the fastest path from src to dest. Fills path with TTripStep pairs, which indicate the mode of travel and node ID at each step. Returns CPathRouter::NoPathExists if no path exists.


### virtual bool GetPathDescription(const std::vector<TTripStep> &path, std::vector<std::string> &desc) const = 0;
Converts a vector of TTripStep into a human-readable vector of strings describing the trip. Returns true on success, and false if the path is invalid.

