#include "TransportationPlannerCommandLine.h"
#include "StringUtils.h"
#include "GeographicUtils.h"
#include "TransportationPlanner.h"
#include <sstream>
#include <iomanip>


struct CTransportationPlannerCommandLine::SImplementation{
    std::shared_ptr<CDataSource> DCommandDataSource;
    std::shared_ptr<CDataSink> DOutputDataSink;
    std::shared_ptr<CDataSink> DErrorDataSink;
    std::shared_ptr<CDataFactory> DResultsDataFactory;
    std::shared_ptr<CTransportationPlanner> DTransportationPlanner;

    std::vector<CTransportationPlanner::TNodeID> path; //Store the latest shortests path
    std::vector<CTransportationPlanner::TTripStep> steps; //Store the latest fastest steps
    CTransportationPlanner::TNodeID src; //Store the latest src node
    CTransportationPlanner::TNodeID dest; //Store the latest dest node

    double time; //Store the latest fastest time 

    SImplementation(std::shared_ptr<CDataSource> cmdsrc, std::shared_ptr<CDataSink> outsink, std::shared_ptr<CDataSink> errsink, std::shared_ptr<CDataFactory> results, std::shared_ptr<CTransportationPlanner> planner){
        DCommandDataSource = cmdsrc;
        DOutputDataSink = outsink;
        DErrorDataSink = errsink;
        DResultsDataFactory = results;
        DTransportationPlanner = planner;
    }

    void Help(){ //Writes to the OutputSink the Help screen
        std::string outputstring;
        outputstring =      "------------------------------------------------------------------------\n"
                            "help     Display this help menu\n"
                            "exit     Exit the program\n"
                            "count    Output the number of nodes in the map\n"
                            "node     Syntax \"node [0, count)\" \n"
                            "         Will output node ID and Lat/Lon for node\n"
                            "fastest  Syntax \"fastest start end\" \n"
                            "         Calculates the time for fastest path from start to end\n"
                            "shortest Syntax \"shortest start end\" \n"
                            "         Calculates the distance for the shortest path from start to end\n"
                            "save     Saves the last calculated path to file\n"
                            "print    Prints the steps for the last calculated path\n";

        for(char c: outputstring){
            DOutputDataSink->Put(c);
        }
    }

    void Count(){ //Writes to the OutputSink the number of nodes
        int count = DTransportationPlanner->NodeCount(); //Retrieve how amny nodes are stored in the vector
        std::string outputstring = std::to_string(count) + " nodes\n";

        for(char c: outputstring){
            DOutputDataSink->Put(c);
        }
    }

    void Node(CTransportationPlanner::TNodeID index){ //Writes to the OutputSink the Node id and location, given an index
        auto node = DTransportationPlanner->SortedNodeByIndex(index); //Retrieve the node by index
        auto id = (node->ID()); 
        auto location = SGeographicUtils::ConvertLLToDMS(node->Location()); //Use GeographicalUtils to convert from long/lat
        std::string outputstring = "Node " + std::to_string(index) + ": id = " + std::to_string(node->ID()) + " is at " + location + "\n";

        for(char c: outputstring){
            DOutputDataSink->Put(c);
        }
    }

    void Shortest(CTransportationPlanner::TNodeID source, CTransportationPlanner::TNodeID destination){ //Writes to the OutputSink the shortest path
        src = source;
        dest = destination;
        auto distance = DTransportationPlanner->FindShortestPath(src, dest, path); //Calls Dijkstra's on the graph
        std::stringstream stream;
        stream << std::fixed << std::setprecision(1) << distance; //1 decimal point
        std::string outputstring = "Shortest path is " + stream.str() + " mi." + "\n";

        for(char c: outputstring){
            DOutputDataSink->Put(c);
        }
    }

