#include "DijkstraTransportationPlanner.h"
#include "DijkstraPathRouter.h"
#include "BusSystemIndexer.h"
#include "GeographicUtils.h"
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <format>

struct CDijkstraTransportationPlanner::SImplementation{
    std::shared_ptr<SConfiguration> DConfiguration; //holds the config (street map, bus system, speeds)
    std::vector<std::shared_ptr<CStreetMap::SNode>> DSortedNodes; //all street map nodes sorted by ID
    CDijkstraPathRouter DShortestPathRouter; //path router weighted by distance in miles
    CDijkstraPathRouter DFastestPathRouter; //path router weighted by time in hours
    std::unordered_map<TNodeID, CPathRouter::TVertexID> DShortestNodeToVertex; //maps OSM node ID -> vertex ID for shortest router
    std::unordered_map<TNodeID, CPathRouter::TVertexID> DFastestNodeToVertex; //maps OSM node ID -> vertex ID for fastest router
    CBusSystemIndexer DBusSystemIndexer; //indexes the bus system for easy stop/route lookup
    
    using ETransportationMode = CTransportationPlanner::ETransportationMode;
    using TTripStep = CTransportationPlanner::TTripStep;

    struct SEdgePairHash{ //hash for (vertexID, vertexID) pairs used in DFastestEdgeModes
        size_t operator()(const std::pair<CPathRouter::TVertexID, CPathRouter::TVertexID> &p) const{
            size_t First = p.first;
            size_t Second = p.second;
            return First * 1000003 + Second;
        }
    };

    struct SNodeIDPairHash{ //hash for (nodeID, nodeID) pairs used in DEdgeStreetName and DEdgeSpeedLimit
        size_t operator()(const std::pair<TNodeID, TNodeID> &p) const{
            size_t First = p.first;
            size_t Second = p.second;
            return First * 1000003 + Second;
        }
    };

    //when reconstructing the fastest path, we need to know which transport mode was used on each edge
    //bike edges overwrite walk for the same pair since bike is always faster on the same road
    std::unordered_map<std::pair<CPathRouter::TVertexID, CPathRouter::TVertexID>, ETransportationMode, SEdgePairHash> DFastestEdgeModes;
    std::unordered_map<std::pair<TNodeID, TNodeID>, std::string, SNodeIDPairHash> DEdgeStreetName; //street name for each directed road edge
    std::unordered_map<std::pair<TNodeID, TNodeID>, double, SNodeIDPairHash> DEdgeSpeedLimit; //speed limit for each directed road edge

