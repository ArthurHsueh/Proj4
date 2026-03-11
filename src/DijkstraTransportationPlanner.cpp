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
    CBusSystemIndexer DBusSystemIndexer; //indexes the bus system for easy stop/route lookup

    CDijkstraPathRouter DShortestPathRouter; //graph for calculating shortest distance
    CDijkstraPathRouter DFastestBikeRouter; //graph for calculating fastest path for bike and walk
    CDijkstraPathRouter DFastestBusRouter;  //graph for calculating fastest path for bus and walk
    
    //maps
    std::unordered_map<TNodeID, CPathRouter::TVertexID> DShortestNodeToVertex; 
    std::unordered_map<TNodeID, CPathRouter::TVertexID> DFastestBikeNodeToVertex; 
    std::unordered_map<TNodeID, CPathRouter::TVertexID> DFastestBusNodeToVertex; 
    
    using ETransportationMode = CTransportationPlanner::ETransportationMode;
    using TTripStep = CTransportationPlanner::TTripStep;


    struct SEdgePairHash{  //hash function to combine the vertex IDs of an edge 
        size_t operator()(const std::pair<CPathRouter::TVertexID, CPathRouter::TVertexID> &p) const{
            size_t First = p.first;
            size_t Second = p.second;
            return First * 1000003 + Second;
        }
    };

    struct SNodeIDPairHash{ //hash function to combine the node IDs of an edge 
        size_t operator()(const std::pair<TNodeID, TNodeID> &p) const{
            size_t First = p.first;
            size_t Second = p.second;
            return First * 1000003 + Second;
        }
    };

    std::unordered_map<std::pair<TNodeID, TNodeID>, std::string, SNodeIDPairHash> DEdgeStreetName; //street name for each directed road edge
    std::unordered_map<std::pair<TNodeID, TNodeID>, double, SNodeIDPairHash> DEdgeSpeedLimit; //speed limit for each directed road edge
    std::unordered_map<std::pair<CPathRouter::TVertexID, CPathRouter::TVertexID>, ETransportationMode, SEdgePairHash> DFastestBikeEdgeModes; //map to identify whether a fastest bike edge is walk or bike
    std::unordered_map<std::pair<CPathRouter::TVertexID, CPathRouter::TVertexID>, ETransportationMode, SEdgePairHash> DFastestBusEdgeModes; //map to identify whether a fastest bus edge is walk or bus
    
    SImplementation(std::shared_ptr<SConfiguration> config) : DBusSystemIndexer(config->BusSystem()){
        DConfiguration = config;

        //get all nodes from the street map into DSortedNodes
        for(size_t Index = 0; Index < DConfiguration->StreetMap()->NodeCount(); Index++){
            DSortedNodes.push_back(DConfiguration->StreetMap()->NodeByIndex(Index));
        }
        //sort nodes by ID 
        std::sort(DSortedNodes.begin(), DSortedNodes.end(), [](std::shared_ptr<CStreetMap::SNode> l, std::shared_ptr<CStreetMap::SNode> r) -> bool{
            return l->ID() < r->ID();
        });

        //register every node as a vertex in both routers for CDijkstraPathRouter
        for(size_t Index = 0; Index < DSortedNodes.size(); Index++){
            auto Node = DSortedNodes[Index];
            DShortestNodeToVertex[Node->ID()] = DShortestPathRouter.AddVertex(Node->ID());
            DFastestBikeNodeToVertex[Node->ID()] = DFastestBikeRouter.AddVertex(Node->ID());
            DFastestBusNodeToVertex[Node->ID()] = DFastestBusRouter.AddVertex(Node->ID());
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
            double SpeedLimit = DConfiguration->DefaultSpeedLimit(); //default is 25mph
            if(Way->HasAttribute("maxspeed")){ //safety check if maxspeed attribute exists before trying to read it, since not all roads have speed limits
                try{
                    SpeedLimit = std::stod(Way->GetAttribute("maxspeed"));
                }
                catch(...){
                    SpeedLimit = DConfiguration->DefaultSpeedLimit();
                }   
            }
            
            //iterate over the nodes inside the way
            for(size_t Index = 1; Index < Way->NodeCount(); Index++){
                auto FirstNodeID = Way->GetNodeID(Index-1); 
                auto SecondNodeID = Way->GetNodeID(Index);

                auto FirstNode = DConfiguration->StreetMap()->NodeByID(FirstNodeID); 
                auto SecondNode = DConfiguration->StreetMap()->NodeByID(SecondNodeID);
                //calculate distance between the 2 nodes, use haversine because the world is round!!
                auto Distance = SGeographicUtils::HaversineDistanceInMiles(FirstNode->Location(), SecondNode->Location()); 

                auto FirstVertexID = DShortestNodeToVertex[FirstNodeID];
                auto SecondVertexID = DShortestNodeToVertex[SecondNodeID];
                DShortestPathRouter.AddEdge(FirstVertexID, SecondVertexID, Distance);
                if(!IsOneWay){ //if its not a one-way street add the opposite edge
                    DShortestPathRouter.AddEdge(SecondVertexID, FirstVertexID, Distance);
                }
                DEdgeStreetName[{FirstNodeID, SecondNodeID}] = Name;
                DEdgeStreetName[{SecondNodeID, FirstNodeID}] = Name;
                DEdgeSpeedLimit[{FirstNodeID, SecondNodeID}] = SpeedLimit;
                DEdgeSpeedLimit[{SecondNodeID, FirstNodeID}] = SpeedLimit;

            }
        }

        //build graphs for Dijkstra's algorithm
        for(size_t WayIndex = 0; WayIndex < DConfiguration->StreetMap()->WayCount(); WayIndex++){ //for each way in the street map
            auto Way = DConfiguration->StreetMap()->WayByIndex(WayIndex); 
            
            //check the way/roads attributes
            bool IsOneWay = Way->HasAttribute("oneway") && Way->GetAttribute("oneway") == "yes"; 
            bool NoBike = Way->HasAttribute("bicycle") && Way->GetAttribute("bicycle") == "no"; 
            
            for(size_t Index = 1; Index < Way->NodeCount(); Index++){ //for every node in the way
                auto FirstNodeID = Way->GetNodeID(Index-1); 
                auto SecondNodeID = Way->GetNodeID(Index);

                auto FirstNode = DConfiguration->StreetMap()->NodeByID(FirstNodeID); 
                auto SecondNode = DConfiguration->StreetMap()->NodeByID(SecondNodeID);
                // calculate distance between the 2 nodes
                auto Distance = SGeographicUtils::HaversineDistanceInMiles(FirstNode->Location(), SecondNode->Location());
                

                //grab the corresponding vertex IDs for both graphs
                auto FirstBikeID = DFastestBikeNodeToVertex[FirstNodeID];
                auto SecondBikeID = DFastestBikeNodeToVertex[SecondNodeID];
                auto FirstBusID = DFastestBusNodeToVertex[FirstNodeID];
                auto SecondBusID = DFastestBusNodeToVertex[SecondNodeID];

                //add walk edges to the bus graph
                double WalkTime = Distance / DConfiguration->WalkSpeed(); 
                DFastestBusRouter.AddEdge(FirstBusID, SecondBusID, WalkTime); 
                DFastestBusEdgeModes[{FirstBusID, SecondBusID}] = ETransportationMode::Walk;
                DFastestBusRouter.AddEdge(SecondBusID, FirstBusID, WalkTime); 
                DFastestBusEdgeModes[{SecondBusID, FirstBusID}] = ETransportationMode::Walk;

                //add bike edges to the bike graph
                if(!NoBike){  //check if bikes are allowed
                    double BikeTime = Distance / DConfiguration->BikeSpeed();
                    DFastestBikeRouter.AddEdge(FirstBikeID, SecondBikeID, BikeTime); 
                    DFastestBikeEdgeModes[{FirstBikeID, SecondBikeID}] = ETransportationMode::Bike;
                    if(!IsOneWay){ //if its not a one-way street add the opposite edge
                        DFastestBikeRouter.AddEdge(SecondBikeID, FirstBikeID, BikeTime);
                        DFastestBikeEdgeModes[{SecondBikeID, FirstBikeID}] = ETransportationMode::Bike;
                    }
                }
            }
        }


        //build the bus network for the graph
        for(size_t RouteIndex = 0; RouteIndex < DConfiguration->BusSystem()->RouteCount(); RouteIndex++){ //for every bus route
            auto Route = DConfiguration->BusSystem()->RouteByIndex(RouteIndex);
            for(size_t Index = 1; Index < Route->StopCount(); Index++){ //for every bus stop in the route
                auto PrevStop = DConfiguration->BusSystem()->StopByID(Route->GetStopID(Index-1));
                auto CurrStop = DConfiguration->BusSystem()->StopByID(Route->GetStopID(Index));
                std::vector<CPathRouter::TVertexID> ShortestPath;
                
                auto PreviousNodeID = PrevStop->NodeID(); //get node ids
                auto CurrentNodeID = CurrStop->NodeID();
                auto StartVertex = DShortestNodeToVertex[PreviousNodeID]; //map the node ids to the vertex ids
                auto EndVertex = DShortestNodeToVertex[CurrentNodeID];
                double RoadDistance = DShortestPathRouter.FindShortestPath(StartVertex, EndVertex, ShortestPath); //find the length of the path between the 2 nodes
                
                if(RoadDistance == CPathRouter::NoPathExists){ //if no road connection between these stops, skip
                    continue;
                }

                //find the time it takes for a bus to travel between 2 stops of a path
                double BusTime = 0;
                for(size_t k = 1; k < ShortestPath.size(); k++){ //for each node in the bus path
                    auto PrevNodeID = std::any_cast<TNodeID>(DShortestPathRouter.GetVertexTag(ShortestPath[k-1]));
                    auto CurrNodeID = std::any_cast<TNodeID>(DShortestPathRouter.GetVertexTag(ShortestPath[k]));
                    auto PrevNode = DConfiguration->StreetMap()->NodeByID(PrevNodeID);
                    auto CurrNode = DConfiguration->StreetMap()->NodeByID(CurrNodeID);
                    double SegDist = SGeographicUtils::HaversineDistanceInMiles(PrevNode->Location(), CurrNode->Location());
                    double SpeedLimit = DConfiguration->DefaultSpeedLimit(); //check if edge isn't in our map
                    if(DEdgeSpeedLimit.count({PrevNodeID, CurrNodeID})){ 
                        SpeedLimit = DEdgeSpeedLimit.at({PrevNodeID, CurrNodeID});
                    }
                    BusTime += SegDist / SpeedLimit;
                }
                
                //map to the bus graph vertices 
                auto PrevVertexID = DFastestBusNodeToVertex[PrevStop->NodeID()];
                auto CurrVertexID = DFastestBusNodeToVertex[CurrStop->NodeID()];
                
                BusTime += DConfiguration->BusStopTime() / 3600.0; // add bus stop time per edge
                DFastestBusRouter.AddEdge(PrevVertexID, CurrVertexID, BusTime);
                DFastestBusEdgeModes[{PrevVertexID, CurrVertexID}] = ETransportationMode::Bus;
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
        
        //check if source or destination are not in the map
        if(!DFastestBikeNodeToVertex.count(src) || !DFastestBikeNodeToVertex.count(dest)){ 
            return CPathRouter::NoPathExists;
        }

        std::vector<CPathRouter::TVertexID> BikeVertexPath;
        std::vector<CPathRouter::TVertexID> BusVertexPath;

        //call findshortestpath on both routers to find the time for them
        double BikeTime = DFastestBikeRouter.FindShortestPath(DFastestBikeNodeToVertex.at(src), DFastestBikeNodeToVertex.at(dest), BikeVertexPath);
        double BusTime = DFastestBusRouter.FindShortestPath(DFastestBusNodeToVertex.at(src), DFastestBusNodeToVertex.at(dest), BusVertexPath);

        //if neither router could find a path, return NoPathExists
        if(BikeTime == CPathRouter::NoPathExists && BusTime == CPathRouter::NoPathExists){ 
            return CPathRouter::NoPathExists;
        }

        //determine which path is faster
        double BestTime;
        std::vector<CPathRouter::TVertexID> BestVertexPath;
        //point BestEdgeNodes to the router that was faster
        //pointer is used so i dont have to repeat the check for who won
        std::unordered_map<std::pair<CPathRouter::TVertexID, CPathRouter::TVertexID>, ETransportationMode, SEdgePairHash>* BestEdgeModes;
        CDijkstraPathRouter* BestRouter;
        //bike wins if it found a path and bus found no path or bike time is strictly less than or equal to bus time
        if(BikeTime != CPathRouter::NoPathExists && (BusTime == CPathRouter::NoPathExists || BikeTime <= BusTime)){
            BestTime = BikeTime;
            BestVertexPath = BikeVertexPath;
            BestEdgeModes = &DFastestBikeEdgeModes;
            BestRouter = &DFastestBikeRouter;
        } else {
            BestTime = BusTime;
            BestVertexPath = BusVertexPath;
            BestEdgeModes = &DFastestBusEdgeModes;
            BestRouter = &DFastestBusRouter;
        }

        //reconstruct the path with the correct modes
        for(size_t i = 0; i < BestVertexPath.size(); i++){
            auto NodeID = std::any_cast<TNodeID>(BestRouter->GetVertexTag(BestVertexPath[i]));
            
            if(i == 0){ //first step is the starting node
                ETransportationMode FirstMode = ETransportationMode::Walk;
                if(BestVertexPath.size() > 1){
                    auto It = BestEdgeModes->find({BestVertexPath[0], BestVertexPath[1]});
                    if(It != BestEdgeModes->end() && It->second == ETransportationMode::Bike){
                        FirstMode = ETransportationMode::Bike;
                    }
                }
                path.push_back({FirstMode, NodeID});
            }
            else{ 
                //look up the mode for this specific edge
                auto ModeIt = BestEdgeModes->find({BestVertexPath[i-1], BestVertexPath[i]}); 
                auto Mode = ETransportationMode::Walk; //default to walk
                
                if(ModeIt != BestEdgeModes->end()){
                    Mode = ModeIt->second;
                }
                path.push_back({Mode, NodeID});
            }
        }
        
        return BestTime;
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
            TNodeID BoardNodeID = PrevNodeID; //node where the passenger boards the bus
            TNodeID ExitNodeID = CurrNodeID; //will be updated to the last consecutive bus node
            
            //look up the initial boarding stop to use for route validation
            auto BoardStop = DBusSystemIndexer.StopByNodeID(BoardNodeID);
            if(!BoardStop) return false;

            //group consecutive bus steps that stay on the same route
            while(i + 1 < path.size() && path[i+1].first == ETransportationMode::Bus){
                auto NextExitNodeID = path[i+1].second;
                auto NextExitStop = DBusSystemIndexer.StopByNodeID(NextExitNodeID);
                if(!NextExitStop) break;
                
                //check if any single route connects our original BoardStop to this NextExitStop
                bool ValidRouteExists = false;
                for(size_t r = 0; r < DConfiguration->BusSystem()->RouteCount(); r++){
                    auto Route = DConfiguration->BusSystem()->RouteByIndex(r);
                    bool FoundBoarding = false;
                    for(size_t s = 0; s < Route->StopCount(); s++){
                        //checks to handle short routes correctly
                        if(!FoundBoarding && Route->GetStopID(s) == BoardStop->ID()){
                            FoundBoarding = true;
                        }
                        if(FoundBoarding && Route->GetStopID(s) == NextExitStop->ID()){
                            ValidRouteExists = true;
                            break;
                        }
                    }
                    if(ValidRouteExists){
                        break;
                    }
                }
                
                if(!ValidRouteExists){
                    break; //transfer needed, stop grouping
                }
                
                //valid connection found, extend the current bus leg
                i++;
                ExitNodeID = path[i].second;
            }

            //get the actual stop object
            auto ExitStop = DBusSystemIndexer.StopByNodeID(ExitNodeID);
            if(!ExitStop) return false;

            //find the BestRoute name that services the BoardStop -> ExitStop pair
            std::string BestRoute = "";
            double BestRemaining = -1;
            for(size_t r = 0; r < DConfiguration->BusSystem()->RouteCount(); r++){
                auto Route = DConfiguration->BusSystem()->RouteByIndex(r);
                bool FoundBoarding = false;
                for(size_t s = 0; s < Route->StopCount(); s++){
                    if(!FoundBoarding && Route->GetStopID(s) == BoardStop->ID()){
                        FoundBoarding = true;
                    }
                    if(FoundBoarding && Route->GetStopID(s) == ExitStop->ID()){
                        //calculate remaining distance for tie-breaking
                        double Remaining = 0;
                        for(size_t k = s + 1; k < Route->StopCount(); k++){
                            auto SA = DConfiguration->BusSystem()->StopByID(Route->GetStopID(k-1));
                            auto SB = DConfiguration->BusSystem()->StopByID(Route->GetStopID(k));
                            auto NA = DConfiguration->StreetMap()->NodeByID(SA->NodeID());
                            auto NB = DConfiguration->StreetMap()->NodeByID(SB->NodeID());
                            if(NA && NB) Remaining += SGeographicUtils::HaversineDistanceInMiles(NA->Location(), NB->Location());
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
            std::string ModeStr = (Mode == ETransportationMode::Bike) ? "Bike" : "Walk";
            std::string StreetName = "";
            if(DEdgeStreetName.count({PrevNodeID, CurrNodeID})){
                StreetName = DEdgeStreetName.at({PrevNodeID, CurrNodeID});
            }

            TNodeID GroupStartNodeID = PrevNodeID;
            TNodeID GroupEndNodeID = CurrNodeID;
            double GroupDist = SGeographicUtils::HaversineDistanceInMiles(GetNode(PrevNodeID)->Location(), GetNode(CurrNodeID)->Location());

            // Group same-street steps
            while(i + 1 < path.size() && path[i+1].first == Mode){
                auto NextNodeID = path[i+1].second;
                std::string NextName = (DEdgeStreetName.count({CurrNodeID, NextNodeID})) ? DEdgeStreetName.at({CurrNodeID, NextNodeID}) : "";
                
                if(NextName != StreetName) break;

                i++;
                GroupDist += SGeographicUtils::HaversineDistanceInMiles(GetNode(GroupEndNodeID)->Location(), GetNode(NextNodeID)->Location());
                GroupEndNodeID = NextNodeID;
                CurrNodeID = NextNodeID;
            }

            double Bearing = SGeographicUtils::CalculateBearing(GetNode(GroupStartNodeID)->Location(), GetNode(GroupEndNodeID)->Location());
            std::string Dir = SGeographicUtils::BearingToDirection(Bearing);
            std::string Line = ModeStr + " " + Dir + " ";

            if(!StreetName.empty()){
                Line += "along " + StreetName; 
            }
            else{
                std::string NextName = "End";
                for(size_t j = i + 1; j < path.size(); j++){
                    if(DEdgeStreetName.count({path[j-1].second, path[j].second})){
                        std::string JName = DEdgeStreetName.at({path[j-1].second, path[j].second});
                        if(!JName.empty()){ NextName = JName; break; }
                    }
                }
                Line += "toward " + NextName;
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
