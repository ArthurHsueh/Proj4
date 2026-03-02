#include <memory>
#include "XMLEntity.h"
#include "DataSource.h"
#include "XMLReader.h"
#include <expat.h>
#include <queue>
#include <vector>

struct CXMLReader::SImplementation{
    std::shared_ptr<CDataSource> DSource; //Stores the string that we're reading from
    XML_Parser DParser; //Expat Parser to parse the data
    std::queue<SXMLEntity> DQueue; //Stores entities whenever callback functions are called. When we call WriteEntity, we pop from queue, and FIFO outputs in the order we read the tags

    static void StartElementHandler(void *data, const XML_Char *name, const XML_Char **atts){ //Callback function for handling StartElement
        SImplementation *This = (SImplementation *)data; //Tells the parser which object it belongs to whenver callback function is called
        SXMLEntity NewEntity;
        NewEntity.DType = SXMLEntity::EType::StartElement; 
        NewEntity.DNameData = name;
        for(int i = 0; atts[i]; i += 2){ //Construct the attributes in key, value pairs
            NewEntity.DAttributes.push_back({atts[i], atts[i+1]}); 
        }
        This->DQueue.push(NewEntity); //Push the Entity into the Queue
    }

    static void EndElementHandler(void *data, const XML_Char *name){ //Callback function for handling EndElement
        SImplementation *This = (SImplementation *)data;
        SXMLEntity NewEntity;
        NewEntity.DType = SXMLEntity::EType::EndElement; 
        NewEntity.DNameData = name;
        This->DQueue.push(NewEntity);
    }

    static void CharacterDataHandler(void *data, const XML_Char *s, int len){ //Callback function for handling CharacterData
        SImplementation *This = (SImplementation *)data;
        std::string string = std::string(s, len); //s is not null terminated, so we have to use a different string construction
        SXMLEntity &entity = This->DQueue.back();
        if (entity.DType == SXMLEntity::EType::CharData){ //Since parser doesn't guarantee outputing string in one call, we have to make sure we're not pushing another CharData and intead appending
            entity.DNameData += string;
        }
        else{ //if last element is not CharData, we can safetly push the entity into the queue 
            SXMLEntity NewEntity;
            NewEntity.DType = SXMLEntity::EType::CharData;
            NewEntity.DNameData = string;
            This->DQueue.push(NewEntity);
        }
    }

    SImplementation(std::shared_ptr<CDataSource> src){
        DSource = src;
        DParser = XML_ParserCreate(nullptr); //Creating the Parser
        XML_SetUserData(DParser, (void*)this); //Provide parser with "this" pointer so static callback functions can operate on this object
        XML_SetStartElementHandler(DParser, StartElementHandler); //Assigning our callback functions to the parser for StartElement
        XML_SetEndElementHandler(DParser, EndElementHandler); //Assigning our callback functions to the parser for EndElement
        XML_SetCharacterDataHandler(DParser, CharacterDataHandler); //Assigning our callback functions to the parser for CharacterData

    }

    ~SImplementation(){  //Free the Parser once finished
        if (DParser){ 
            XML_ParserFree(DParser);
        }
    }

    bool End() const{ //End of DataSource and the queue is empty. Return true if ReadEntity reads false next time
        if (DSource->End() && DQueue.empty()){
            return true;
        }
        return false;
    }   

    bool ReadEntity(SXMLEntity &entity, bool skipcdata){
        if (DQueue.empty() && !DSource->End()){ //Check if there are elements in the queue and that we haven't reached the end of the XML file 
            std::vector<char> Buffer(512); //Allows us to read from DataSource, read source into a buffer
            DSource->Read(Buffer, Buffer.size());  //Be able to read 512, but we might not get 512
            XML_Parse(DParser, Buffer.data(), Buffer.size(), DSource->End()); //Give data to Expat to parse through and call appropriate callback functions
        }

        if (!DQueue.empty()){ //If there's entities in the queue, pop the front one and assign it to the entity reference
            entity = DQueue.front();
            DQueue.pop();

            if (skipcdata){ //If we skip data, we loop through the queue until the first non-CharData entity
                while (entity.DType == SXMLEntity::EType::CharData){
                    if (!DQueue.empty()){
                        entity = DQueue.front();
                        DQueue.pop();
                    }
                }
            }
            return true; 
        }
        return false;
    }
};

CXMLReader::CXMLReader(std::shared_ptr<CDataSource> src){
    DImplementation = std::make_unique<SImplementation>(src);
}


CXMLReader::~CXMLReader(){
    
}

bool CXMLReader::End() const{
    return DImplementation->End();
}

bool CXMLReader::ReadEntity(SXMLEntity &entity, bool skipcdata){
    return DImplementation->ReadEntity(entity, skipcdata);
}




