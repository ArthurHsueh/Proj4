#include <memory>
#include "XMLEntity.h"
#include "DataSource.h"
#include "XMLWriter.h"
#include "StringUtils.h"
#include <expat.h>
#include <queue>
#include <stack>
#include <vector>

struct CXMLWriter::SImplementation{
    std::shared_ptr< CDataSink > DSink; //Stores the string that we're going to modify
    std::stack<std::string> DStack; //Stores the Start tags for Flush(). LIFO ensures we output the most recent tag 

    SImplementation(std::shared_ptr<CDataSink> sink){ //Constructor accepts CDataSink as input
        DSink = sink;
    }

    bool Flush(){ 
        size_t size = DStack.size(); 
        for (int i = 0; i < size; i++){ //Loop through stack, outputting all remaining EndElement entities
            std::string name = DStack.top();
            DStack.pop();
            std::vector<char> Buffer(name.begin(), (name.end()));
            DSink->Put('<'); //Construct the EndElement entity
            DSink->Put('/'); 
            DSink->Write(Buffer);
            DSink->Put('>');
        }
        return true;
    }   

    bool WriteEntity(const SXMLEntity &entity){
        SXMLEntity entityCopy = entity; //Create copy of entity so we can modify it
        Convert(entityCopy.DNameData); //Ensure that we escape special characters 
        std::vector<char> Buffer((entityCopy.DNameData).begin(), (entityCopy.DNameData).end());

        if(entity.DType == SXMLEntity::EType::StartElement){ //Construct StartElement Entity in the form: <tag attr1="val1"...>
            DStack.push(entityCopy.DNameData); //Push into stack to establish the right order in which Flush() will output the EndElement entities

            DSink->Put('<');
            DSink->Write(Buffer); //Loops through the buffer and writes each character into the Sink
            if (!(entity.DAttributes).empty()){
                for (auto &attribute : entityCopy.DAttributes) //Loop through all attributes, writing them in the correct format 
                {
                    Convert(attribute.first); //Convert escape characters in each attribute
                    Convert(attribute.second);
                    std::vector<char> temp1((attribute.first).begin(), (attribute.first).end());
                    std::vector<char> temp2((attribute.second).begin(), (attribute.second).end());

                    DSink->Put(' ');
                    DSink->Write(temp1);
                    DSink->Put('=');
                    DSink->Put('"');
                    DSink->Write(temp2);
                    DSink->Put('"');
                }
            }
            DSink->Put('>');
            return true;
        }
        else if (entity.DType == SXMLEntity::EType::EndElement){ //Construct EndElement Entity in the form: </tag>
            if (!DStack.empty()) //Remove the appropriate StartElements since the EndElement is already parsed
            {
                DStack.pop(); 
            }

            DSink->Put('<');
            DSink->Put('/');
            DSink->Write(Buffer);
            DSink->Put('>');
            return true;
        }
        else if (entity.DType == SXMLEntity::EType::CharData){ //Construct CharData Entity directly
            DSink->Write(Buffer);
            return true;
        }
        else if (entity.DType == SXMLEntity::EType::CompleteElement){ //Construct CompleteElement Entity in the form: <tag attr1="val".../>
            DSink->Put('<');
            DSink->Write(Buffer);
            if (!(entity.DAttributes).empty()){ //Loop through all attributes, writing them in the correct format 
                for (auto &attribute : entityCopy.DAttributes)
                {
                    Convert(attribute.first); //Convert escape characters in each attribute
                    Convert(attribute.second);
                    std::vector<char> temp1((attribute.first).begin(), (attribute.first).end());
                    std::vector<char> temp2((attribute.second).begin(), (attribute.second).end());

                    DSink->Put(' ');
                    DSink->Write(temp1);
                    DSink->Put('=');
                    DSink->Put('"');
                    DSink->Write(temp2);
                    DSink->Put('"');
                }
            }
            DSink->Put('/');
            DSink->Put('>');
            return true;
        }
        return false;
    }

    void Convert(std::string &string){ //Helper function that converts all escape characters to their appropriate type
        string = StringUtils::Replace(string, "&", "&amp;");
        string = StringUtils::Replace(string, "<", "&lt;");
        string = StringUtils::Replace(string, ">", "&gt;");
        string = StringUtils::Replace(string, "'", "&apos;");
        string = StringUtils::Replace(string, "\"", "&quot;");
    }
    
};

CXMLWriter::CXMLWriter(std::shared_ptr< CDataSink > sink){
    DImplementation = std::make_unique<SImplementation>(sink);
}

CXMLWriter::~CXMLWriter(){
}

bool CXMLWriter::Flush(){
    return DImplementation->Flush();
}

bool CXMLWriter::WriteEntity(const SXMLEntity &entity){
    return DImplementation->WriteEntity(entity);
}
