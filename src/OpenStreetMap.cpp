#include "OpenStreetMap.h"
#include <unordered_map>
#include <iostream>

struct COpenStreetMap::SImplementation{
    const std::string DOSMTag = "osm";
    const std::string DNodeTag = "node";
    const std::string DTagTag = "tag";
    const std::string DWayTag = "way";
    const std::string DNodeIDAttr = "id";
    const std::string DNodeLatAttr = "lat";
    const std::string DNodeLonAttr = "lon";
    const std::string DWayIDAttr = "id";
    const std::string DWayNodeTag = "nd";
    const std::string DWayNodeAttr = "ref";
    const std::string DTagKeyAttr = "k";
    const std::string DTagValueAttr = "v";

    struct SNode: public CStreetMap::SNode{
        TNodeID DID; //Store the Node ID
        SLocation DLocation; //Store the location of the Node as (latitude, longitude)
        std::vector<std::string> DAttributesByIndex; //Store the attributes' key by index  
        std::unordered_map<std::string, std::string> DAttributesByKey; //Store the attributes' by key, value pair 

        ~SNode(){};
        TNodeID ID() const noexcept override{ //Returns the ID of the Node
            return DID;
        }

        SLocation Location() const noexcept override{ //Returns the (latitude, longitude) of the Node
            return DLocation;
        }

        std::size_t AttributeCount() const noexcept override{ //Returns the number of elements Attributes vector
            return DAttributesByIndex.size();
        }

        std::string GetAttributeKey(std::size_t index) const noexcept override{ //Checks for out of bounds, then returns the Attribute at the provided index
            size_t size = AttributeCount();
            if (index >= size){
                return "";
            }
            return DAttributesByIndex[index];
        }

        bool HasAttribute(const std::string &key) const noexcept override{ //Returns true if the key exists in the Attributes unordered_map, false otherwise 
            if (DAttributesByKey.find(key) == DAttributesByKey.end()){
                return false;
            }
            return true;
        }

        std::string GetAttribute(const std::string &key) const noexcept override{ //Returns the Attribute at the provided key, returns empty string if it doesn't exist
            if (DAttributesByKey.find(key) == DAttributesByKey.end()){
                return "";
            }
            return DAttributesByKey.at(key);
        }
    };

    struct SWay: public CStreetMap::SWay{
        TWayID DID; //Store the Way ID
        std::vector<TNodeID> Nodes; //Store the nodes in order
        std::vector<std::string> DAttributesByIndex; //Store the attributes' key by index
        std::unordered_map<std::string, std::string> DAttributesByKey; //Store the attributes' by key, value pair

        ~SWay(){};
        TWayID ID() const noexcept override{ //Returns the ID of the Way
            return DID;
        }

        std::size_t NodeCount() const noexcept override{ //Returns the number of elements in std::vector<TNodeID> Nodes
            return Nodes.size();
        }

        TNodeID GetNodeID(std::size_t index) const noexcept override{ //If the index is out of bounds, it returns InvalidWayID. Otherewise, it returns the node at the corresponding index
            size_t size = NodeCount();
            if (index >= size){
                return CStreetMap::InvalidWayID;
            }
            return Nodes[index];
        }

        std::size_t AttributeCount() const noexcept override{ //Returns the number of elements in std::vector<std::string> DAttributesByIndex
            return DAttributesByIndex.size();
        }

        std::string GetAttributeKey(std::size_t index) const noexcept override{ //If the index is out of bounds, it returns an empty string. Otherwise, it returns the Atrribute at the corresponding index
            size_t size = AttributeCount();
            if (index >= size){
                return "";
            }
            return DAttributesByIndex[index];
        }

        bool HasAttribute(const std::string &key) const noexcept override{ //Checks if the attribute key exists in the dictionary, returning true if yes and false if no
            if (DAttributesByKey.find(key) == DAttributesByKey.end()){
                return false;
            }
            return true;
        }

        std::string GetAttribute(const std::string &key) const noexcept override{ //If the attribute key doesn't exist, it returns an empty string. Otherwise, it returns the attribute value at the corresponding key
            if (DAttributesByKey.find(key) == DAttributesByKey.end()){
                return "";
            }
            return DAttributesByKey.at(key);
        }
    };

    std::vector<std::shared_ptr<SNode>> DNodesByIndex;
    std::unordered_map<TNodeID,std::shared_ptr<SNode>> DNodesByID;