    SImplementation(std::shared_ptr<SConfiguration> config) : DBusSystemIndexer(config->BusSystem()){
        DConfiguration = config;

        //get all nodes from the street map into DSortedNodes
        for(size_t Index = 0; Index < DConfiguration->StreetMap()->NodeCount(); Index++){
            DSortedNodes.push_back(DConfiguration->StreetMap()->NodeByIndex(Index));
        }
        //sort nodes by ID so SortedNodeByIndex works correctly
        std::sort(DSortedNodes.begin(), DSortedNodes.end(), [](std::shared_ptr<CStreetMap::SNode> l, std::shared_ptr<CStreetMap::SNode> r) -> bool{
            return l->ID() < r->ID();
        });

        //register every node as a vertex in both routers for CDijkstraPathRouter
        for(size_t Index = 0; Index < DSortedNodes.size(); Index++){
            auto Node = DSortedNodes[Index];
            DShortestNodeToVertex[Node->ID()] = DShortestPathRouter.AddVertex(Node->ID());
            DFastestNodeToVertex[Node->ID()] = DFastestPathRouter.AddVertex(Node->ID());
        }

        //build the shortest path graph with edges weighted by distance
        //also fill DEdgeStreetName and DEdgeSpeedLimit for use in bus timing and path description
        for(size_t WayIndex = 0; WayIndex < DConfiguration->StreetMap()->WayCount(); WayIndex++){ //for each way in the street map
            auto Way = DConfiguration->StreetMap()->WayByIndex(WayIndex); //grab each way

            //read rules of the way
            bool IsOneWay = Way->HasAttribute("oneway") && Way->GetAttribute("oneway") == "yes"; //one-way streets only allow forward travel
            std::string Name = ""; //default to no name if the way doesn't have one
            if(Way->HasAttribute("name")){
                Name = Way->GetAttribute("name");
            }
            double SpeedLimit = DConfiguration->DefaultSpeedLimit(); //use default of 25mph if not specified
            if(Way->HasAttribute("maxspeed")){
                SpeedLimit = std::stod(Way->GetAttribute("maxspeed"));
            }
            
            //iterate over the nodes inside the way
            for(size_t Index = 1; Index < Way->NodeCount(); Index++){
                auto FirstNodeID = Way->GetNodeID(Index-1); //get id of node, index-1 for the previous node
                auto SecondNodeID = Way->GetNodeID(Index); 
                
                auto FirstNode = DConfiguration->StreetMap()->NodeByID(FirstNodeID); //get the actual node by id
                auto SecondNode = DConfiguration->StreetMap()->NodeByID(SecondNodeID);

                //calculate distance between the 2 nodes, use haversine because the world is round!!
                auto Distance = SGeographicUtils::HaversineDistanceInMiles(FirstNode->Location(), SecondNode->Location());

                auto FirstVertexID = DShortestNodeToVertex[FirstNodeID];
                auto SecondVertexID = DShortestNodeToVertex[SecondNodeID];
                DShortestPathRouter.AddEdge(FirstVertexID, SecondVertexID, Distance); //forward edge always added
                DEdgeStreetName[{FirstNodeID, SecondNodeID}] = Name;
                DEdgeSpeedLimit[{FirstNodeID, SecondNodeID}] = SpeedLimit;
                if(!IsOneWay){ //two-way street, also add the reverse edge
                    DShortestPathRouter.AddEdge(SecondVertexID, FirstVertexID, Distance);
                    DEdgeStreetName[{SecondNodeID, FirstNodeID}] = Name;
                    DEdgeSpeedLimit[{SecondNodeID, FirstNodeID}] = SpeedLimit;
                }
            }
        }

        //build the fastest path graph with edges weighted by time (distance / speed)
        //walk edges always go both directions regardless of oneway
        //bike edges only go forward on one-way streets and are skipped if bicycle=no
        //bike overwrites walk in DFastestEdgeModes for the same edge since bike is always faster
        for(size_t WayIndex = 0; WayIndex < DConfiguration->StreetMap()->WayCount(); WayIndex++){
            auto Way = DConfiguration->StreetMap()->WayByIndex(WayIndex); //grab each way
            
            //read rules of the way
            bool IsOneWay = Way->HasAttribute("oneway") && Way->GetAttribute("oneway") == "yes"; //one-way streets only allow forward travel
            bool NoBike = Way->HasAttribute("bicycle") && Way->GetAttribute("bicycle") == "no"; //some ways explicitly ban bikes

            //iterate over the nodes inside the way
            for(size_t Index = 1; Index < Way->NodeCount(); Index++){
                auto FirstNodeID = Way->GetNodeID(Index-1); //get id of node, index-1 for the previous node 
                auto SecondNodeID = Way->GetNodeID(Index);

                auto FirstNode = DConfiguration->StreetMap()->NodeByID(FirstNodeID); //get the actual node by id
                auto SecondNode = DConfiguration->StreetMap()->NodeByID(SecondNodeID);

                //calculate distance between the 2 nodes, use haversine because the world is round
                auto Distance = SGeographicUtils::HaversineDistanceInMiles(FirstNode->Location(), SecondNode->Location());

                //path router uses vertex IDs so convert from node ID to vertex ID
                auto FirstVertexID = DFastestNodeToVertex[FirstNodeID];
                auto SecondVertexID = DFastestNodeToVertex[SecondNodeID];

                //convert distance to walking time
                double WalkTime = Distance / DConfiguration->WalkSpeed(); //divide by walk speed which is 3mph
                
                //add walk edges
                DFastestPathRouter.AddEdge(FirstVertexID, SecondVertexID, WalkTime); //walk forward
                DFastestEdgeModes[{FirstVertexID, SecondVertexID}] = ETransportationMode::Walk;
                DFastestPathRouter.AddEdge(SecondVertexID, FirstVertexID, WalkTime); //walk backward
                DFastestEdgeModes[{SecondVertexID, FirstVertexID}] = ETransportationMode::Walk;
                if(!NoBike){ //only add bike edges if biking is allowed on this road
                    double BikeTime = Distance / DConfiguration->BikeSpeed();
                    DFastestPathRouter.AddEdge(FirstVertexID, SecondVertexID, BikeTime); //bike is faster so it overwrites walk
                    DFastestEdgeModes[{FirstVertexID, SecondVertexID}] = ETransportationMode::Bike;
                    if(!IsOneWay){ //bikes also follow one-way restrictions
                        DFastestPathRouter.AddEdge(SecondVertexID, FirstVertexID, BikeTime);
                        DFastestEdgeModes[{SecondVertexID, FirstVertexID}] = ETransportationMode::Bike;
                    }
                }
            }
        }

        // Add bus edges to the fastest path graph between adjacent stops on each route
        // Bus time uses the actual road distance (via shortest path) and per-road speed limits
        // BusStopTime() is in seconds so divide by 3600 to convert to hours
        for(size_t RouteIndex = 0; RouteIndex < DConfiguration->BusSystem()->RouteCount(); RouteIndex++){
            auto Route = DConfiguration->BusSystem()->RouteByIndex(RouteIndex);
            for(size_t Index = 1; Index < Route->StopCount(); Index++){
                auto PrevStop = DConfiguration->BusSystem()->StopByID(Route->GetStopID(Index-1));
                auto CurrStop = DConfiguration->BusSystem()->StopByID(Route->GetStopID(Index));
                std::vector<CPathRouter::TVertexID> ShortestPath;
                double RoadDistance = DShortestPathRouter.FindShortestPath(
                    DShortestNodeToVertex[PrevStop->NodeID()],
                    DShortestNodeToVertex[CurrStop->NodeID()],
                    ShortestPath
                );
                if(RoadDistance == CPathRouter::NoPathExists){ //if no road connection between these stops, skip
                    continue;
                }

                // Sum time for each road segment using that road's actual speed limit
                double BusTime = 0;
                for(size_t k = 1; k < ShortestPath.size(); k++){
                    auto PrevNodeID = std::any_cast<TNodeID>(DShortestPathRouter.GetVertexTag(ShortestPath[k-1]));
                    auto CurrNodeID = std::any_cast<TNodeID>(DShortestPathRouter.GetVertexTag(ShortestPath[k]));
                    auto PrevNode = DConfiguration->StreetMap()->NodeByID(PrevNodeID);
                    auto CurrNode = DConfiguration->StreetMap()->NodeByID(CurrNodeID);
                    double SegDist = SGeographicUtils::HaversineDistanceInMiles(PrevNode->Location(), CurrNode->Location());
                    double SpeedLimit = DConfiguration->DefaultSpeedLimit(); //fallback if edge isn't in our map
                    if(DEdgeSpeedLimit.count({PrevNodeID, CurrNodeID})){
                        SpeedLimit = DEdgeSpeedLimit.at({PrevNodeID, CurrNodeID});
                    }
                    BusTime += SegDist / SpeedLimit;
                }
                auto PrevVertexID = DFastestNodeToVertex[PrevStop->NodeID()];
                auto CurrVertexID = DFastestNodeToVertex[CurrStop->NodeID()];
                BusTime += DConfiguration->BusStopTime() / 3600.0; // add bus stop time per edge
                DFastestPathRouter.AddEdge(PrevVertexID, CurrVertexID, BusTime);
                DFastestEdgeModes[{PrevVertexID, CurrVertexID}] = ETransportationMode::Bus;
            }
        }
    }

