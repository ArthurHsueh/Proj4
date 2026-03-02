#include "CSVBusSystem.h"
#include <unordered_map>

struct CCSVBusSystem::SImplementation{

    struct SStop: public CBusSystem::SStop{  //represents a single bus stop
        TStopID DID; //stores the stop id used for routes
        CStreetMap::TNodeID DNodeID; //stores the stop id that corresponds to a node on the map

        ~SStop(){

        }

        // Returns the stop id of the stop
        TStopID ID() const noexcept{
            return DID;
        }

        // Returns the node id of the bus stop        
        CStreetMap::TNodeID NodeID() const noexcept{
            return DNodeID;
        }
    };

    struct SRoute: public CBusSystem::SRoute{
        std::string DName; //stores the route name
        std::vector<TStopID> DStopIDs; //stores the ids of the stops on the route
        ~SRoute(){

        }

        // Returns the name of the route
        std::string Name() const noexcept{
            return DName;
        }
        // Returns the number of stops on the route
        std::size_t StopCount() const noexcept{
            return DStopIDs.size();
        }

        // Returns the stop id specified by the index, returns InvalidStopID if index
        // is greater than or equal to StopCount()
        TStopID GetStopID(std::size_t index) const noexcept{
            if (index >= StopCount()){
                return CBusSystem::InvalidStopID;
            }
            return DStopIDs[index];
        }
    };

    const std::string STOP_ID_HEADER    = "stop_id";
    const std::string NODE_ID_HEADER    = "node_id";
    const std::string ROUTE_HEADER      = "route";

    std::vector< std::shared_ptr< SStop > > DStopsByIndex; //stores stops in a vector by index
    std::unordered_map< TStopID, std::shared_ptr< SStop > > DStopsByID; //stores stops in a map by stop id
    std::vector< std::shared_ptr< SRoute > > DRoutesByIndex; //stores routes in a vector by index
    std::unordered_map< std::string, std::shared_ptr< SRoute > > DRoutesByName; //stores routes in a map by name

    bool ReadStops(std::shared_ptr< CDSVReader > stopsrc){
        std::vector<std::string> TempRow;

        if(stopsrc->ReadRow(TempRow)){ //read header row
            size_t StopColumn = -1;
            size_t NodeColumn = -1;
            for(size_t Index = 0; Index < TempRow.size(); Index++){ //find which column is stop id and which is node id
                if(TempRow[Index] == STOP_ID_HEADER){
                    StopColumn = Index;
                }
                else if(TempRow[Index] == NODE_ID_HEADER){
                    NodeColumn = Index;
                }
            }
            if(StopColumn == -1 || NodeColumn == -1){ //return false if neither column was found
                return false;
            }
            while(stopsrc->ReadRow(TempRow)){ //read the rest of the rows
                TStopID StopID = std::stoull(TempRow[StopColumn]);
                CStreetMap::TNodeID NodeID = std::stoull(TempRow[NodeColumn]);

                if(DStopsByID.find(StopID) != DStopsByID.end()){ //duplicate stop id check
                    return false; //don't load any more stops
                }

                auto NewStop = std::make_shared< SStop >(); //create new stop, add it to the vector and map
                NewStop->DID = StopID;
                NewStop->DNodeID = NodeID;
                DStopsByIndex.push_back(NewStop);
                DStopsByID[StopID] = NewStop;
            }

            return true;
        }
        return false; //no stop data will be loaded
    }

    bool ReadRoutes(std::shared_ptr< CDSVReader > routesrc){
        std::vector<std::string> TempRow;

        if(routesrc->ReadRow(TempRow)){ //read header row
            size_t NameColumn = -1;
            size_t StopIDsColumn = -1;
            for(size_t Index = 0; Index < TempRow.size(); Index++){ //find which column is route name and which is stop ids
                if(TempRow[Index] == ROUTE_HEADER){
                    NameColumn = Index;
                }
                else if(TempRow[Index] == STOP_ID_HEADER){
                    StopIDsColumn = Index;
                }
            }
            if(NameColumn == -1 || StopIDsColumn == -1){ //return false if neither column was found
                return false;
            }
            while(routesrc->ReadRow(TempRow)){ //read the rest of the rows
                std::string RouteName = TempRow[NameColumn];
                TStopID StopID = std::stoull(TempRow[StopIDsColumn]);
                
                if(DStopsByID.find(StopID) == DStopsByID.end()){ //route references stop that doesn't exist
                    return false;
                }
                
                if(DRoutesByName.find(RouteName) == DRoutesByName.end()){ //if route doesn't exist, create it
                    auto NewRoute = std::make_shared<SRoute>();
                    NewRoute->DName = RouteName;
                    DRoutesByIndex.push_back(NewRoute);
                    DRoutesByName[RouteName] = NewRoute;
                }
                //add the stop id to the route
                DRoutesByName[RouteName]->DStopIDs.push_back(StopID);
            }
            return true;
        }
        return false; //no route data will be loaded
    }