    bool ParseNodes(std::shared_ptr<CXMLReader> src, SXMLEntity &nextentity){
        SXMLEntity TempEntity;

        while(src->ReadEntity(TempEntity)){ //Loops and parses through all the nodes
            if(TempEntity.DType == SXMLEntity::EType::StartElement && TempEntity.DNameData == DWayTag){ //Once we see the first Way tag, we finished parsing all the Nodes and set NextEntity to the just-consumed Way entity, letting ParseWay parse it 
                nextentity = TempEntity;
                return true;
            }

            if(TempEntity.DType == SXMLEntity::EType::EndElement && TempEntity.DNameData == DOSMTag){ //If there aren’t ways, then there will be missing info as well
                return false;
            } 

            if(TempEntity.DType == SXMLEntity::EType::StartElement && TempEntity.DNameData == DNodeTag){ //If we see a start Node tag, we parse and loop until see an end Node tag
                if (TempEntity.AttributeValue(DNodeIDAttr).empty() || TempEntity.AttributeValue(DNodeLatAttr).empty() || TempEntity.AttributeValue(DNodeLonAttr).empty()){ //Not a valid Node as it must have id and location
                    return false;
                }
                auto NodeID = std::stoull(TempEntity.AttributeValue(DNodeIDAttr)); //Retrieve the attributes given by the XML Reader
                auto NodeLat = std::stod(TempEntity.AttributeValue(DNodeLatAttr));
                auto NodeLon = std::stod(TempEntity.AttributeValue(DNodeLonAttr));
                auto NewNode = std::make_shared<SNode>(); //Create the new node
                NewNode->DID = NodeID; //Set the ID
                NewNode->DLocation = {NodeLat,NodeLon}; //Set the location
                DNodesByIndex.push_back(NewNode); //Push the node into the vector
                DNodesByID[NodeID] = NewNode; //Push the (id, node) pair into the unordered_map

                while(src->ReadEntity(TempEntity)){ //Loop until we find an end Node tag
                    if(TempEntity.DType == SXMLEntity::EType::EndElement && TempEntity.DNameData == DNodeTag){ //If we see an end Node tag, break out of the loop and keep parsing for more nodes
                        break;
                    }

                    if(TempEntity.DType == SXMLEntity::EType::StartElement && TempEntity.DNameData == DTagTag){ //If we see a start Tag tag, store the attributes in the vector and unordered_map
                        auto TagKey = TempEntity.AttributeValue(DTagKeyAttr); //Retrieve the attributes given by the XML Reader
                        auto TagValue = TempEntity.AttributeValue(DTagValueAttr);
                        NewNode->DAttributesByIndex.push_back(TagKey);
                        NewNode->DAttributesByKey[TagKey] = TagValue; 
                    }

                    if(TempEntity.DType == SXMLEntity::EType::EndElement && TempEntity.DNameData == DTagTag){ //If we see an end Tag tag, continue on the loop
                        continue;
                    }
                }
            }
        }
        return false;
    }

    std::vector<std::shared_ptr<SWay>> DWaysByIndex;
    std::unordered_map<TWayID,std::shared_ptr<SWay>> DWaysByID;

    bool ParseWays(std::shared_ptr<CXMLReader> src, SXMLEntity &firstentity){
        SXMLEntity TempEntity = firstentity; //Parse the Way that was parsed in ParseNode()

        do{ //Loops and parses through all the ways
            if(TempEntity.DType == SXMLEntity::EType::EndElement && TempEntity.DNameData == DOSMTag){ //If we see the end OSM tag, we've reached the end of the file and can return true safely
                return true;
            } 

            if(TempEntity.DType == SXMLEntity::EType::StartElement && TempEntity.DNameData == DWayTag){  //If we see a start Way tag, we parse and loop until see an end Node tag
                if(TempEntity.AttributeValue(DWayIDAttr).empty()){ //Check if Way doesn't contain an ID, in which we stop parsing and return false
                    return false;
                }
                auto WayID = std::stoull(TempEntity.AttributeValue(DWayIDAttr)); //Retrieve the attributes given by the XML Reader
                auto NewWay = std::make_shared<SWay>(); //Create the new wyt
                NewWay->DID = WayID; //Set the ID
                 
                while (src->ReadEntity(TempEntity)){ //Loop until we find an end Way tag
                    if(TempEntity.DType == SXMLEntity::EType::EndElement && TempEntity.DNameData == DWayTag){ //If we see an end Node tag, break out of the loop and keep parsing for more ways
                        break;
                    }

                    if(TempEntity.DType == SXMLEntity::EType::StartElement && TempEntity.DNameData == DWayNodeTag){ //If we see a start node tag, we add it to our nodes vector
                        auto NodeID = std::stoull(TempEntity.AttributeValue(DWayNodeAttr));
                        NewWay->Nodes.push_back(NodeID);
                    }

                    if(TempEntity.DType == SXMLEntity::EType::EndElement && TempEntity.DNameData == DWayNodeTag){ //If we see and end node tag, continue on the loop
                        continue;
                    }

                    if(TempEntity.DType == SXMLEntity::EType::StartElement && TempEntity.DNameData == DTagTag){ //If we see a start Tag tag, store the attributes in the vector and unordered_map
                        auto TagKey = TempEntity.AttributeValue(DTagKeyAttr); //Retrieve the attributes given by the XML reader
                        auto TagValue = TempEntity.AttributeValue(DTagValueAttr);
                        NewWay->DAttributesByIndex.push_back(TagKey);
                        NewWay->DAttributesByKey[TagKey] = TagValue; 
                    }

                    if(TempEntity.DType == SXMLEntity::EType::EndElement && TempEntity.DNameData == DTagTag){ //If we see an end Tag tag, continue on the loop
                        continue;
                    }
                }
                if(NewWay->NodeCount() < 2){ //Not a valid Way as it must have at least 2 nodes
                    return false;
                }
                else{ //Safely store the Way
                    DWaysByIndex.push_back(NewWay); // Push the way into the vector
                    DWaysByID[WayID] = NewWay; //Push the (id, way) pair into the unordered_map
                }
            }
        }
        while (src->ReadEntity(TempEntity)); //do-while loop
        return false;
    }