    ~SImplementation(){

    }

    std::size_t NodeCount() const noexcept{ //returns total number of street map nodes
        return DSortedNodes.size();
    }

    std::shared_ptr<CStreetMap::SNode> SortedNodeByIndex(std::size_t index) const noexcept{ //returns node at given index, nullptr if out of bounds
        if(index >= DSortedNodes.size()){
            return nullptr;
        }
        return DSortedNodes[index];
    }

    double FindShortestPath(TNodeID src, TNodeID dest, std::vector<TNodeID> &path){ //returns distance in miles, fills path with node IDs
        path.clear();
        if(!DShortestNodeToVertex.count(src) || !DShortestNodeToVertex.count(dest)){ //src or dest not in the map
            return CPathRouter::NoPathExists;
        }
        std::vector<CPathRouter::TVertexID> VertexPath;

        double Distance = DShortestPathRouter.FindShortestPath(DShortestNodeToVertex.at(src), DShortestNodeToVertex.at(dest), VertexPath);

        if(Distance == CPathRouter::NoPathExists){ //no path found
            return CPathRouter::NoPathExists;
        }

        for(auto VertexID : VertexPath){ //convert vertex IDs back to node IDs
            path.push_back(std::any_cast<TNodeID>(DShortestPathRouter.GetVertexTag(VertexID)));
        }
        return Distance;
    }

