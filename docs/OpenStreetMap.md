# OpenStreetMap

This class implements a parser for OSM files. It inherits from StreetMap.h, an abstract class that defines the SNode and SWay structs and various functions for accessing them. It uses the XML Reader from XMLReader.h to parse through the XML file, storing Nodes, Ways, and their corresponding attributes. It also contains various functions that allow users to access node, ways, and attributes through indexing or through (key, value) pairs. 

## Constructor

#### **COpenStreetMap::COpenStreetMap(std::shared_ptr<CXMLReader> src)**

The Parser accepts a shared pointer to an XML Reader object for its constructor, allowing it parse through the OSM file and store and return the parsed data safety.

## Structs

## struct SNode: public CStreetMap::SNode ##

#### **TNodeID ID() const noexcept override** ####
This functions returns the Node's ID.

#### **TLocation Location() const noexcept override** ####
This function returns the Node's (latitude, longitude).

#### **std::size_t AttributeCount() const noexcept override** ####
This function returns the number of attributes in the Node. It does this by using a vector to store the attributes and returning it size.

#### **std::string GetAttributeKey(std::size_t index) const noexcept override** ####
This function returns the attribute key at a given index. It does this through a vector, retruning the corresponding attribute it the index is valid.

#### **bool HasAttribute(const std::string &key) const noexcept override** ####
This function indicates whether a given key in its unordered_map exists, returning true it it does and false otherwise.

#### **std::string GetAttribute(const std::string &key) const noexcept override** ####
This function returns the attribute value of a given key. It does this through an unordered_map, checking if the key exists and returing its attribute value. 


## struct SWay: public CStreetMap::SWay ##

#### **TWayID ID() const noexcept override** ####
This functions returns the Way's ID.

#### **std::size_t NodeCount() const noexcept override** ####
This functions returns tne number of nodes in the Way.

#### **TNodeID GetNodeID(std::size_t index) const noexcept override** ####
This functions returns the node at a given index. It does this through a vector, retruning the corresponding attribute it the index is valid.

#### **std::size_t AttributeCount() const noexcept override** ####
This function returns the number of attributes in the Way. It does this by using a vector to store the attributes and returning it size.

#### **std::string GetAttributeKey(std::size_t index) const noexcept override** ####
This function returns the attribute value at a given index. It does this through a vector, retruning the corresponding attribute it the index is valid.

#### **bool HasAttribute(const std::string &key) const noexcept override** ####
This function indicates whether a given key in its unordered_map exists, returning true it it does and false otherwise.

#### **std::string GetAttribute(const std::string &key) const noexcept override** ####
This function returns the attribute value of a given key. It does this through an unordered_map, checking if the key exists and returing its attribute value. 

## Functions

#### **bool ParseNodes(std::shared_ptr<CXMLReader> src, SXMLEntity &nextentity);**
This function is responsible for parsing all the nodes in an OSM file. It accepts a shared pointer to an CXMLReader to parse the OSM_XML file and a SXMLEntity to store the most recenly parsed entity, particularly the first SWay entity that we want ParseWays() to parse.

For parsing, it does a while loop, searching for a start Node entity. Once it does, it stores the Node information and pushes it into a vector and unordered_map for lookup. It then conducts another while loop, seraching for its corresponding end Node entity. If it sees a start Tag entity, it stores its attribute to the corresponding Node and continues the loop. Once it finds the end Node entity, it breaks out of the inner for loop and continues in the outer while loop. If the outer while loop encounters a start Way tag, it stores this Way tag in &nextentity for ParseWays() to parse and returns true to indicate successful parsing. If the outer while loop encounters an end OSM tag, it returns false to indicatee that the given file is an invalid OSM file as it doesn't have any ways (missing info).

#### **bool ParseWays(std::shared_ptr<CXMLReader> src, SXMLEntity &firstentity);**
This function is responsible for parsing all the ways in an OSM file. It accepts a shared pointer to an CXMLReader to parse the OSM_XML file and a SXMLEntity  returned by ParseNodes() that represents the first SWay entity in the OSM file.

For parsing, it does a do-while loop, first storing the Way provided in &firstentity byParseNodes(), then searching for a start Way entity. Once it does, it stores the Way information and pushes it into a vector and unordered_map for lookup. It then conducts another while loop, seraching for its corresponding end Way entity. If it sees a start Tag entity, it stores its attribute to the corresponding Node and continues the loop. If it sees a start Node tag (with Data name "nd"), it stores the node information in a vector and continues the loop. Once it finds the end Node entity, it breaks out of the inner for loop and checks if the given Way has at least two nodes; it continues in the outer while loop if yes, returns false and stops parsing if no. If the outer while loop encounters an end OSM tag, it returns true to indicate successful parsing (no more Ways to parse, reached EOF).

#### **bool ParseOpenStreetMap(std::shared_ptr<CXMLReader> src);**
This function is responsible for parsing the entire OSM file. It accepts a shared pointer to an CXMLReader to parse the OSM_XML file. It first creates a temporary SXMLEntity object which is used to store the most recently parsed entity. It then reads the first entity: if it's a starting tag and it's data name is "osm", then the function begins parsing the entier OSM file. It first calls ParseNodes to parse all the nodes (and all its attributes). If the parsing was successful, it then calls ParseWays to parse all ways (and all its attributes). If the parsing was successful, the function returns true, indicating success.

#### **std::size_t NodeCount() const noexcept override;**
This function returns the number of nodes that are in the parsed OSM file. It does this by storing each parsed node in a vector and returning the size of this vector.

#### **std::size_t WayCount() const noexcept override;**
This function returns the number of ways that are in the parsed OSM file. It does this by storing each parsed way in a vector and returning the size of this vector.

#### **std::shared_ptr\<CStreetMap::SNode\> NodeByIndex(std::size_t index) const noexcept override;**
This functions accepts an index, returning a shared pointer to the SNode object located at the given index of a vector.

#### **std::shared_ptr\<CCStreetMap::SNode\> NodeByID(TNodeID id) const noexcept override;**
This functions accepts an id, using that id as a key and returning its corresponding SNode object value via an unordered_map.

#### **std::shared_ptr\<CCStreetMap::SWay\> WayByIndex(std::size_t index) const noexcept override;**
This functions accepts an index, returning a shared pointer to the SWay object located at the given index of a vector. 

#### **std::shared_ptr\<CCStreetMap::SWay\> WayByID(TWayID id) const noexcept override;**
This functions accepts an id, using that id as a key and returning its corresponding SWay object value via an unordered_map.


