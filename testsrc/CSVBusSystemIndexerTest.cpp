#include <gtest/gtest.h>
#include "XMLReader.h"
#include "StringUtils.h"
#include "StringDataSource.h"
#include "DSVReader.h"
#include "CSVBusSystem.h"
#include "BusSystemIndexer.h"

#include <iostream>
using std::cout;
using std::endl;

TEST(CSVBusSystemIndexer, SimpleTest){
cout<<"IN "<<__FILE__<<" @ "<<__LINE__<<endl;
    auto InStreamStops = std::make_shared<CStringDataSource>("stop_id,node_id");
    auto InStreamRoutes = std::make_shared<CStringDataSource>("route,stop_id");
    auto CSVReaderStops = std::make_shared<CDSVReader>(InStreamStops,',');
cout<<"IN "<<__FILE__<<" @ "<<__LINE__<<endl;
    auto CSVReaderRoutes = std::make_shared<CDSVReader>(InStreamRoutes,',');
cout<<"IN "<<__FILE__<<" @ "<<__LINE__<<endl;
    auto BusSystem = std::make_shared<CCSVBusSystem>(CSVReaderStops, CSVReaderRoutes);
cout<<"IN "<<__FILE__<<" @ "<<__LINE__<<endl;
    CBusSystemIndexer BusSystemIndexer(BusSystem);
cout<<"IN "<<__FILE__<<" @ "<<__LINE__<<endl;
    EXPECT_EQ(BusSystemIndexer.StopCount(),0);
    EXPECT_EQ(BusSystemIndexer.RouteCount(),0);
}

TEST(CSVBusSystemIndexer, StopTest){
    auto InStreamStops = std::make_shared<CStringDataSource>(   "stop_id,node_id\n"
                                                                "2,102\n"
                                                                "1,101");
    auto InStreamRoutes = std::make_shared<CStringDataSource>("route,stop_id");
    auto CSVReaderStops = std::make_shared<CDSVReader>(InStreamStops,',');
    auto CSVReaderRoutes = std::make_shared<CDSVReader>(InStreamRoutes,',');
    auto BusSystem = std::make_shared<CCSVBusSystem>(CSVReaderStops, CSVReaderRoutes);
    CBusSystemIndexer BusSystemIndexer(BusSystem);

    EXPECT_EQ(BusSystemIndexer.StopCount(),2);
    EXPECT_EQ(BusSystemIndexer.RouteCount(),0);
    auto Stop1Index = BusSystemIndexer.SortedStopByIndex(0);
    auto Stop1Node = BusSystemIndexer.StopByNodeID(101);
    EXPECT_EQ(Stop1Index,Stop1Node);
    ASSERT_TRUE(bool(Stop1Index));
    EXPECT_EQ(Stop1Index->ID(),1);
    EXPECT_EQ(Stop1Index->NodeID(),101);
    auto Stop2Index = BusSystemIndexer.SortedStopByIndex(1);
    auto Stop2Node = BusSystemIndexer.StopByNodeID(102);
    EXPECT_EQ(Stop2Index,Stop2Node);
    ASSERT_TRUE(bool(Stop2Index));
    EXPECT_EQ(Stop2Index->ID(),2);
    EXPECT_EQ(Stop2Index->NodeID(),102);
}

