#include "DSVReader.h"
#include "StringUtils.h" //Reference string functions from Project 1 if needed

struct CDSVReader::SImplementation{
    std::shared_ptr< CDataSource > DSrc; //Stores the string that we're reading from
    char DDelimeter; //Stores the delimeter

    SImplementation(std::shared_ptr< CDataSource > src, char delimiter){
        DSrc = src;
        if (delimiter == '"'){ //The delimiter " is treated as ,
            delimiter = ',';
        }
        DDelimeter = delimiter;
    }

    ~SImplementation(){

    }

    bool End() const{ //End of DataSource, no more string to be read
        return DSrc->End();
    }

    bool ReadRow(std::vector<std::string> &row){
        row.clear(); //Clear existing data in the row
        
        std::string temp; //Temporary string used to construct each element 
        char c; //Temporary character used to traverse the string
        bool quote = false; //Indicates whether we are inside a quote

        if (DSrc->End()){ //If the string is empty, we don't read any row
            return false;
        }

        while (DSrc->Get(c)){ //While there are still characters to get form the string
            if (c == '"'){ //Special rules when handling double quote
                if (!quote){ //We are now inside a double quote, so newline chracters, delimiters, and more quotes are expected
                    quote = true;
                    continue;
                }
                else{ 
                    char next;
                    DSrc->Peek(next); 
                    if (next == '"'){ //If there are two doulbe quotes in a row, we append a single double quote to the temp string
                        if (DSrc->Get(c)){
                            temp += '"';
                        }   
                    }
                    else{ //Else, we exited the quote and proceed with the rest of the string
                        quote = false;
                    }
                }   
            }
            else if (c == '\n' && quote == false){ //If there is a newline character and we are not inside a quote, we push the temp string into the vector
                row.push_back(temp);
                return true;
            }
            else if (c == DDelimeter && quote == false){ //If there is a delimiter and we are not inside a quote, we push the temp string into the vector and clear it for the next
                row.push_back(temp);
                temp.clear();
            }
            else{ //A regular character that we can just append to the string
                temp += c;
            }

        }
        row.push_back(temp); //Any remaining characters are pushed into the vector
        return true;
    }
    
};

CDSVReader::CDSVReader(std::shared_ptr< CDataSource > src, char delimiter){
    DImplementation = std::make_unique<SImplementation>(src, delimiter);
}

CDSVReader::~CDSVReader(){

}

bool CDSVReader::End() const{
    return DImplementation->End();
}

bool CDSVReader::ReadRow(std::vector<std::string> &row){
    return DImplementation->ReadRow(row);
}




/*
Can't distinguish between no entity and empty string line
Reader would interpret as one row ""
First time write row, but just put value
Every time else you put a new line and then put a value
*/