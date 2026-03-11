#include "TransportationPlannerCommandLine.h"
#include "DataFactory.h"
#include "DijkstraTransportationPlanner.h"
#include "FileDataSource.h"
#include "FileDataSink.h"
#include "FileDataFactory.h"
#include "StandardDataSink.h"
#include "StandardDataSource.h"
#include "StandardErrorDataSink.h"
#include "StringDataSink.h"
#include "StringDataSource.h"

int main(int argc, char *argv[]){


    std::shared_ptr<CDataSource> CommandDataSource = std::make_shared<CStringDataSource>();
    std::shared_ptr<CDataSink> OutputSink = std::make_shared<CStringDataSink>();
    std::shared_ptr<CDataSink> ErrorSink = std::make_shared<CStringDataSink>();
    std::shared_ptr<CDataFactory> ResultsFactory = std::make_shared<CFileDataFactory>();
    std::shared_ptr<CTransportationPlanner> TransportationPlanner = std::make_shared<CDijkstraTransportationPlanner>();

    CTransportationPlannerCommandLine CommandLine(CommandDataSource, OutputSink, ErrorSink,ResultsFactory, TransportationPlanner);
    CommandLine.ProcessCommands();

    return 0;
}
//ProcessCommand()

//CTransportationPlannerCommandLine(std::shared_ptr<CDataSource> cmdsrc, std::shared_ptr<CDataSink> outsink, std::shared_ptr<CDataSink> errsink, std::shared_ptr<CDataFactory> results, std::shared_ptr<CTransportationPlanner> planner);