TEST(CSVBusSystemIndexer, RouteTest){
    auto InStreamStops = std::make_shared<CStringDataSource>(   "stop_id,node_id\n"
                                                                "1,101\n"
                                                                "2,102");
    auto InStreamRoutes = std::make_shared<CStringDataSource>(  "route,stop_id\n"
                                                                "B,1\n"
                                                                "B,2\n"
                                                                "B,1\n"
                                                                "A,2\n"
                                                                "A,1\n"
                                                                "A,2");
    auto CSVReaderStops = std::make_shared<CDSVReader>(InStreamStops,',');
    auto CSVReaderRoutes = std::make_shared<CDSVReader>(InStreamRoutes,',');
    auto BusSystem = std::make_shared<CCSVBusSystem>(CSVReaderStops, CSVReaderRoutes);
    CBusSystemIndexer BusSystemIndexer(BusSystem);

    EXPECT_EQ(BusSystemIndexer.StopCount(),2);
    EXPECT_EQ(BusSystemIndexer.RouteCount(),2);
    auto Route1Index = BusSystemIndexer.SortedRouteByIndex(0);
    ASSERT_TRUE(bool(Route1Index));
    EXPECT_EQ(Route1Index->Name(),"A");
    EXPECT_EQ(Route1Index->StopCount(),3);
    EXPECT_EQ(Route1Index->GetStopID(0),2);
    EXPECT_EQ(Route1Index->GetStopID(1),1);
    EXPECT_EQ(Route1Index->GetStopID(2),2);
    auto Route2Index = BusSystemIndexer.SortedRouteByIndex(1);
    ASSERT_TRUE(bool(Route2Index));
    EXPECT_EQ(Route2Index->Name(),"B");
    EXPECT_EQ(Route2Index->StopCount(),3);
    EXPECT_EQ(Route2Index->GetStopID(0),1);
    EXPECT_EQ(Route2Index->GetStopID(1),2);
    EXPECT_EQ(Route2Index->GetStopID(2),1);
    std::unordered_set< std::shared_ptr<CBusSystem::SRoute> > Routes;
    EXPECT_TRUE(BusSystemIndexer.RoutesByNodeIDs(101,102,Routes));
    EXPECT_EQ(Routes.size(),2);
    EXPECT_TRUE(Routes.find(Route1Index) != Routes.end());
    EXPECT_TRUE(Routes.find(Route2Index) != Routes.end());

}

TEST(CSVBusSystemIndexer, StopCountTest){
    auto InStreamStops = std::make_shared<CStringDataSource>("stop_id,node_id\n"
                                                            "1,101\n"
                                                            "2,102");
    auto InStreamRoutes = std::make_shared<CStringDataSource>("route,stop_id");
    auto CSVReaderStops = std::make_shared<CDSVReader>(InStreamStops,',');
    auto CSVReaderRoutes = std::make_shared<CDSVReader>(InStreamRoutes,',');
    auto BusSystem = std::make_shared<CCSVBusSystem>(CSVReaderStops, CSVReaderRoutes);
    CBusSystemIndexer BusSystemIndexer(BusSystem);
    EXPECT_EQ(BusSystemIndexer.StopCount(),2);
}

TEST(CSVBusSystemIndexer, RouteCountTest){
    auto InStreamStops = std::make_shared<CStringDataSource>("stop_id,node_id\n"
                                                            "1,101\n"
                                                            "2,102");
    auto InStreamRoutes = std::make_shared<CStringDataSource>("route,stop_id\n"
                                                            "A,1\n"
                                                            "A,2");
    auto CSVReaderStops = std::make_shared<CDSVReader>(InStreamStops,',');
    auto CSVReaderRoutes = std::make_shared<CDSVReader>(InStreamRoutes,',');
    auto BusSystem = std::make_shared<CCSVBusSystem>(CSVReaderStops, CSVReaderRoutes);
    CBusSystemIndexer BusSystemIndexer(BusSystem);
    EXPECT_EQ(BusSystemIndexer.RouteCount(),1); 
}

TEST(CSVBusSystemIndexer, SortedStopByIndexTest){
    auto InStreamStops = std::make_shared<CStringDataSource>("stop_id,node_id\n"
                                                            "2,102\n"
                                                            "1,101");
    auto InStreamRoutes = std::make_shared<CStringDataSource>("route,stop_id");
    auto CSVReaderStops = std::make_shared<CDSVReader>(InStreamStops,',');
    auto CSVReaderRoutes = std::make_shared<CDSVReader>(InStreamRoutes,',');
    auto BusSystem = std::make_shared<CCSVBusSystem>(CSVReaderStops, CSVReaderRoutes);
    CBusSystemIndexer BusSystemIndexer(BusSystem);
    auto Stop = BusSystemIndexer.SortedStopByIndex(0);
    ASSERT_TRUE(bool(Stop));
    EXPECT_EQ(Stop->ID(),1);
}