    void Fastest(CTransportationPlanner::TNodeID source, CTransportationPlanner::TNodeID destination){ //Writes to the OutputSink the fastest path
        src = source;
        dest = destination;
        time = DTransportationPlanner->FindFastestPath(src, dest, steps);
    
        int hours = static_cast<int>(time); //Convert to hours (int rounding)
        int minutes = static_cast<int>((time - hours) * 60); //Convert to minutes (int rouding)
        int seconds = static_cast<int>(((time - hours) * 60 - minutes) * 60); //Convert to seconds (int rounding)

        std::string outputstring;
        if (hours == 0 && seconds == 0){ //If no hours and seconds, just write the minutes directly
            outputstring = "Fastest path takes " + std::to_string(minutes) + " min." + "\n";
        }
        else{ //If hours, minutes, and seconds all exist, write all of their data 
            outputstring = "Fastest path takes " + std::to_string(hours) + " hr " + std::to_string(minutes) + " min " + std::to_string(seconds) + " sec." + "\n";
        }
        for(char c: outputstring){
            DOutputDataSink->Put(c);
        }
    }

    void Print(){ //Writes to the OutputSink information about the path
        std::vector<std::string> desc;
        if (steps.empty()){ //Writes to the ErrorSink if no path is defined
            std::string outputstring = "No valid path to print, see help.\n";
            for(char c: outputstring){
                DErrorDataSink->Put(c);
            }
        }
        else{
            DTransportationPlanner->GetPathDescription(steps, desc); //Gete the PathDescription and write to the OutputWink
            for(std::string s: desc){
                for(char c: s){
                    DOutputDataSink->Put(c);
                }
                DOutputDataSink->Put('\n');
            }
        }
    }

    void Save(){ //Creates a file to save the path, then writes to the OutputSink the saved path file
        if (steps.empty()){ //Writes to the ErrorSink if no path is defined
            std::string outputstring = "No valid path to save, see help.\n";
            for(char c: outputstring){
                DErrorDataSink->Put(c);
            }
        }
        else{
            std::stringstream stream;
            stream << std::fixed << std::setprecision(6) << time; //6 decimal points
            std::string name = std::to_string(src) + "_" + std::to_string(dest) + "_" + stream.str() + "hr.csv"; //File name contains the src node, dest, nodes, and the time it takes to traverse the path
            auto Sink = DResultsDataFactory->CreateSink(name); //Creates a DataSink to store the strings
            std::string sinkstring = "mode,node_id";
            for(char c: sinkstring){
                Sink->Put(c);
            }

            for(auto step: steps){ //Detects which method of transportation is being used, returing a string so we can write to 
                std::string mode;
                if (step.first == CTransportationPlanner::ETransportationMode::Walk){
                    mode = "Walk";
                }
                else if (step.first == CTransportationPlanner::ETransportationMode::Bike){
                    mode = "Bike";
                }
                else{
                    mode = "Bus";
                }
                
                sinkstring = "\n" + mode + "," + std::to_string(step.second);
                for(char c : sinkstring){
                    Sink->Put(c);
                }
            }

            std::string outputstring = "Path saved to <results>/" + name + "\n"; //Writes to the OutputSink the file name
            for(char c: outputstring){
                DOutputDataSink->Put(c);
            }
        }
    }

    void Error(std::string error){ //Writes to the ErrorSink when encountering an invalid command
        std::string outputstring = "Unknown command \"" + error + "\" type help for help." + "\n";
        
        for(char c: outputstring){
            DErrorDataSink->Put(c);
        }
    }

