#include "BusSystemIndexer.h"
#include <vector>
#include <unordered_map>
#include <algorithm>

struct CBusSystemIndexer::SImplementation{
    std::shared_ptr<CBusSystem> DBusSystem; //the bus system being indexed
    std::vector<std::shared_ptr<SStop>> DSortedStopsByIndex; //stops sorted by ID for SortedStopByIndex
    std::unordered_map<TNodeID,std::shared_ptr<SStop>> DStopsByNodeID; //map from node ID to stop for StopByNodeID
    std::vector<std::shared_ptr<SRoute>> DSortedRoutesByIndex; //routes sorted by Name for SortedRouteByIndex
    
    struct SNodeIDPairHash{ //hash function to combine the node IDs of an edge for faster lookup
        std::size_t operator()(const std::pair<TNodeID,TNodeID> &pair) const{
            std::size_t First = pair.first;
            std::size_t Second = pair.second;
            return First ^ (Second<<1); //shift second left 1 bit, merge the two
        }
    };
    std::unordered_map<std::pair<TNodeID,TNodeID>,std::unordered_set<std::shared_ptr<SRoute>>,SNodeIDPairHash> DRoutesByNodeIDs;

    SImplementation(std::shared_ptr<CBusSystem> bussystem){
        DBusSystem = bussystem;
        for(size_t Index = 0; Index < DBusSystem->StopCount(); Index++){ //add each stop into DSortedStopsByIndex and DStopsByNodeID
            auto Stop = DBusSystem->StopByIndex(Index);
            DSortedStopsByIndex.push_back(Stop);
            DStopsByNodeID[Stop->NodeID()] = Stop;
        }
        // sort the stops by ID
        std::sort(DSortedStopsByIndex.begin(), DSortedStopsByIndex.end(), [](std::shared_ptr<SStop> l, std::shared_ptr<SStop> r) -> bool{
            return l->ID() < r->ID();
        });
        for(size_t Index = 0; Index < DBusSystem->RouteCount(); Index++){ //add each route into DSortedRoutesByIndex
            auto Route = DBusSystem->RouteByIndex(Index);
            DSortedRoutesByIndex.push_back(Route);
        }
        //sort the routes by Name
        std::sort(DSortedRoutesByIndex.begin(), DSortedRoutesByIndex.end(), [](std::shared_ptr<SRoute> l, std::shared_ptr<SRoute> r) -> bool{
            return l->Name() < r->Name();
        });
        for(auto Route: DSortedRoutesByIndex){ //get information for each route and store in DRoutesByNodeIDs
            for(size_t Index = 1; Index < Route->StopCount(); Index++){
                auto Previous = Route->GetStopID(Index-1);
                auto Current = Route->GetStopID(Index);
                auto FirstNodeID = DBusSystem->StopByID(Previous)->NodeID();
                auto SecondNodeID = DBusSystem->StopByID(Current)->NodeID();
                auto Key = std::make_pair(FirstNodeID,SecondNodeID);
                auto Search = DRoutesByNodeIDs.find(Key);
                if(Search == DRoutesByNodeIDs.end()){
                    DRoutesByNodeIDs[Key] = {Route};
                }
                else{
                    Search->second.insert(Route);
                }
            }
        }

    }

    ~SImplementation(){

    }

    std::size_t StopCount() const noexcept{
        return DBusSystem->StopCount();
    }

    std::size_t RouteCount() const noexcept{
        return DBusSystem->RouteCount();
    }

    std::shared_ptr<SStop> SortedStopByIndex(std::size_t index) const noexcept{
        if(index >= StopCount()){ //if index is greater than equal to stop count, return nullptr
            return nullptr;
        }
        return DSortedStopsByIndex[index];
    }

    std::shared_ptr<SRoute> SortedRouteByIndex(std::size_t index) const noexcept{
        if(index >= RouteCount()){ //if index is greater than equal to route count, return nullptr
            return nullptr;
        }
        return DSortedRoutesByIndex[index];
    }

    std::shared_ptr<SStop> StopByNodeID(TNodeID id) const noexcept{ 
        auto Search = DStopsByNodeID.find(id); 
        if(Search != DStopsByNodeID.end()){ //go through the stops to find the node id, if found return the stop, otherwise return nullptr
            return Search->second;
        }
        return nullptr;
    }

    bool RoutesByNodeIDs(TNodeID src, TNodeID dest, std::unordered_set<std::shared_ptr<SRoute> > &routes) const noexcept{
        auto Search = DRoutesByNodeIDs.find(std::make_pair(src,dest));
        if(Search != DRoutesByNodeIDs.end()){
            routes = Search->second;
            return true;
        }
        return false;
    }

    bool RouteBetweenNodeIDs(TNodeID src, TNodeID dest) const noexcept{
        std::unordered_set<std::shared_ptr<SRoute>> Routes;
        return RoutesByNodeIDs(src, dest, Routes);
    }

};

// Constructor for the Bus System Indexer
CBusSystemIndexer::CBusSystemIndexer(std::shared_ptr<CBusSystem> bussystem){
    DImplementation = std::make_unique<SImplementation>(bussystem);
}

// Destructor for the Bus System Indexer
CBusSystemIndexer::~CBusSystemIndexer(){

}

// Returns the number of stops in the CBusSystem being indexed
std::size_t CBusSystemIndexer::StopCount() const noexcept{
    return DImplementation->StopCount();
}

// Returns the number of routes in the CBusSystem being indexed
std::size_t CBusSystemIndexer::RouteCount() const noexcept{
    return DImplementation->RouteCount();
}

// Returns the SStop specified by the index where the stops are sorted by
// their ID, nullptr is returned if index is greater than equal to
// StopCount()
std::shared_ptr<CBusSystemIndexer::SStop> CBusSystemIndexer::SortedStopByIndex(std::size_t index) const noexcept{
    return DImplementation->SortedStopByIndex(index);

}

// Returns the SRoute specified by the index where the routes are sorted by
// their Name, nullptr is returned if index is greater than equal to
// RouteCount()
std::shared_ptr<CBusSystemIndexer::SRoute> CBusSystemIndexer::SortedRouteByIndex(std::size_t index) const noexcept{
    return DImplementation->SortedRouteByIndex(index);
}
    


// Returns the SStop associated with the specified node ID, nullptr is
// returned if no SStop associated with the node ID exists
std::shared_ptr<CBusSystemIndexer::SStop> CBusSystemIndexer::StopByNodeID(TNodeID id) const noexcept{
    return DImplementation->StopByNodeID(id);
}


// Returns true if at least one route exists between the stops at the src and
// dest node IDs. All routes that have a route segment between the stops at
// the src and dest nodes will be placed in routes unordered set.
bool CBusSystemIndexer::RoutesByNodeIDs(TNodeID src, TNodeID dest, std::unordered_set<std::shared_ptr<SRoute> > &routes) const noexcept{
    return DImplementation->RoutesByNodeIDs(src,dest,routes);
}

// Returns true if at least one route exists between the stops at the src and
// dest node IDs.
bool CBusSystemIndexer::RouteBetweenNodeIDs(TNodeID src, TNodeID dest) const noexcept{
    return DImplementation->RouteBetweenNodeIDs(src,dest);
}
