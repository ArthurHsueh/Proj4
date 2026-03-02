# XMLWriter

This class implements a reader for XML files. It uses the CDataSink class to store and manipulate string for returning. The writer returns a string of the entity in correct XML format.

## Constructor

#### CXMLWriter(std::shared_ptr< CDataSink > sink);
The Writer accepts a CDataSink object for its constructor, allowing it to write and directly output the formatted string.

## Functions

#### **bool Flush();**
This function outputs the appropriate End tags for those that have been started. This is useful for writing out XML files that have a large number of Start and End tags and want to output the rest of the End tags in one line.

#### **bool WriteEntity(const SXMLEntity &entity);**
This function accepts an XML Entity as an input. It first creates a copy of the entity and runs an internal Convert() function, which uses the Replace() function from StringUtils.h. It translates 5 special characters that require escaping:
1. "<" -> "&lt;" 
2. ">" -> "&gt;" 
3. "&" -> "&amp;"
4. "'" -> "&apos;" 
5. '"' -> "&quot;"

It then stores the string in a buffer to help with parsing.

If the Entity type is StartElement, it appends "<", followed by the NameData, attributes (in pairs, where the arguments are separated by "=" and the second argument is quoted), and ">", forming the complete StartElement tag. It then returns true to indicate success. 
 
If the Entity type is EndElement, it appends "<", followed by the NameData, "/", and ">", forming the complete EndElement tag. It then returns true to indicate success. 
 
If the Entity type is CharData, it writes the NameData directly to the string. It then returns true to indicate success. 

If the Entity type is CompleteElement, it appends "<", followed by the NameData, attributes (in pairs, where the arguments are separated by "=" and the second argument is quoted), "/", and ">". It then returns true to indicate sentence.