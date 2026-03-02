# DSVReader

This class implements a reader for DSV data. It uses the CDataSource class to manage input data and provides an interface to extract rows. 

## Constructor

#### CDSVReader(std::shared_ptr< CDataSource > src, char delimiter);
The Reader accepts a CDataSource object and a delimiter for its constructor, letting it parse through a given file or string. It also includes an if statement to check whether a double quote is specified as the delimiter, in which case the Reader handles it as a comma.
 
## Functions

#### **bool End() const;**
This function checks if the parser has reached the end of the file. It returns true if there is no data to be processed, and false if more characters are available. It does this by checking if CDataSource is empty.

#### **bool ReadRow(std::vector\<std::string\> &row);**
This function extracts values from the data source into a vector. It returns true if a row was parsed through, and false if no row could be read.

The function first checks if the file is empty, returning false if so. 
Then, it creates a while loop to search through each character of the row.

It handles delimiters, double quotes, or newlines within double quotes by setting quote = true if it detects a double quote, and setting quote = false if the next character isn't a double quote. 

If the function reaches a delimiter while not in a double quote, it takes the characters it has gathered in temp, pushes them to a vector, and clears temp.

If the function reaches a newline \n while not in a double quote, it pushes the characters it has gathered to a vector and returns true. 