    double FindFastestPath(TNodeID src, TNodeID dest, std::vector<TTripStep> &path){ //returns time in hours, fills path with (mode, nodeID) pairs
        path.clear();
        if(!DFastestNodeToVertex.count(src) || !DFastestNodeToVertex.count(dest)){ //check if source or destination  not in the map
            return CPathRouter::NoPathExists;
        }

        std::vector<CPathRouter::TVertexID> VertexPath;
        double Time = DFastestPathRouter.FindShortestPath(DFastestNodeToVertex.at(src), DFastestNodeToVertex.at(dest), VertexPath);

        if(Time == CPathRouter::NoPathExists){ //no path found
            return CPathRouter::NoPathExists;
        }

        for(size_t i = 0; i < VertexPath.size(); i++){
            auto NodeID = std::any_cast<TNodeID>(DFastestPathRouter.GetVertexTag(VertexPath[i]));
            if(i == 0){ //if first step is the starting node
                //use the mode of the first edge, unless it's Bus
                //("You cannot take your bike on the bus, so if you take the bus you must walk to it")
                ETransportationMode FirstMode = ETransportationMode::Walk;
                if(VertexPath.size() > 1){
                    auto It = DFastestEdgeModes.find({VertexPath[0], VertexPath[1]});
                    if(It != DFastestEdgeModes.end() && It->second == ETransportationMode::Bike){
                        FirstMode = ETransportationMode::Bike;
                    }
                }
                path.push_back({FirstMode, NodeID});
            }
            else{ 
                auto ModeIt = DFastestEdgeModes.find({VertexPath[i-1], VertexPath[i]}); //Look up the mode for this edge
                auto Mode = ETransportationMode::Walk; //default to walk if edge not found
                
                if(ModeIt != DFastestEdgeModes.end()){
                    Mode = ModeIt->second;
                }
                path.push_back({Mode, NodeID});

            }
        }
        return Time;
    }