TEST(CSVBusSystemIndexer, SortedRouteByIndexTest){
    auto InStreamStops = std::make_shared<CStringDataSource>("stop_id,node_id\n"
                                                            "1,101\n"
                                                            "2,102");
    auto InStreamRoutes = std::make_shared<CStringDataSource>("route,stop_id\n"
                                                            "B,1\n"
                                                            "B,2\n"
                                                            "A,1\n"
                                                            "A,2");
    auto CSVReaderStops = std::make_shared<CDSVReader>(InStreamStops,',');
    auto CSVReaderRoutes = std::make_shared<CDSVReader>(InStreamRoutes,',');
    auto BusSystem = std::make_shared<CCSVBusSystem>(CSVReaderStops, CSVReaderRoutes);
    CBusSystemIndexer BusSystemIndexer(BusSystem);
    auto Route = BusSystemIndexer.SortedRouteByIndex(0);
    ASSERT_TRUE(bool(Route));
    EXPECT_EQ(Route->Name(), "A"); 
}

TEST(CSVBusSystemIndexer, StopByNodeIDTest){
    auto InStreamStops = std::make_shared<CStringDataSource>("stop_id,node_id\n"
                                                            "1,101\n"
                                                            "2,102");  
    auto InStreamRoutes = std::make_shared<CStringDataSource>("route,stop_id");
    auto CSVReaderStops = std::make_shared<CDSVReader>(InStreamStops,',');
    auto CSVReaderRoutes = std::make_shared<CDSVReader>(InStreamRoutes,',');
    auto BusSystem = std::make_shared<CCSVBusSystem>(CSVReaderStops, CSVReaderRoutes);
    CBusSystemIndexer BusSystemIndexer(BusSystem);
    auto Stop = BusSystemIndexer.StopByNodeID(101);
    ASSERT_TRUE(bool(Stop));
    EXPECT_EQ(Stop->ID(), 1);
}

TEST(CSVBusSystemIndexer, RoutesByNodeIDsTest){
    auto InStreamStops = std::make_shared<CStringDataSource>("stop_id,node_id\n"
                                                            "1,101\n"
                                                            "2,102");
    auto InStreamRoutes = std::make_shared<CStringDataSource>("route,stop_id\n"
                                                            "A,1\n"
                                                            "A,2");
    auto CSVReaderStops = std::make_shared<CDSVReader>(InStreamStops,',');
    auto CSVReaderRoutes = std::make_shared<CDSVReader>(InStreamRoutes,',');
    auto BusSystem = std::make_shared<CCSVBusSystem>(CSVReaderStops, CSVReaderRoutes);
    CBusSystemIndexer BusSystemIndexer(BusSystem);
    std::unordered_set<std::shared_ptr<CBusSystem::SRoute>> Routes;
    EXPECT_TRUE(BusSystemIndexer.RoutesByNodeIDs(101, 102, Routes));
    EXPECT_EQ(Routes.size(), 1);
}

TEST(CSVBusSystemIndexer, RouteBetweenNodeIDsTest){
    auto InStreamStops = std::make_shared<CStringDataSource>("stop_id,node_id\n"
                                                            "1,101\n"
                                                            "2,102");
    auto InStreamRoutes = std::make_shared<CStringDataSource>("route,stop_id\n"
                                                            "A,1\n"
                                                            "A,2");
    auto CSVReaderStops = std::make_shared<CDSVReader>(InStreamStops,',');
    auto CSVReaderRoutes = std::make_shared<CDSVReader>(InStreamRoutes,',');
    auto BusSystem = std::make_shared<CCSVBusSystem>(CSVReaderStops, CSVReaderRoutes);
    CBusSystemIndexer BusSystemIndexer(BusSystem);
    EXPECT_TRUE(BusSystemIndexer.RouteBetweenNodeIDs(101, 102));
}


