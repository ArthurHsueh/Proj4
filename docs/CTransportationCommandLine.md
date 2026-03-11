# CTransportationCommandLine

This class implements a CommandLine program that allows the user to parse information from the command line. The program interacts with the user through prompts, where the user is able to retrieve information on nodes, fastest/shortest path, and description. The user is able to save this information through a CDataSource. The program also implements a help screen and error messages to help users navigate the progarm effectively. 
## Constructor

#### **CTransportationPlannerCommandLine(std::shared_ptr<CDataSource> cmdsrc, std::shared_ptr<CDataSink> outsink, std::shared_ptr<CDataSink> errsink, std::shared_ptr<CDataFactory> results, std::shared_ptr<CTransportationPlanner> planner);**

The program accepts shared pointers to one CDataSource, two CDataSink, one CDataFactory, and one CTransportationPlanners to parse command line arguments, write to output, write to error, create files, and access transportation information.

## Helper Functions

#### **void Help();**

This function writes the help menu to the OutputSink

#### **bool Count();**
This function wrtie the number of node the OutputSink 
#### **bool Node(CTransportationPlanner::TNodeID index);**
This function accepts an ID, writing its corresponding node's ID and location to the OutputSink. 
#### **bool Shortest(CTransportationPlanner::TNodeID source, CTransportationPlanner::TNodeID destination);**
This function accepts the ID's of a source and destination node, finding the shortest path between them and writing the shortest distance to the OutputSink. 
#### **bool Fastest((CTransportationPlanner::TNodeID source, CTransportationPlanner::TNodeID destination));**
This function accepts the ID's of a source and destination node, finding the fastest path between them and writing the shortest time to the OutputSink. 
#### **bool Print();**
This function writes the path description to the OutputSink. If no information is defined (no path processed), it writes to the ErrorSink
#### **bool Save();**
This functions creates a file to save the most recent path, writing the saved path file to the OutputSink. If no path is definen, it writes to the ErrorSink
#### **bool Error();**
This function processes any invalid commands, writing to the ErrorSink about the unknown command

## Function(s)

#### **bool ProcessCommands();**
The main function that drives the code. It loops through the CDataSource, reading each command by parsing the entire line and stopping when it reads a newline character. From the command, it splits it based on space, storing it in a vector. It then calls the appropriate helper function based on the parameters presented. If any parameter is missing or is invalid, it writes to ErrorSink. If the user writes "exit", the program terminates.  