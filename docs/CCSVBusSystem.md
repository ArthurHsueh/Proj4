# CCSVBusSystem
This class implements the CBusSystem abstract interface. It parses through bus stop and route data from two Comma Separated Value (CSV) files. The routes CSV file provides a list of routes under the format route,stop_id. The stops CSV file provides a list of stop IDs along with their node IDs under the format of stop_id,node_id. 

The class follows the PIMPL idiom implementation. It defines a struct SImplementation, which is implemented in CSVBusSystem.cpp, and a smart pointer DImplementation to hide the internal details of SImplementation. 

## Functions
The class defines various inherited functions from the CBusSystem class, which are implemented in CSVBusSystem.cpp.

See CBusSystem for descriptions of StopCount(), RouteCount, StopByIndex, StopByID, RouteByIndex, and RouteByName.

## Constructor

#### CCSVBusSystem(std::shared_ptr<CDSVReader> stopsrc, std::shared_ptr<CDSVReader> routesrc);
The constructor accepts two CDSVReader objects stopsrc and routesrc for stops and for routes respectively. The stops CSV file must have stop_id and node_id columns. The routes CSV file must have route and stop_id columns. The constructor parses through both files and stores the data through the functions ReadStops and ReadRoutes into DStopByIndex, DStopsByID, DRoutesByIndex, and DRoutesByName. 

## Destructor

#### ~CCSVBusSystem();
The destructor for the CCSVBusSystem. It's empty as all cleanup is handled by the property of the unique_ptr.

## Special Cases
If the stops CSV file or routes CSV file is missing any headers, such as stop_id, node_id, or route, the system will stop parsing. 

If a duplicate stop ID is found in the stops CSV, the system will stop parsing.

If a route references a stop ID that doesn't exist in the stops CSV, the system will stop parsing.

Data loaded before an error will be kept. For example, if three of five routes were loaded in, and the system finds an error in the fourth route, it will keep the three routes and will not load the fourth or fifth.

## Usage/Examples
The system can be used through the CBusSystem interface to access stops and routes. See CBusSystem.md for examples.