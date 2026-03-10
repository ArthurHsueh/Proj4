#include "DijkstraPathRouter.h"
#include <queue>
#include <algorithm>

struct CDijkstraPathRouter::SImplementation{
    struct SVertex;
    using TEdge = std::pair<double,std::shared_ptr<SVertex>>;
    struct SVertex{
        std::vector<TEdge> DEdges;
        std::any DTag;
    };
    std::vector<std::shared_ptr<SVertex>> DVertices;

    SImplementation(){

    }

    ~SImplementation(){

    }

    std::size_t VertexCount() const noexcept{
        return DVertices.size(); //Return the size of the vector storing the vertices
    }

    TVertexID AddVertex(std::any tag) noexcept{
        auto NewVertex = std::make_shared<SVertex>(); //Create a shared pointer to an SVertex
        NewVertex->DTag = tag; //Assings tag to the vertex 
        TVertexID NewID = DVertices.size(); //Assigns the ID (index of the vector storing the vertices) to the vertex
        DVertices.push_back(NewVertex); //Push the vertex to the vector
        return NewID;
    }

    std::any GetVertexTag(TVertexID id) const noexcept{
        if(id < DVertices.size()){ //Check if the ID is valid
            return DVertices[id]->DTag; //Returns the tag of the vertex indexed by theID
        }
        return std::any();
    }

    bool AddEdge(TVertexID src, TVertexID dest, double weight, bool bidir = false) noexcept{
        if(src < DVertices.size() && dest < DVertices.size()){ //Checks if the IDs' are valid
            DVertices[src]->DEdges.push_back(std::make_pair(weight,DVertices[dest])); //Create the edge
            if(bidir){
                DVertices[dest]->DEdges.push_back(std::make_pair(weight,DVertices[src])); //If bidirectional, create the edge going the other direction as well
            }
            return true; //Return true on success
        }
        return false;
    }

    bool Precompute(std::chrono::steady_clock::time_point deadline) noexcept{
        return true;
    }

    double FindShortestPath(TVertexID src, TVertexID dest, std::vector<TVertexID> &path) noexcept{
        path.clear();
        std::vector<double> Weights; //Array to store the weight (distance) from the soruce node
        Weights.resize(DVertices.size(),std::numeric_limits<double>::max()); //Initially, all nodes are infinitely away from the source node
        std::vector<TVertexID> Previous; //Array to store the node preceding a given node, allowing us to construct the path by going backwards
        Previous.resize(DVertices.size(),std::numeric_limits<TVertexID>::max()); //Initially, all nodes have no nodes preceding it as no path has been constructred it
        std::priority_queue<std::pair<double,TVertexID>, std::vector<std::pair<double,TVertexID>>, std::greater<std::pair<double,TVertexID>>> pq; //Priority queue used to retrieve the (univisted) node with the shortest distance in O(log V)

        Weights[src] = 0.0; //The distance from the source to itself is 0
        pq.push(std::make_pair(0.0, src)); //Push the source and its weight to the queue to begin traversal

        while(!pq.empty()){ //Loop until the heap is empty
            auto node = pq.top(); //Pop the next univisted node with the shortest distance
            pq.pop();
            double edgeWeight = node.first;
            TVertexID id = node.second;
            if(edgeWeight > Weights[id]){  //Lazy Dijkstra: ignore entries in the queue that have longer distances than what is currently stored in the Weights[] array
                continue;
            }
            
            if(id == dest){ //If we find the destination node, break out of the loop early
                break;
            }

            std::vector<TEdge> &adjacent = DVertices[id]->DEdges; //Iterates through the adjacency list
            for(TEdge edge: adjacent){
                auto weight = edge.first; //Distance from the node
                auto neighbor = edge.second; //Shared pointer of the node, in which we have to convert to its ID
                auto res = (std::find(DVertices.begin(), DVertices.end(), neighbor));
                auto neighborid = static_cast<size_t>(std::distance(DVertices.begin(), res));

                if (Weights[id] + weight < Weights[neighborid]){ //If we find a shorter distance from source to the node
                    Weights[neighborid] = Weights[id] + weight; //Update weight
                    Previous[neighborid] = id; //Update parent
                    pq.push(std::make_pair(Weights[id] + weight, neighborid)); //Push the new entry into queue for evaluation of shortest distance and relaxation of adjacent nodes
                }
            }
        }

        if(Weights[dest] != std::numeric_limits<double>::max()){ //If a path from source to destination does exist, construct the path
            TVertexID node = dest;
            while(node != src){
                path.push_back(node);
                node = Previous[node];
            }
            path.push_back(src);
            std::reverse(path.begin(), path.end()); //Reverse the the order of the vector
        }

        return Weights[dest]; //Return the final distance from source to destination
    }
};

CDijkstraPathRouter::CDijkstraPathRouter(){
    DImplementation = std::make_unique<SImplementation>();
}

CDijkstraPathRouter::~CDijkstraPathRouter(){

}

std::size_t CDijkstraPathRouter::VertexCount() const noexcept{
    return DImplementation->VertexCount();
}

CPathRouter::TVertexID CDijkstraPathRouter::AddVertex(std::any tag) noexcept{
    return DImplementation->AddVertex(tag);
}

std::any CDijkstraPathRouter::GetVertexTag(TVertexID id) const noexcept{
    return DImplementation->GetVertexTag(id);
}

bool CDijkstraPathRouter::AddEdge(TVertexID src, TVertexID dest, double weight, bool bidir) noexcept{
    return DImplementation->AddEdge(src,dest,weight,bidir);
}

bool CDijkstraPathRouter::Precompute(std::chrono::steady_clock::time_point deadline) noexcept{
    return DImplementation->Precompute(deadline);
}

double CDijkstraPathRouter::FindShortestPath(TVertexID src, TVertexID dest, std::vector<TVertexID> &path) noexcept{
    return DImplementation->FindShortestPath(src,dest,path);
}
