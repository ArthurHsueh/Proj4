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

    std::vector<CTransportationPlanner::TNodeID> path;
    std::vector<CTransportationPlanner::TTripStep> steps;
    CTransportationPlanner::TNodeID src;
    CTransportationPlanner::TNodeID dest;

    double time;

    SImplementation(std::shared_ptr<CDataSource> cmdsrc, std::shared_ptr<CDataSink> outsink, std::shared_ptr<CDataSink> errsink, std::shared_ptr<CDataFactory> results, std::shared_ptr<CTransportationPlanner> planner){
        DCommandDataSource = cmdsrc;
        DOutputDataSink = outsink;
        DErrorDataSink = errsink;
        DResultsDataFactory = results;
        DTransportationPlanner = planner;
    }

    void Help(){
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

    void Count(){
        int count = DTransportationPlanner->NodeCount();
        std::string outputstring = std::to_string(count) + " nodes\n";

        for(char c: outputstring){
            DOutputDataSink->Put(c);
        }
    }

    void Node(CTransportationPlanner::TNodeID index){
        auto node = DTransportationPlanner->SortedNodeByIndex(index);
        auto id = (node->ID());
        auto location = SGeographicUtils::ConvertLLToDMS(node->Location());
        std::string outputstring = "Node " + std::to_string(index) + ": id = " + std::to_string(node->ID()) + " is at " + location + "\n";

        for(char c: outputstring){
            DOutputDataSink->Put(c);
        }
    }

    void Shortest(CTransportationPlanner::TNodeID source, CTransportationPlanner::TNodeID destination){
        src = source;
        dest = destination;
        auto distance = DTransportationPlanner->FindShortestPath(src, dest, path);
        std::stringstream stream;
        stream << std::fixed << std::setprecision(1) << distance;
        std::string outputstring = "Shortest path is " + stream.str() + " mi." + "\n";

        for(char c: outputstring){
            DOutputDataSink->Put(c);
        }
    }

    void Fastest(CTransportationPlanner::TNodeID source, CTransportationPlanner::TNodeID destination){
        src = source;
        dest = destination;
        time = DTransportationPlanner->FindFastestPath(src, dest, steps);
    
        int hours = static_cast<int>(time);
        int minutes = static_cast<int>((time - hours) * 60);
        int seconds = static_cast<int>(((time - hours) * 60 - minutes) * 60);

        std::string outputstring;
        if (hours == 0 && seconds == 0){
            outputstring = "Fastest path takes " + std::to_string(minutes) + " min." + "\n";
        }
        else{
            outputstring = "Fastest path takes " + std::to_string(hours) + " hr " + std::to_string(minutes) + " min " + std::to_string(seconds) + " sec." + "\n";
        }
        for(char c: outputstring){
            DOutputDataSink->Put(c);
        }
    }

    void Print(){
        std::vector<std::string> desc;
        if (steps.empty()){
            std::string outputstring = "No valid path to print, see help.\n";
            for(char c: outputstring){
                DErrorDataSink->Put(c);
            }
        }
        else{
            DTransportationPlanner->GetPathDescription(steps, desc);
            for(std::string s: desc){
                for(char c: s){
                    DOutputDataSink->Put(c);
                }
                DOutputDataSink->Put('\n');
            }
        }
    }

    void Save(){
        if (steps.empty()){
            std::string outputstring = "No valid path to save, see help.\n";
            for(char c: outputstring){
                DErrorDataSink->Put(c);
            }
        }
        else{
            std::stringstream stream;
            stream << std::fixed << std::setprecision(6) << time;
            std::string name = std::to_string(src) + "_" + std::to_string(dest) + "_" + stream.str() + "hr.csv";
            auto Sink = DResultsDataFactory->CreateSink(name);
            std::string sinkstring = "mode,node_id";
            for(char c: sinkstring){
                Sink->Put(c);
            }

            for(auto step: steps){
                std::string mode;
                if (step.first == CTransportationPlanner::ETransportationMode::Walk){
                    mode = "Walk";
                }
                else if (step.first == CTransportationPlanner::ETransportationMode::Walk){
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

            std::string outputstring = "Path saved to <results>/" + name + "\n";
            for(char c: outputstring){
                DOutputDataSink->Put(c);
            }
        }
    }

    void Error(std::string error){
        std::string outputstring = "Unknown command \"" + error + "\" type help for help." + "\n";
        
        for(char c: outputstring){
            DErrorDataSink->Put(c);
        }
    }

    bool ProcessCommands(){
        char ch;
        std::string command;

        while (!DCommandDataSource->End()){
            command.clear();
            DOutputDataSink->Put('>');
            DOutputDataSink->Put(' ');

            while(DCommandDataSource->Get(ch)){
                if(ch == '\n'){
                    break;
                }
                command += ch;
            }
            std::vector<std::string> commandsplit = StringUtils::Split(command);

            if (commandsplit[0] == "exit"){
                return true;
            }

            if (commandsplit[0] == "count"){
                Count();
                continue;
            }

            if (commandsplit[0] == "help"){
                Help();
                continue;
            }

            if (commandsplit[0] == "node"){
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
                catch(const std::invalid_argument&){
                    std::string outputstring = "Invalid node parameter, see help.\n";
                    for(char c: outputstring){
                        DErrorDataSink->Put(c);
                    }
                    continue;
                }

                Node(id);
                continue;
            }

            if (commandsplit[0] == "shortest"){
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
                catch(const std::invalid_argument&){
                    std::string outputstring = "Invalid shortest parameter, see help.\n";
                    for(char c: outputstring){
                        DErrorDataSink->Put(c);
                    }
                    continue;
                }
                try{
                    d = std::stoi(commandsplit[2]);
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

            if (commandsplit[0] == "fastest"){
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
                catch(const std::invalid_argument&){
                    std::string outputstring = "Invalid fastest parameter, see help.\n";
                    for(char c: outputstring){
                        DErrorDataSink->Put(c);
                    }
                    continue;
                }
                try{
                    d = std::stoi(commandsplit[2]);
                }
                catch(const std::invalid_argument&){
                    std::string outputstring = "Invalid fastest parameter, see help.\n";
                    for(char c: outputstring){
                        DErrorDataSink->Put(c);
                    }
                    continue;
                }

                Fastest(s, d);
                continue;
            }   
        
            if (commandsplit[0] == "print"){
                Print();
                continue;
            }

            if (commandsplit[0] == "save"){
                Save();
                continue;
            }

            else{
                Error(commandsplit[0]);
                continue;
            }
        }
        return true;
    }
};

CTransportationPlannerCommandLine::CTransportationPlannerCommandLine(
        std::shared_ptr<CDataSource> cmdsrc,
        std::shared_ptr<CDataSink> outsink,
        std::shared_ptr<CDataSink> errsink,
        std::shared_ptr<CDataFactory> results,
        std::shared_ptr<CTransportationPlanner> planner){

    DImplementation = std::make_unique<SImplementation>(cmdsrc, outsink, errsink, results, planner);
}

CTransportationPlannerCommandLine::~CTransportationPlannerCommandLine(){

}

bool CTransportationPlannerCommandLine::ProcessCommands(){
    return DImplementation->ProcessCommands();
}