//tests the bounds of sortedstopbyindex and sortedroutebyindex, returns nullptr when index is out of bounds
TEST(CSVBusSystemIndexer, OutOfBoundsCheckTest){
    auto InStreamStops = std::make_shared<CStringDataSource>("stop_id,node_id\n1,101\n2,102");
    auto InStreamRoutes = std::make_shared<CStringDataSource>("route,stop_id\nA,1\nA,2");
    auto CSVReaderStops = std::make_shared<CDSVReader>(InStreamStops,',');
    auto CSVReaderRoutes = std::make_shared<CDSVReader>(InStreamRoutes,',');
    auto BusSystem = std::make_shared<CCSVBusSystem>(CSVReaderStops, CSVReaderRoutes);
    CBusSystemIndexer BusSystemIndexer(BusSystem);
    EXPECT_EQ(BusSystemIndexer.SortedStopByIndex(99), nullptr);
    EXPECT_EQ(BusSystemIndexer.SortedRouteByIndex(99), nullptr);
}

TEST(CSVBusSystemIndexer, StopByNodeIDInvalidTest){
    auto InStreamStops = std::make_shared<CStringDataSource>("stop_id,node_id\n"
                                                            "1,101\n"
                                                            "2,102");
    auto InStreamRoutes = std::make_shared<CStringDataSource>("route,stop_id");
    auto CSVReaderStops = std::make_shared<CDSVReader>(InStreamStops,',');
    auto CSVReaderRoutes = std::make_shared<CDSVReader>(InStreamRoutes,',');
    auto BusSystem = std::make_shared<CCSVBusSystem>(CSVReaderStops, CSVReaderRoutes);
    CBusSystemIndexer BusSystemIndexer(BusSystem);
    auto ValidStop = BusSystemIndexer.StopByNodeID(101);
    ASSERT_TRUE(bool(ValidStop));
    EXPECT_EQ(BusSystemIndexer.StopByNodeID(999), nullptr);

}

TEST(CSVBusSystemIndexer, RoutesByNodeIDsNoRouteTest){
    auto InStreamStops = std::make_shared<CStringDataSource>("stop_id,node_id\n"
                                                            "1,101\n"
                                                            "2,102");
    auto InStreamRoutes = std::make_shared<CStringDataSource>("route,stop_id\n"
                                                            "A,1\n"
                                                            "A,2");
    auto CSVReaderStops = std::make_shared<CDSVReader>(InStreamStops,',');
    auto CSVReaderRoutes = std::make_shared<CDSVReader>(InStreamRoutes,',');
    auto BusSystem = std::make_shared<CCSVBusSystem>(CSVReaderStops, CSVReaderRoutes);
    CBusSystemIndexer BusSystemIndexer(BusSystem);
    std::unordered_set<std::shared_ptr<CBusSystem::SRoute>> Routes;
    EXPECT_TRUE(BusSystemIndexer.RoutesByNodeIDs(101, 102, Routes));
    Routes.clear();
    EXPECT_FALSE(BusSystemIndexer.RoutesByNodeIDs(101, 999, Routes));
    EXPECT_EQ(Routes.size(), 0);
}

