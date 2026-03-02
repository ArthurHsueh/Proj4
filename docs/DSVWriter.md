# DSVWriter

This class implements a writer for DSV data. It uses the CDataSink class to output formatted strings. The writer turns a vector of strings into a DSV row.

## Constructor

#### CDSVWriter(std::shared_ptr< CDataSink > sink, char delimiter, bool quoteall = false);
The Writer accepts a CDataSink object, a character delimiter, and a boolean that checks whether written data should be enclosed by double quotes. It also includes an if statement to check whether a double quote is specified as the delimiter, in which case the Reader handles it as a comma. 
 
## Functions

#### **bool WriteRow(const std::vector<std::string> &row)**
This function turns a vector of strings into a formatted DSV row and writes it to the data sink. 

The function first checks whether it is at the start of the file to prevent a newline being written at the very top of the file.

The function then loops through, inserting delimiters between each column of the row. It checks whether it is at the beginning or end of the row to ensure it doesn't place delimiters there. 

The function also checks whether a double quote has been found and adds another double quote next to it to ensure proper formatting.

The function also checks whether it needs to include double quotes if the quote boolean is true or if the string contains a delimiter, newline, or double quote. Then, it writes the string to the data sink and returns true when the entire row is committed. 