    bool GetPathDescription(const std::vector<TTripStep> &path, std::vector<std::string> &desc) const{ //converts trip steps into human-readable strings
        desc.clear();
        if(path.empty()){
            return false;
        }

        //get a street map node by its ID
        auto GetNode = [this](TNodeID id){ return DConfiguration->StreetMap()->NodeByID(id); };

        //format the distance using feet if below 0.1 miles, otherwise in miles w/ 2 sig figs
        auto FormatDist = [](double dist) -> std::string {
            if(dist < 0.1){
                return std::format("{:.0f} ft", dist * 5280.0);
            }
            return std::format("{:.2g} mi", dist);
        };

        auto StartNode = GetNode(path[0].second);
        if(!StartNode){ //invalid starting node
            return false;
        }
        desc.push_back("Start at " + SGeographicUtils::ConvertLLToDMS(StartNode->Location()));

        size_t i = 1;
        while(i < path.size()){
            auto Mode = path[i].first;
            auto PrevNodeID = path[i-1].second;
            auto CurrNodeID = path[i].second;

            if(Mode == ETransportationMode::Bus){
                //collect all consecutive bus steps to find the full board/exit range
                TNodeID BoardNodeID = PrevNodeID; //node where the passenger boards the bus
                TNodeID ExitNodeID = CurrNodeID; //will be updated to the last consecutive bus node
                
                while(i + 1 < path.size() && path[i+1].first == ETransportationMode::Bus){
                    i++;
                    ExitNodeID = path[i].second;
                }
                auto BoardStop = DBusSystemIndexer.StopByNodeID(BoardNodeID); //look up the bus stop at the boarding node
                auto ExitStop = DBusSystemIndexer.StopByNodeID(ExitNodeID); //look up the bus stop at the exit node
                if(!BoardStop || !ExitStop){ //these nodes aren't bus stops, something went wrong
                    return false;
                }

                //find the route that contains board→exit in order and goes furthest past exit.
                //tie-break: alphabetical route name.
                std::string BestRoute = "";
                double BestRemaining = -1;
                for(size_t r = 0; r < DConfiguration->BusSystem()->RouteCount(); r++){
                    auto Route = DConfiguration->BusSystem()->RouteByIndex(r);
                    bool FoundBoard = false;
                    for(size_t s = 0; s < Route->StopCount(); s++){
                        if(!FoundBoard && Route->GetStopID(s) == BoardStop->ID()){ //find where we board on this route
                            FoundBoard = true;
                        }
                        if(FoundBoard && Route->GetStopID(s) == ExitStop->ID()){ //found the exit stop after boarding
                            //calculate remaining distance the route continues past our exit stop
                            double Remaining = 0;
                            for(size_t k = s + 1; k < Route->StopCount(); k++){
                                auto SA = DConfiguration->BusSystem()->StopByID(Route->GetStopID(k-1));
                                auto SB = DConfiguration->BusSystem()->StopByID(Route->GetStopID(k));
                                auto NA = DConfiguration->StreetMap()->NodeByID(SA->NodeID());
                                auto NB = DConfiguration->StreetMap()->NodeByID(SB->NodeID());
                                if(NA && NB){
                                    Remaining += SGeographicUtils::HaversineDistanceInMiles(NA->Location(), NB->Location());
                                }
                            }
                            if(Remaining > BestRemaining || (Remaining == BestRemaining && (BestRoute.empty() || Route->Name() < BestRoute))){
                                BestRemaining = Remaining;
                                BestRoute = Route->Name();
                            }
                            break;
                        }
                    }
                }
                desc.push_back("Take Bus " + BestRoute + " from stop " +
                               std::to_string(BoardStop->ID()) + " to stop " +
                               std::to_string(ExitStop->ID()));
            }
            else{ //walk or bike segment
                std::string ModeStr = "Walk";
                if(Mode == ETransportationMode::Bike){
                    ModeStr = "Bike";
                }
                std::string StreetName = ""; //default to no name if edge isn't in our map
                if(DEdgeStreetName.count({PrevNodeID, CurrNodeID})){
                    StreetName = DEdgeStreetName.at({PrevNodeID, CurrNodeID});
                }

                TNodeID GroupStartNodeID = PrevNodeID; //first node of this street segment group
                TNodeID GroupEndNodeID = CurrNodeID; //last node, extended as we group consecutive same-street steps
                double GroupDist = SGeographicUtils::HaversineDistanceInMiles(
                    GetNode(PrevNodeID)->Location(), GetNode(CurrNodeID)->Location());

                //keep extending the group while the next step uses the same mode and street name
                while(i + 1 < path.size() && path[i+1].first == Mode){
                    auto NextNodeID = path[i+1].second;
                    std::string NextName = "";
                    if(DEdgeStreetName.count({CurrNodeID, NextNodeID})){
                        NextName = DEdgeStreetName.at({CurrNodeID, NextNodeID});
                    }
                    if(NextName != StreetName){ //different street, end the group
                        break;
                    }
                    i++;
                    GroupDist += SGeographicUtils::HaversineDistanceInMiles(
                        GetNode(GroupEndNodeID)->Location(), GetNode(NextNodeID)->Location());
                    GroupEndNodeID = NextNodeID;
                    CurrNodeID = NextNodeID;
                }

                //direction is calculated from the start to the end of the whole group, not just to the next node
                double Bearing = SGeographicUtils::CalculateBearing(
                    GetNode(GroupStartNodeID)->Location(), GetNode(GroupEndNodeID)->Location());
                std::string Dir = SGeographicUtils::BearingToDirection(Bearing);

                std::string Line = ModeStr + " " + Dir + " ";
                if(!StreetName.empty()){
                    Line += "along " + StreetName; 
                }
                else{
                    //unnamed street, look ahead for the next named street, or say "End"
                    std::string NextName = "End";
                    for(size_t j = i + 1; j < path.size(); j++){
                        auto JPrev = path[j-1].second;
                        auto JCurr = path[j].second;
                        if(DEdgeStreetName.count({JPrev, JCurr})){
                            std::string JName = DEdgeStreetName.at({JPrev, JCurr});
                            if(!JName.empty()){
                                NextName = JName;
                                break;
                            }
                        }
                    }
                    Line += "toward " + NextName; //unnamed street: "toward Main St." or "toward End"
                }
                Line += " for " + FormatDist(GroupDist);
                desc.push_back(Line);
            }
            i++;
        }

        auto EndNode = GetNode(path.back().second);
        desc.push_back("End at " + SGeographicUtils::ConvertLLToDMS(EndNode->Location()));
        return true;
    }
};