TEST(CSVBusSystemIndexer, RouteBetweenNodeIDsNoRouteTest){
    auto InStreamStops = std::make_shared<CStringDataSource>("stop_id,node_id\n"
                                                            "1,101\n"
                                                            "2,102");
    auto InStreamRoutes = std::make_shared<CStringDataSource>("route,stop_id\n"
                                                            "A,1\n"
                                                            "A,2");
    auto CSVReaderStops = std::make_shared<CDSVReader>(InStreamStops,',');
    auto CSVReaderRoutes = std::make_shared<CDSVReader>(InStreamRoutes,',');
    auto BusSystem = std::make_shared<CCSVBusSystem>(CSVReaderStops, CSVReaderRoutes);
    CBusSystemIndexer BusSystemIndexer(BusSystem);
    EXPECT_TRUE(BusSystemIndexer.RouteBetweenNodeIDs(101, 102));
    EXPECT_FALSE(BusSystemIndexer.RouteBetweenNodeIDs(101, 999));
}

TEST(CSVBusSystemIndexer, RoutesByNodeIDsDirectionTest){
    auto InStreamStops = std::make_shared<CStringDataSource>("stop_id,node_id\n"
                                                            "1,101\n"
                                                            "2,102");
    auto InStreamRoutes = std::make_shared<CStringDataSource>("route,stop_id\n"
                                                            "A,1\n"
                                                            "A,2");
    auto CSVReaderStops = std::make_shared<CDSVReader>(InStreamStops,',');
    auto CSVReaderRoutes = std::make_shared<CDSVReader>(InStreamRoutes,',');
    auto BusSystem = std::make_shared<CCSVBusSystem>(CSVReaderStops, CSVReaderRoutes);
    CBusSystemIndexer BusSystemIndexer(BusSystem);
    std::unordered_set<std::shared_ptr<CBusSystem::SRoute>> Routes;
    EXPECT_TRUE(BusSystemIndexer.RoutesByNodeIDs(101, 102, Routes));
    EXPECT_FALSE(BusSystemIndexer.RoutesByNodeIDs(102, 101, Routes));
}

TEST(CSVBusSystemIndexer, SingleStopRouteTest){
    auto InStreamStops = std::make_shared<CStringDataSource>("stop_id,node_id\n"
                                                            "1,101\n"
                                                            "2,102");
    auto InStreamRoutes = std::make_shared<CStringDataSource>("route,stop_id\n"
                                                            "A,1");
    auto CSVReaderStops = std::make_shared<CDSVReader>(InStreamStops,',');
    auto CSVReaderRoutes = std::make_shared<CDSVReader>(InStreamRoutes,',');
    auto BusSystem = std::make_shared<CCSVBusSystem>(CSVReaderStops, CSVReaderRoutes);
    CBusSystemIndexer BusSystemIndexer(BusSystem);
    std::unordered_set<std::shared_ptr<CBusSystem::SRoute>> Routes;
    EXPECT_FALSE(BusSystemIndexer.RoutesByNodeIDs(101, 102, Routes));
    EXPECT_FALSE(BusSystemIndexer.RouteBetweenNodeIDs(101, 102));
    EXPECT_EQ(BusSystemIndexer.StopCount(), 2);
}

