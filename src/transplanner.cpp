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
#include <iostream>


int main(int argc, char *argv[]){
    auto CommandDataSource  = std::make_shared<CStringDataSource>(std::cin);
    auto OutputSink = std::make_shared<CStringDataSink>(std::cout);
    auto ErrorSink = std::make_shared<CStringDataSink>(std::cerr);
    auto ResultsFactory = std::make_shared<CFileDataFactory>(std::string("results"));

    auto StreetMap = std::make_shared<CStreetMap>();
    auto BusSystem = std::make_shared<CBusSystem>();

    auto TransportationPlanner = std::make_shared<CDijkstraTransportationPlanner>();

    CTransportationPlannerCommandLine CommandLine(CommandDataSource, OutputSink, ErrorSink,ResultsFactory, TransportationPlanner);
    CommandLine.ProcessCommands();
    return 0;
}