// Constructor for the Dijkstra Transportation Planner
CDijkstraTransportationPlanner::CDijkstraTransportationPlanner(std::shared_ptr<SConfiguration> config){
    DImplementation = std::make_unique<SImplementation>(config);
}

// Destructor for the Dijkstra Transportation Planner
CDijkstraTransportationPlanner::~CDijkstraTransportationPlanner(){

}

// Returns the number of nodes in the street map
std::size_t CDijkstraTransportationPlanner::NodeCount() const noexcept{
    return DImplementation->NodeCount();
}

// Returns the street map node specified by index if index is less than the
// NodeCount(). nullptr is returned if index is greater than or equal to
// NodeCount(). The nodes are sorted by Node ID.
std::shared_ptr<CStreetMap::SNode> CDijkstraTransportationPlanner::SortedNodeByIndex(std::size_t index) const noexcept{
    return DImplementation->SortedNodeByIndex(index);
}

// Returns the distance in miles between the src and dest nodes of the
// shortest path if one exists. NoPathExists is returned if no path exists.
// The nodes of the shortest path are filled in the path parameter.
double CDijkstraTransportationPlanner::FindShortestPath(TNodeID src, TNodeID dest, std::vector<TNodeID> &path){
    return DImplementation->FindShortestPath(src, dest, path);
}

// Returns the time in hours for the fastest path between the src and dest
// nodes of the if one exists. NoPathExists is returned if no path exists.
// The transportation mode and nodes of the fastest path are filled in the
// path parameter.
double CDijkstraTransportationPlanner::FindFastestPath(TNodeID src, TNodeID dest, std::vector<TTripStep> &path){
    return DImplementation->FindFastestPath(src, dest, path);
}

// Returns true if the path description is created. Takes the trip steps path
// and converts it into a human readable set of steps.
bool CDijkstraTransportationPlanner::GetPathDescription(const std::vector<TTripStep> &path, std::vector<std::string> &desc) const{
    return DImplementation->GetPathDescription(path, desc);
}