TEST(CSVBusSystemIndexer, LongTest){
    auto InStreamStops = std::make_shared<CStringDataSource>("stop_id,node_id\n"
                                                            "3,103\n"
                                                            "1,101\n"
                                                            "4,104\n"
                                                            "2,102");
    auto InStreamRoutes = std::make_shared<CStringDataSource>("route,stop_id\n"
                                                             "C,1\n"
                                                             "C,2\n"
                                                             "C,3\n"
                                                             "B,3\n"
                                                             "B,4\n"
                                                             "B,1\n"
                                                             "A,2\n"
                                                             "A,3\n"
                                                             "A,4");
    auto CSVReaderStops = std::make_shared<CDSVReader>(InStreamStops,',');
    auto CSVReaderRoutes = std::make_shared<CDSVReader>(InStreamRoutes,',');
    auto BusSystem = std::make_shared<CCSVBusSystem>(CSVReaderStops, CSVReaderRoutes);
    CBusSystemIndexer BusSystemIndexer(BusSystem);

    EXPECT_EQ(BusSystemIndexer.StopCount(), 4);
    EXPECT_EQ(BusSystemIndexer.RouteCount(), 3);

    auto Stop0 = BusSystemIndexer.SortedStopByIndex(0);
    auto Stop1 = BusSystemIndexer.SortedStopByIndex(1);
    auto Stop2 = BusSystemIndexer.SortedStopByIndex(2);
    auto Stop3 = BusSystemIndexer.SortedStopByIndex(3);
    ASSERT_TRUE(bool(Stop0));
    ASSERT_TRUE(bool(Stop1));
    ASSERT_TRUE(bool(Stop2));
    ASSERT_TRUE(bool(Stop3));
    EXPECT_EQ(Stop0->ID(), 1);
    EXPECT_EQ(Stop1->ID(), 2);
    EXPECT_EQ(Stop2->ID(), 3);
    EXPECT_EQ(Stop3->ID(), 4);

    auto Route0 = BusSystemIndexer.SortedRouteByIndex(0);
    auto Route1 = BusSystemIndexer.SortedRouteByIndex(1);
    auto Route2 = BusSystemIndexer.SortedRouteByIndex(2);
    ASSERT_TRUE(bool(Route0));
    ASSERT_TRUE(bool(Route1));
    ASSERT_TRUE(bool(Route2));
    EXPECT_EQ(Route0->Name(), "A");
    EXPECT_EQ(Route1->Name(), "B");
    EXPECT_EQ(Route2->Name(), "C");

    auto StopNode101 = BusSystemIndexer.StopByNodeID(101);
    auto StopNode102 = BusSystemIndexer.StopByNodeID(102);
    auto StopNode103 = BusSystemIndexer.StopByNodeID(103);
    auto StopNode104 = BusSystemIndexer.StopByNodeID(104);
    ASSERT_TRUE(bool(StopNode101));
    ASSERT_TRUE(bool(StopNode102));
    ASSERT_TRUE(bool(StopNode103));
    ASSERT_TRUE(bool(StopNode104));
    EXPECT_EQ(StopNode101->ID(), 1);
    EXPECT_EQ(StopNode102->ID(), 2);
    EXPECT_EQ(StopNode103->ID(), 3);
    EXPECT_EQ(StopNode104->ID(), 4);

    EXPECT_EQ(BusSystemIndexer.StopByNodeID(999), nullptr);

    EXPECT_EQ(BusSystemIndexer.SortedStopByIndex(4), nullptr);
    EXPECT_EQ(BusSystemIndexer.SortedRouteByIndex(3), nullptr);

    std::unordered_set<std::shared_ptr<CBusSystem::SRoute>> Routes;
    EXPECT_TRUE(BusSystemIndexer.RoutesByNodeIDs(101, 102, Routes));
    EXPECT_EQ(Routes.size(), 1);
    EXPECT_TRUE(Routes.find(Route2) != Routes.end());

    Routes.clear();
    EXPECT_TRUE(BusSystemIndexer.RoutesByNodeIDs(102, 103, Routes));
    EXPECT_EQ(Routes.size(), 2); 
    EXPECT_TRUE(Routes.find(Route0) != Routes.end()); 
    EXPECT_TRUE(Routes.find(Route2) != Routes.end()); 
    Routes.clear();
    EXPECT_FALSE(BusSystemIndexer.RoutesByNodeIDs(102, 101, Routes));

    EXPECT_TRUE(BusSystemIndexer.RouteBetweenNodeIDs(101, 102));
    EXPECT_TRUE(BusSystemIndexer.RouteBetweenNodeIDs(103, 104));
    EXPECT_FALSE(BusSystemIndexer.RouteBetweenNodeIDs(101, 999));
    EXPECT_FALSE(BusSystemIndexer.RouteBetweenNodeIDs(999, 101)); 
    EXPECT_FALSE(BusSystemIndexer.RouteBetweenNodeIDs(102, 101)); 
}
