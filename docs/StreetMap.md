# OpenMap

This abstract class (interface) defines the SNode and SWay structs used to store information about the nodes and ways that we parse in an osm file. Each struct contains various virtual functions, allowing the user to retrieve specific nodes and ways as well as information about their attributes, id's, and locations (only for nodes). The interface also consists of its own virtal functions that provides information about nodes and says in general such as count.

## Constructor

This abstract class has no explicit constructor

## Structs

## struct SNode: public CStreetMap::SNode##

#### **virtual TNodeID ID() const noexcept = 0;** ####
Returns the Node's ID.

#### **virtual TLocation Location() const noexcept = 0;** ####
Returns the node's location (lat, long).

#### **virtual std::size_t AttributeCount() const noexcept = 0;** ####
Returns the number of attributes in the node.

#### **virtual std::string GetAttributeKey(std::size_t index) const noexcept = 0;** ####
Returns the attribute key at a given index.

#### **virtual bool HasAttribute(const std::string &key) const noexcept = 0;** ####
Returns true if the attribute key exists.

#### **virtual std::string GetAttribute(const std::string &key) const noexcept = 0;** ####
Returns the value at the given attribute key.


## struct SWay: public CStreetMap::SWay ##

#### **virtual TWayID ID() const noexcept = 0;** ####
Returns the Way's ID.

#### **virtual std::size_t NodeCount() const noexcept = 0;** ####
Returns the number of nodes in the way.

#### **virtual TNodeID GetNodeID(std::size_t index) const noexcept = 0;** ####
Returns the node id at a given index

#### **virtual std::size_t AttributeCount() const noexcept = 0;** ####
Returns the number of attributes in the node.

#### **virtual std::string GetAttributeKey(std::size_t index) const noexcept = 0;** ####
Returns the attribute key at a given index.

#### **virtual bool HasAttribute(const std::string &key) const noexcept = 0;** ####
Returns true if the attribute key exists.

#### **virtual std::string GetAttribute(const std::string &key) const noexcept = 0;** ####
Returns the value at the given attribute key.


## Functions

#### **virtual std::size_t NodeCount() const noexcept = 0;**
This functions returns the number of nodes in the osm file

#### **virtual std::size_t WayCount() const noexcept = 0;**
This function returns the number of ways that are in the parsed OSM file


#### **virtual std::shared_ptr\<SNode\> NodeByIndex(std::size_t index) const noexcept = 0;**
This functions accepts an index, returning a shared pointer to the SNode object located at the given index of a vector. 

#### **virtual std::shared_ptr\<SNode\> NodeByID(TNodeID id) const noexcept = 0;**
This functions accepts an id, using that id as a key and returning its corresponding SNode object value via an unordered_map

#### **svirtual std::shared_ptr\<SWay\> WayByIndex(std::size_t index) const noexcept = 0;**
This functions accepts an index, returning a shared pointer to the SWay object located at the given index of a vector. 

#### **virtual std::shared_ptr\<SWay\> WayByID(TWayID id) const noexcept = 0;;**
This functions accepts an id, using that id as a key and returning its corresponding SWay object value via an unordered_map
