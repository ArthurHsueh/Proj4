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
#include "XMLReader.h"
#include "DSVReader.h"
#include "OpenStreetMap.h"
#include "CSVBusSystem.h"
#include <iostream>


int main(int argc, char *argv[]){
    auto CommandDataSource  = std::make_shared<CStringDataSource>(std::cin);
    auto OutputSink = std::make_shared<CStringDataSink>(std::cout);
    auto ErrorSink = std::make_shared<CStringDataSink>(std::cerr);
    auto ResultsFactory = std::make_shared<CFileDataFactory>(std::string("results"));

    std::shared_ptr<CXMLReader> XMLReader = std::make_shared<CXMLReader>("map.osm");
    std::shared_ptr<CDSVReader> StopsReader = std::make_shared<CDSVReader>("stops.csv");
    std::shared_ptr<CDSVReader> RoutesReader = std::make_shared<CDSVReader>("routes.csv");
    std::shared_ptr<CStreetMap> StreetMap = std::make_shared<COpenStreetMap>(XMLReader);
    std::shared_ptr<CBusSystem> BusSystem = std::make_shared<CCSVBusSystem>(StopsReader, RoutesReader);
    std::shared_ptr<CTransportationPlanner::SConfiguration> Configuration = std::make_shared<CTransportationPlanner::SConfiguration>(StreetMap, BusSystem);
    auto TransportationPlanner = std::make_shared<CDijkstraTransportationPlanner>(CommandDataSource, OutputSink, ErrorSink, ResultsFactory, Configuration);

    CTransportationPlannerCommandLine CommandLine(CommandDataSource, OutputSink, ErrorSink,ResultsFactory, TransportationPlanner);
    CommandLine.ProcessCommands();
    return 0;
}