    SImplementation(std::shared_ptr< CDSVReader > stopsrc, std::shared_ptr< CDSVReader > routesrc){
        if(ReadStops(stopsrc)){
            
        }
        if(ReadRoutes(routesrc)){

        }
    }

    // Returns the number of stops in the system
    std::size_t StopCount() const noexcept{
        return DStopsByIndex.size();
    }

    // Returns the number of routes in the system
    std::size_t RouteCount() const noexcept{
        return DRoutesByIndex.size();
    }

    // Returns the SStop specified by the index, nullptr is returned if index is
    // greater than equal to StopCount()
    // Differs from StopByID as this reads the whole system for a stop, while GetStopID only looks within one route
    std::shared_ptr<SStop> StopByIndex(std::size_t index) const noexcept{
        if (index >= StopCount()){
            return nullptr;
        }
        return DStopsByIndex[index];
    }

    // Returns the SStop specified by the stop id, nullptr is returned if id is
    // not in the stops
    std::shared_ptr<SStop> StopByID(TStopID id) const noexcept{
        auto key_value_pair = DStopsByID.find(id); //searches by key (DID) through the stop id in the whole map/system
        if (key_value_pair == DStopsByID.end()){ //if reached end of map 
            return nullptr;
        }
        return key_value_pair->second; //returns the value (SStop id) of the key value pair
    }

    // Returns the SRoute specified by the index, nullptr is returned if index is
    // greater than equal to RouteCount()
    std::shared_ptr<SRoute> RouteByIndex(std::size_t index) const noexcept{
        if (index >= RouteCount()){
            return nullptr;
        }
        return DRoutesByIndex[index];
    }

    // Returns the SRoute specified by the name, nullptr is returned if name is
    // not in the routes
    std::shared_ptr<SRoute> RouteByName(const std::string &name) const noexcept{
        auto key_value_pair = DRoutesByName.find(name); //searches by key (DName) through the route name in the whole map/system
        if (key_value_pair == DRoutesByName.end()){ //if reached end of map 
            return nullptr;
        }
        return key_value_pair->second; //returns the value (SRoute id) of the key value pair
    }


};

// Constructor for the CSV Bus System
// takes in a DSV reader for the stops and a DSV reader for the routes
CCSVBusSystem::CCSVBusSystem(std::shared_ptr< CDSVReader > stopsrc, std::shared_ptr< CDSVReader > routesrc){
    DImplementation = std::make_unique<SImplementation>(stopsrc,routesrc);
}

// Destructor for the CSV Bus System
CCSVBusSystem::~CCSVBusSystem(){ //empty because of unique_ptr

}

std::size_t CCSVBusSystem::StopCount() const noexcept{
    return DImplementation->StopCount();
}

std::size_t CCSVBusSystem::RouteCount() const noexcept{
    return DImplementation->RouteCount();
}

std::shared_ptr<CBusSystem::SStop> CCSVBusSystem::StopByIndex(std::size_t index) const noexcept{
    return DImplementation->StopByIndex(index);
}

std::shared_ptr<CBusSystem::SStop> CCSVBusSystem::StopByID(TStopID id) const noexcept{
    return DImplementation->StopByID(id);
}

std::shared_ptr<CBusSystem::SRoute> CCSVBusSystem::RouteByIndex(std::size_t index) const noexcept{
    return DImplementation->RouteByIndex(index);
}

std::shared_ptr<CBusSystem::SRoute> CCSVBusSystem::RouteByName(const std::string &name) const noexcept{
    return DImplementation->RouteByName(name);
}