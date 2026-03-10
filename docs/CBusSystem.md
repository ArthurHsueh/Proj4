# CBusSystem

This class defines an abstract interface for a bus system. It provides a blueprint for the any bus system implementation, and defines the structure for stops and routes. A stop represents a physical bus stop, and is expressed with a node on the street map. A route represents an ordered sequence of stops that a bus stops through.

## Typedefs

#### TStopID

Defined as uint64_t type, which is a 64 bit unsigned integer. It is used for giving bus stops a unique identification.

#### InvalidStopID

A static constant of type TStopID, which is set to the maximum value of the 64 bit unsigned integer. It is used for returning invalid stop IDs.

## Struct SStop

SStop is an abstract struct that represents a single bus stop. It has a virtual destructor and two pure virtual functions. 

#### virtual ~SStop(){};
The destructor for SStop.

#### virtual TStopID ID() const noexcept = 0;
A pure virtual function that, when implemented, returns the Stop ID of the stop, which corresponds to a stop in routes.

#### virtual CStreetMap::TNodeID NodeID() const noexcept = 0; 
A pure virtual function that, when implemented, returns the node ID of the stop, which corresponds to a node on the street map.

## Struct SRoute

SRoute is an abstract struct that represents a single bus route. It has a virtual destructor and three pure virtual functions.

#### virtual ~SRoute(){};
The virtual destructor for SRoute.

#### virtual std::string Name() const noexcept = 0;
A pure virtual function that, when implemented, returns the name of the route.

#### virtual std::size_t StopCount() const noexcept = 0;
A pure virtual function that, when implemented, returns the number of stops in the route.

#### virtual TStopID GetStopID(std::size_t index) const noexcept = 0;
A pure virtual function that, when implemented, returns the stop ID at the given index. It also returns InvalidStopID if the index is greater than or equal to StopCount().

## Functions

#### virtual ~CBusSystem(){};
The virtual destructor for CBusSystem.

#### virtual std::size_t StopCount() const noexcept = 0;
A pure virtual function that, when implemented, returns the total number of stops.
#### virtual std::size_t RouteCount() const noexcept = 0;
A pure virtual function that, when implemented, returns the total number of routes.

#### virtual std::shared_ptr<SStop> StopByIndex(std::size_t index) const noexcept = 0;
A pure virtual function that, when implemented, returns the stop at the given index. Returns nullptr if the index is greater than or equal to StopCount().

#### virtual std::shared_ptr<SStop> StopByID(TStopID id) const noexcept = 0;
A pure virtual function that, when implemented, returns the stop with the given stop ID. Returns nullptr if the ID does not exist within the system.

#### virtual std::shared_ptr<SRoute> RouteByIndex(std::size_t index) const noexcept = 0;
A pure virtual function that, when implemented, returns the route at the given index. Returns nullptr if the index is greater than or equal to RouteCount().

#### virtual std::shared_ptr<SRoute> RouteByName(const std::string &name) const noexcept = 0;
A pure virtual function that, when implemented, returns a route with the given name. Returns nullptr if the name does not exist within the system.

## Usage/Examples
You can call StopCount() and RouteCount() to get the total number of stops and routes. 

Example:
std::size_t totalStops = BusSystem->StopCount();
std::size_t totalRoutes = BusSystem->RouteCount();

Use StopByIndex() or StopByID() with a known stop ID to access a specific stop. 

Example:

auto stop = BusSystem->StopByIndex(0);

auto stopByID = BusSystem->StopByID(1);

From a stop object, you can call ID() to get its stop ID and NodeID() to get its corresponding street map node. 

Example:

stop->ID();

stop->NodeID();

To access a route, use RouteByName() with the route's name or RouteByIndex() with an index. 

Example:

auto route = BusSystem->RouteByName("A");

auto routebyindex = BusSystem->RouteByIndex(0);

From a route object, you can call Name() for its name, StopCount() for the number of stops, and GetStopID() with an index to get each stop ID in order.

Example:

route->Name();      

route->StopCount(); 

route->GetStopID(0);