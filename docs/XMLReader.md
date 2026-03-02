# XMLReader

This class implements a reader for XML files. It uses the CDataSource class to store the string and keep track of the index. Using EXPAT as a parser, the reader identifies three types of entities: 
1. StartElement
2. EndElement
3. CharacterData

## Constructor

#### CXMLReader(std::shared_ptr< CDataSource > src);
The Reader accepts a CDataSource object for its constructor, allowing it to parse through the given string. It also creates an XML_Parser through EXPAT to help parse the file.

## Functions


#### **bool End() const;**
This function returns true if the parser has reached the end of the file, or that the next read from the parser will return false. It does this by checking if the CDataSource is not empty.

#### **bool ReadEntity(SXMLEntity &entity, bool skipcdata = false);**
This function accepts XML entity and a boolean value skipcdata that tells the function whether to ignore CharacterData entities. It returns true on a successful read and false if there was no more left to read.
 
It manages the entities through a queue. If the queue is empty, it creates a buffer with fixed size. It reads from the CDataSource and stores the characters in the buffer. It then gives the data to the parser, which calls the appropriate callback functions. 

If the queue is not empty, it pops the entity from the front of the queue and assigns it to the passed in entity reference. If skipcdata was true, it loops through the queue until if finds the first non-CharacterData entity and assigns it to the passed in entity reference. 