    bool ProcessCommands(){ //Main code for detecting the different commands
        char ch;
        std::string command;

        while (!DCommandDataSource->End()){
            command.clear();
            DOutputDataSink->Put('>'); //"> " for each command prompt
            DOutputDataSink->Put(' ');

            while(DCommandDataSource->Get(ch)){ //Keep parsing in the characters until we hit new line
                if(ch == '\n'){
                    break;
                }
                command += ch;
            }
            std::vector<std::string> commandsplit = StringUtils::Split(command); //Split the command by space, allowing us to parse the parameters

            if (commandsplit[0] == "exit"){  //If command is exit, we terminate the program
                return true;
            }

            if (commandsplit[0] == "count"){ //If command is count, call the Count() function
                Count();
                continue;
            }

            if (commandsplit[0] == "help"){ //If command is help, call the Help() function
                Help();
                continue;
            }

            if (commandsplit[0] == "node"){ //If command is node, check if a valid node paramter exists; if yes, call Node() and write to ErrorSink otherwise
                if(commandsplit.size() < 2){
                    std::string outputstring = "Invalid node command, see help.\n";
                    for(char c: outputstring){
                        DErrorDataSink->Put(c);
                    }
                    continue;
                }
                
                int id;
                try{
                    id = std::stoi(commandsplit[1]);
                }
                catch(const std::invalid_argument&){ //Try and see if the node paramter is an int by catching the error during conversion
                    std::string outputstring = "Invalid node parameter, see help.\n";
                    for(char c: outputstring){
                        DErrorDataSink->Put(c);
                    }
                    continue;
                }

                Node(id);
                continue;
            }

            if (commandsplit[0] == "shortest"){ //If command is shortest, check if a valid shortest parameter exists; if yes, call shortest() and write to ErrorSink otherwise
                path.clear();
                if(commandsplit.size() < 2){
                    std::string outputstring = "Invalid shortest command, see help.\n";
                    for(char c: outputstring){
                        DErrorDataSink->Put(c);
                    }
                    continue;
                }
                
                CTransportationPlanner::TNodeID s;
                CTransportationPlanner::TNodeID d;
                try{
                    s = std::stoi(commandsplit[1]);
                }
                catch(const std::invalid_argument&){ //Try and see if the first Shortest parameter is an int by catching the error during conversion
                    std::string outputstring = "Invalid shortest parameter, see help.\n";
                    for(char c: outputstring){
                        DErrorDataSink->Put(c);
                    }
                    continue;
                }
                try{
                    d = std::stoi(commandsplit[2]); //Try and see if the second Shortest parameter is an int by catching the error during conversion
                }
                catch(const std::invalid_argument&){
                    std::string outputstring = "Invalid shortest parameter, see help.\n";
                    for(char c: outputstring){
                        DErrorDataSink->Put(c);
                    }
                    continue;
                }

                Shortest(s, d);
                continue;
            }

            if (commandsplit[0] == "fastest"){  //If command is fastest, check if a valid shortest parameter exists; if yes, call fastest() and write to ErrorSink otherwise
                steps.clear();
                if(commandsplit.size() < 2){
                    std::string outputstring = "Invalid fastest command, see help.\n";
                    for(char c: outputstring){
                        DErrorDataSink->Put(c);
                    }
                    continue;
                }

                CTransportationPlanner::TNodeID s;
                CTransportationPlanner::TNodeID d;
                try{
                    s = std::stoi(commandsplit[1]);
                }
                catch(const std::invalid_argument&){ //Try and see if the first Fastest parameter is an int by catching the error during conversion
                    std::string outputstring = "Invalid fastest parameter, see help.\n";
                    for(char c: outputstring){
                        DErrorDataSink->Put(c);
                    }
                    continue;
                }
                try{
                    d = std::stoi(commandsplit[2]);
                }
                catch(const std::invalid_argument&){ //Try and see if the first Fastest parameter is an int by catching the error during conversion
                    std::string outputstring = "Invalid fastest parameter, see help.\n";
                    for(char c: outputstring){
                        DErrorDataSink->Put(c);
                    }
                    continue;
                }

                Fastest(s, d);
                continue;
            }   
        
            if (commandsplit[0] == "print"){ //If command is print, call the Print() function
                Print();
                continue;
            }

            if (commandsplit[0] == "save"){ //If command is save, call the Save() function
                Save();
                continue;
            }

            else{ //If the command is invalid, call the Error() function, call the Error() function
                Error(commandsplit[0]);
                continue;
            }
        }
        return true; //Return true on success 
    }
};

CTransportationPlannerCommandLine::CTransportationPlannerCommandLine(std::shared_ptr<CDataSource> cmdsrc, std::shared_ptr<CDataSink> outsink, std::shared_ptr<CDataSink> errsink, std::shared_ptr<CDataFactory> results, std::shared_ptr<CTransportationPlanner> planner){
    DImplementation = std::make_unique<SImplementation>(cmdsrc, outsink, errsink, results, planner);
}

CTransportationPlannerCommandLine::~CTransportationPlannerCommandLine(){

}

bool CTransportationPlannerCommandLine::ProcessCommands(){
    return DImplementation->ProcessCommands();
}