    bool ParseOpenStreetMap(std::shared_ptr<CXMLReader> src){ 
        SXMLEntity TempEntity;
        
        if(src->ReadEntity(TempEntity)){ 
            if(TempEntity.DType == SXMLEntity::EType::StartElement && TempEntity.DNameData == DOSMTag){ //Checks if it's a valid OSM file (should have start OSM tag)
                if(ParseNodes(src,TempEntity)){ //If ParseNodes() was successful (not invalid OSM file, nothing crashed), continue with ParseWays()
                    if(ParseWays(src,TempEntity)){ //If PasrseWays() was successful (not invalid OSM file, nothing crashed), return true to indicate successful parsing
                        return true;
                    }
                }
            }
        }
        return false;
    }

    SImplementation(std::shared_ptr<CXMLReader> src){
        ParseOpenStreetMap(src);
    }

    std::size_t NodeCount() const noexcept{ //Returns size of std::vector<std::shared_ptr<SNode>> DNodesByIndex
        return DNodesByIndex.size();
    }

    std::size_t WayCount() const noexcept{ //Return size of std::vector<std::shared_ptr<SWay>> DWaysByIndex;
        return DWaysByIndex.size();
    }
    
    std::shared_ptr<CStreetMap::SNode> NodeByIndex(std::size_t index) const noexcept{ //Checks for out of bounds, then returns the Node at the provided index
        size_t size = NodeCount();
        if(index >= size){
            return nullptr;
        }
        return DNodesByIndex[index];
    }
    
    std::shared_ptr<CStreetMap::SNode> NodeByID(TNodeID id) const noexcept{ //Checks if the id is in the unordered_map, then returns the Node at the provide id key
        if(DNodesByID.find(id) == DNodesByID.end()){
            return nullptr;
        }
        return DNodesByID.at(id);;
    }
    
    std::shared_ptr<CStreetMap::SWay> WayByIndex(std::size_t index) const noexcept{ //Checks for out of bounds, then returns the Way at the provided index
        size_t size = WayCount();
        if(index >= size){
            return nullptr;
        }
        return DWaysByIndex[index];
    }
    
    std::shared_ptr<CStreetMap::SWay> WayByID(TWayID id) const noexcept{ //Checks if the id is in the unordered_map, then returns the Way at the provide id key
        if(DWaysByID.find(id) == DWaysByID.end()){
            return nullptr;
        }
        return DWaysByID.at(id);
    }
};

COpenStreetMap::COpenStreetMap(std::shared_ptr<CXMLReader> src){
    DImplementation = std::make_unique<SImplementation>(src);
}

COpenStreetMap::~COpenStreetMap(){
}

std::size_t COpenStreetMap::NodeCount() const noexcept {
    return DImplementation->NodeCount();
}
std::size_t COpenStreetMap::WayCount() const noexcept{
    return DImplementation->WayCount();
}

std::shared_ptr<CStreetMap::SNode> COpenStreetMap::NodeByIndex(std::size_t index) const noexcept{
    return DImplementation->NodeByIndex(index);
}

std::shared_ptr<CStreetMap::SNode> COpenStreetMap::NodeByID(TNodeID id) const noexcept{
    return DImplementation->NodeByID(id);
}
std::shared_ptr<CStreetMap::SWay> COpenStreetMap::WayByIndex(std::size_t index) const noexcept{
    return DImplementation->WayByIndex(index);
}

std::shared_ptr<CStreetMap::SWay> COpenStreetMap::WayByID(TWayID id) const noexcept{
    return DImplementation->WayByID(id);
}
