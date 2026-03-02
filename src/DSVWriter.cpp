#include "DSVWriter.h"

struct CDSVWriter::SImplementation{
    std::shared_ptr< CDataSink > DSink; //Stores the string that we're going to modify
    char DDelimiter; //Stores the delimiter 
    bool DQuoteAll; //Indicates if have to quote all strings
    bool DFirstLine; //Check if we're on the first line in a multi-row DSV file

    SImplementation(std::shared_ptr< CDataSink > sink, char delimiter, bool quoteall){
        DSink = sink;
        if (delimiter == '"'){ //The delimiter " is treated as ,
            delimiter = ',';
        }
        DDelimiter = delimiter;
        DQuoteAll = quoteall;
        DFirstLine = false; //Don't put newline if there's only one line; only do so if there are mutliple lines
    }

    ~SImplementation(){

    }

    bool WriteRow(const std::vector<std::string> &row){
        if (DFirstLine == true){ //If we're not on the first line, we append a newline character  
            DSink->Put('\n');
        }
        if (DFirstLine == false){ //If we're on the first line, then subsequent lines we have to add a newline chracter
            DFirstLine = true;
        }

        bool First = true;
        for (auto &Column : row){ //Loop through each element in the vector, outputting each string in the right format
            std::vector<char> Buffer(Column.begin(), Column.end());
            if(!First){ //Only put delimiters after the first element
                DSink->Put(DDelimiter);
            }
            First = false;

            for (size_t i = 0; i < Buffer.size(); i++){ //Convert each " into ""
                if (Buffer[i] == '"'){
                    Buffer.insert(Buffer.begin() + i, '"');
                    i++;
                }
            }
        
            if (Column.find(DDelimiter) != std::string::npos || Column.find('\n') != std::string::npos || Column.find('"') != std::string::npos || DQuoteAll){ //If the delimiter, newline character, doublequotes, or the QuoteAll parameter is true, we quote the string
                DSink->Put('"');
                DSink->Write(Buffer);
                DSink->Put('"');
            }
            else{ //else, we just write the string directly
                DSink->Write(Buffer);
            }
        }
        return true;
    }

};

CDSVWriter::CDSVWriter(std::shared_ptr< CDataSink > sink, char delimiter, bool quoteall){
    DImplementation = std::make_unique<SImplementation>(sink,delimiter,quoteall);
}

CDSVWriter::~CDSVWriter(){

}

bool CDSVWriter::WriteRow(const std::vector<std::string> &row){
    return DImplementation->WriteRow(row);
}

