#include <gtest/gtest.h>
#include "OpenStreetMap.h"
#include "StreetMap.h"
#include "StringDataSource.h"

#include <limits>

TEST(OSMTest, SimpleFiles){
    auto OSMDataSource = std::make_shared< CStringDataSource >("<?xml version='1.0' encoding='UTF-8'?>\n"
                                                                "<osm version=\"0.6\" generator=\"osmconvert 0.8.5\">\n"
	                                                            "  <node id=\"1\" lat=\"38.5\" lon=\"-121.7\"/>\n"
	                                                            "  <node id=\"2\" lat=\"38.5\" lon=\"-121.8\"/>\n"
                                                                "  <way id=\"100\">\n"
		                                                        "    <nd ref=\"258592863\"/>\n"
		                                                        "    <nd ref=\"4399281377\"/>\n"
                                                                "  </way>\n"
                                                                "</osm>"
                                                                );
    auto OSMReader = std::make_shared< CXMLReader >(OSMDataSource);
    COpenStreetMap OpenStreetMap(OSMReader);

    EXPECT_EQ(OpenStreetMap.NodeCount(),2);
    EXPECT_EQ(OpenStreetMap.WayCount(),1);

    EXPECT_NE(OpenStreetMap.NodeByIndex(0),nullptr);
    EXPECT_NE(OpenStreetMap.NodeByIndex(1),nullptr);
    EXPECT_EQ(OpenStreetMap.NodeByIndex(2),nullptr);
    EXPECT_NE(OpenStreetMap.WayByIndex(0),nullptr);
    EXPECT_EQ(OpenStreetMap.WayByIndex(1),nullptr);


    //Testing Nodes
    auto TempNode = OpenStreetMap.NodeByIndex(0);
    ASSERT_NE(TempNode,nullptr);
    EXPECT_EQ(TempNode, OpenStreetMap.NodeByID(TempNode->ID()));
    EXPECT_EQ(TempNode->ID(),1);
    auto TempLocation = CStreetMap::SLocation{38.5,-121.7};
    EXPECT_EQ(TempNode->Location(), TempLocation);
    EXPECT_EQ(TempNode->AttributeCount(), 0);
    EXPECT_EQ(TempNode->GetAttributeKey(0), "");
    EXPECT_EQ(TempNode->HasAttribute("highway"), false);
    EXPECT_EQ(TempNode->GetAttribute("highway"), "");
    
    TempNode = OpenStreetMap.NodeByIndex(1);
    ASSERT_NE(TempNode,nullptr);
    EXPECT_EQ(TempNode, OpenStreetMap.NodeByID(TempNode->ID()));
    EXPECT_EQ(TempNode->ID(),2);
    TempLocation = CStreetMap::SLocation{38.5,-121.8};
    EXPECT_EQ(TempNode->Location(), TempLocation);
    EXPECT_EQ(TempNode->AttributeCount(), 0);
    EXPECT_EQ(TempNode->GetAttributeKey(0), "");
    EXPECT_EQ(TempNode->HasAttribute("highway"), false);
    EXPECT_EQ(TempNode->GetAttribute("highway"), "");

    //Testing Ways
    auto TempWay = OpenStreetMap.WayByIndex(0);
    ASSERT_NE(TempWay,nullptr);
    EXPECT_EQ(TempWay, OpenStreetMap.WayByID(TempWay->ID()));
    EXPECT_EQ(TempWay->ID(),100);
    EXPECT_EQ(TempWay->NodeCount(), 2);
    EXPECT_EQ(TempWay->GetNodeID(0), 258592863);
    EXPECT_EQ(TempWay->GetNodeID(1), 4399281377);
    EXPECT_EQ(TempWay->AttributeCount(), 0);
    EXPECT_EQ(TempWay->GetAttributeKey(0), "");
    EXPECT_EQ(TempWay->HasAttribute("highway"), false);
    EXPECT_EQ(TempWay->GetAttribute("highway"), "");
}

TEST(OSMTest, MediumFile){
    auto OSMDataSource = std::make_shared< CStringDataSource> ("<?xml version='1.0' encoding='UTF-8'?>\n"
                                                                "<osm version=\"0.6\" generator=\"osmconvert 0.8.5\">\n"
                                                                "  <node id=\"62232643\" lat=\"38.52920\" lon=\"-121.75658\"/>\n"
                                                                "  <node id=\"62232651\" lat=\"38.52937\" lon=\"-121.75563\">\n"
                                                                "    <tag k=\"highway\" v=\"motorway_junction\"/>\n"
                                                                "    <tag k=\"ref\" v=\"70\"/>\n"
                                                                "  </node>\n"
                                                                "  <node id=\"62242470\" lat=\"38.52417\" lon=\"-121.78252\"/>\n"
                                                                "  <node id=\"62242472\" lat=\"38.52397\" lon=\"-121.78221\"/>\n"
                                                                "  <node id=\"62242474\" lat=\"38.52391\" lon=\"-121.78207\">\n"
                                                                "    <tag k=\"foot\" v=\"yes\"/>\n"
                                                                "    <tag k=\"barrier\" v=\"gate\"/>\n"
                                                                "    <tag k=\"bicycle\" v=\"yes\"/>\n"
                                                                "  </node>\n"
                                                                "  <way id=\"8701218\">\n"
                                                                "    <nd ref=\"62248792\"/>\n"
                                                                "    <nd ref=\"2549420729\"/>\n"
                                                                "    <tag k=\"layer\" v=\"1\"/>\n"
                                                                "    <tag k=\"bridge\" v=\"yes\"/>\n"
                                                                "    <tag k=\"highway\" v=\"residential\"/>\n"
                                                                "  </way>\n"
                                                                "  <way id=\"8706922\">\n"
                                                                "    <nd ref=\"62321892\"/>\n"
                                                                "    <nd ref=\"62225731\"/>\n"
                                                                "    <nd ref=\"591391496\"/>\n"
                                                                "    <tag k=\"highway\" v=\"residential\"/>\n"
                                                                "  </way>\n"
                                                                "</osm>"
    );

    auto OSMReader = std::make_shared< CXMLReader >(OSMDataSource);
    COpenStreetMap OpenStreetMap(OSMReader);

    EXPECT_EQ(OpenStreetMap.NodeCount(),5);
    EXPECT_EQ(OpenStreetMap.WayCount(),2);

    EXPECT_NE(OpenStreetMap.NodeByIndex(0),nullptr);
    EXPECT_NE(OpenStreetMap.NodeByIndex(1),nullptr);
    EXPECT_NE(OpenStreetMap.NodeByIndex(2),nullptr);
    EXPECT_NE(OpenStreetMap.NodeByIndex(3),nullptr);
    EXPECT_NE(OpenStreetMap.NodeByIndex(4),nullptr);
    EXPECT_EQ(OpenStreetMap.NodeByIndex(5),nullptr);
    EXPECT_NE(OpenStreetMap.WayByIndex(0),nullptr);
    EXPECT_NE(OpenStreetMap.WayByIndex(1),nullptr);
    EXPECT_EQ(OpenStreetMap.WayByIndex(2),nullptr);

    //Testing Nodes
    auto TempNode = OpenStreetMap.NodeByIndex(0);
    ASSERT_NE(TempNode,nullptr);
    EXPECT_EQ(TempNode, OpenStreetMap.NodeByID(TempNode->ID()));
    EXPECT_EQ(TempNode->ID(),62232643);
    auto TempLocation = CStreetMap::SLocation{38.52920,-121.75658};
    EXPECT_EQ(TempNode->Location(), TempLocation);
    EXPECT_EQ(TempNode->AttributeCount(), 0);
    EXPECT_EQ(TempNode->GetAttributeKey(0), "");
    EXPECT_EQ(TempNode->HasAttribute("highway"), false);
    EXPECT_EQ(TempNode->GetAttribute("highway"), "");
    
    TempNode = OpenStreetMap.NodeByIndex(1);
    ASSERT_NE(TempNode,nullptr);
    EXPECT_EQ(TempNode, OpenStreetMap.NodeByID(TempNode->ID()));
    EXPECT_EQ(TempNode->ID(),62232651);
    TempLocation = CStreetMap::SLocation{38.52937,-121.75563};
    EXPECT_EQ(TempNode->Location(), TempLocation);
    EXPECT_EQ(TempNode->AttributeCount(), 2);
    EXPECT_EQ(TempNode->GetAttributeKey(0), "highway");
    EXPECT_EQ(TempNode->GetAttributeKey(1), "ref");
    EXPECT_EQ(TempNode->HasAttribute("highway"), true);
    EXPECT_EQ(TempNode->HasAttribute("ref"), true);
    EXPECT_EQ(TempNode->GetAttribute("highway"), "motorway_junction");
    EXPECT_EQ(TempNode->GetAttribute("ref"), "70");

    TempNode = OpenStreetMap.NodeByIndex(2);
    ASSERT_NE(TempNode,nullptr);
    EXPECT_EQ(TempNode, OpenStreetMap.NodeByID(TempNode->ID()));
    EXPECT_EQ(TempNode->ID(),62242470);
    TempLocation = CStreetMap::SLocation{38.52417,-121.78252};
    EXPECT_EQ(TempNode->Location(), TempLocation);
    EXPECT_EQ(TempNode->AttributeCount(), 0);
    EXPECT_EQ(TempNode->GetAttributeKey(0), "");
    EXPECT_EQ(TempNode->HasAttribute("ref"), false);
    EXPECT_EQ(TempNode->GetAttribute("ref"), "");

    TempNode = OpenStreetMap.NodeByIndex(3);
    ASSERT_NE(TempNode,nullptr);
    EXPECT_EQ(TempNode, OpenStreetMap.NodeByID(TempNode->ID()));
    EXPECT_EQ(TempNode->ID(),62242472);
    TempLocation = CStreetMap::SLocation{38.52397,-121.78221};
    EXPECT_EQ(TempNode->Location(), TempLocation);
    EXPECT_EQ(TempNode->AttributeCount(), 0);
    EXPECT_EQ(TempNode->GetAttributeKey(0), "");
    EXPECT_EQ(TempNode->HasAttribute("highway"), false);
    EXPECT_EQ(TempNode->GetAttribute("highway"), "");

    TempNode = OpenStreetMap.NodeByIndex(4);
    ASSERT_NE(TempNode,nullptr);
    EXPECT_EQ(TempNode, OpenStreetMap.NodeByID(TempNode->ID()));
    EXPECT_EQ(TempNode->ID(),62242474);
    TempLocation = CStreetMap::SLocation{38.52391,-121.78207};
    EXPECT_EQ(TempNode->Location(), TempLocation);
    EXPECT_EQ(TempNode->AttributeCount(), 3);
    EXPECT_EQ(TempNode->GetAttributeKey(0), "foot");
    EXPECT_EQ(TempNode->GetAttributeKey(1), "barrier");
    EXPECT_EQ(TempNode->GetAttributeKey(2), "bicycle");
    EXPECT_EQ(TempNode->HasAttribute("foot"), true);
    EXPECT_EQ(TempNode->HasAttribute("barrier"), true);
    EXPECT_EQ(TempNode->HasAttribute("bicycle"), true);
    EXPECT_EQ(TempNode->GetAttribute("foot"), "yes");
    EXPECT_EQ(TempNode->GetAttribute("barrier"), "gate");
    EXPECT_EQ(TempNode->GetAttribute("bicycle"), "yes");

    //Testing Ways
    auto TempWay = OpenStreetMap.WayByIndex(0);
    ASSERT_NE(TempWay,nullptr);
    EXPECT_EQ(TempWay, OpenStreetMap.WayByID(TempWay->ID()));
    EXPECT_EQ(TempWay->ID(),8701218);
    EXPECT_EQ(TempWay->NodeCount(), 2);
    EXPECT_EQ(TempWay->GetNodeID(0), 62248792);
    EXPECT_EQ(TempWay->GetNodeID(1), 2549420729);
    EXPECT_EQ(TempWay->AttributeCount(), 3);
    EXPECT_EQ(TempWay->GetAttributeKey(0), "layer");
    EXPECT_EQ(TempWay->GetAttributeKey(1), "bridge");
    EXPECT_EQ(TempWay->GetAttributeKey(2), "highway");
    EXPECT_EQ(TempWay->HasAttribute("layer"), true);
    EXPECT_EQ(TempWay->HasAttribute("bridge"), true);
    EXPECT_EQ(TempWay->HasAttribute("highway"), true);
    EXPECT_EQ(TempWay->GetAttribute("layer"), "1");
    EXPECT_EQ(TempWay->GetAttribute("bridge"), "yes");
    EXPECT_EQ(TempWay->GetAttribute("highway"), "residential");

    TempWay = OpenStreetMap.WayByIndex(1);
    ASSERT_NE(TempWay,nullptr);
    EXPECT_EQ(TempWay, OpenStreetMap.WayByID(TempWay->ID()));
    EXPECT_EQ(TempWay->ID(),8706922);
    EXPECT_EQ(TempWay->NodeCount(), 3);
    EXPECT_EQ(TempWay->GetNodeID(0), 62321892);
    EXPECT_EQ(TempWay->GetNodeID(1), 62225731);
    EXPECT_EQ(TempWay->GetNodeID(2), 591391496);
    EXPECT_EQ(TempWay->AttributeCount(), 1);
    EXPECT_EQ(TempWay->GetAttributeKey(0), "highway");
    EXPECT_EQ(TempWay->HasAttribute("highway"), true);
    EXPECT_EQ(TempWay->GetAttribute("highway"), "residential");
}

TEST(OSMTest, LongFile){
    auto OSMDataSource = std::make_shared< CStringDataSource> ("<?xml version='1.0' encoding='UTF-8'?>\n"
                                                                "<osm version=\"0.6\" generator=\"osmconvert 0.8.5\">\n"
                                                                "  <node id=\"95710049\" lat=\"38.5553012\" lon=\"-121.7876287\">\n"
                                                                "    <tag k=\"highway\" v=\"stop\"/>\n"
                                                                "  </node>\n"
                                                                "  <node id=\"95710135\" lat=\"38.5469162\" lon=\"-121.8019329\"/>\n"
                                                                "  <node id=\"95710149\" lat=\"38.5541719\" lon=\"-121.7685632\"/>\n"
                                                                "  <node id=\"95710151\" lat=\"38.5557044\" lon=\"-121.7686195\"/>\n"
                                                                "  <node id=\"95710158\" lat=\"38.5649688\" lon=\"-121.7599721\"/>\n"
                                                                "  <node id=\"95710159\" lat=\"38.5643809\" lon=\"-121.7599814\">\n"
                                                                "    <tag k=\"highway\" v=\"turning_circle\"/>\n"
                                                                "  </node>\n"
                                                                "  <node id=\"95710171\" lat=\"38.5611334\" lon=\"-121.7861528\">\n"
                                                                "    <tag k=\"highway\" v=\"stop\"/>\n"
                                                                "  </node>\n"
                                                                "  <node id=\"95710201\" lat=\"38.5478828\" lon=\"-121.7128271\">\n"
                                                                "    <tag k=\"description\" v=\"A rather large roundabout with a field of cobblestones in the center\"/>\n"
                                                                "    <tag k=\"direction\" v=\"anticlockwise\"/>\n"
                                                                "    <tag k=\"fixme\" v=\"this roundabout lacks visual pop on the map. Looking at 5th &#38; Cantrell\"/>\n"
                                                                "    <tag k=\"highway\" v=\"mini_roundabout\"/>\n"
                                                                "    <tag k=\"junction\" v=\"roundabout\"/>\n"
                                                                "    <tag k=\"start_date\" v=\"2018-01-15\"/>\n"
                                                                "  </node>\n"
                                                                "  <way id=\"8706994\">\n"
                                                                "    <nd ref=\"95705340\"/>\n"
                                                                "    <nd ref=\"364006811\"/>\n"
                                                                "    <nd ref=\"4399281354\"/>\n"
                                                                "    <nd ref=\"4399281352\"/>\n"
                                                                "    <nd ref=\"62322905\"/>\n"
                                                                "    <nd ref=\"291405427\"/>\n"
                                                                "    <nd ref=\"4399281349\"/>\n"
                                                                "    <nd ref=\"62232630\"/>\n"
                                                                "    <nd ref=\"4399281338\"/>\n"
                                                                "    <nd ref=\"62232628\"/>\n"
                                                                "    <nd ref=\"62232626\"/>\n"
                                                                "    <nd ref=\"4399281324\"/>\n"
                                                                "    <nd ref=\"62232623\"/>\n"
                                                                "    <nd ref=\"4399281313\"/>\n"
                                                                "    <nd ref=\"62232620\"/>\n"
                                                                "    <nd ref=\"62232618\"/>\n"
                                                                "    <nd ref=\"4399281306\"/>\n"
                                                                "    <nd ref=\"62232615\"/>\n"
                                                                "    <tag k=\"bicycle\" v=\"no\"/>\n"
                                                                "    <tag k=\"destination\" v=\"San Francisco\"/>\n"
                                                                "    <tag k=\"destination:ref\" v=\"I 80 West\"/>\n"
                                                                "    <tag k=\"highway\" v=\"motorway_link\"/>\n"
                                                                "    <tag k=\"lanes\" v=\"1\"/>\n"
                                                                "    <tag k=\"oneway\" v=\"yes\"/>\n"
                                                                "  </way>\n"
                                                                "  <way id=\"10744397\">\n"
                                                                "    <nd ref=\"95704046\"/>\n"
                                                                "    <nd ref=\"95704047\"/>\n"
                                                                "    <tag k=\"name\" v=\"Santa Cruz Way\"/>\n"
                                                                "    <tag k=\"name_1\" v=\"Santa Cruz\"/>\n"
                                                                "    <tag k=\"highway\" v=\"residential\"/>\n"
                                                                "  </way>\n"
                                                                "  <way id=\"10745114\">\n"
                                                                "    <nd ref=\"95707015\"/>\n"
                                                                "    <nd ref=\"3937107143\"/>\n"
                                                                "    <nd ref=\"1893016451\"/>\n"
                                                                "    <nd ref=\"1893016455\"/>\n"
                                                                "    <nd ref=\"95708422\"/>\n"
                                                                "    <nd ref=\"1893016459\"/>\n"
                                                                "    <nd ref=\"1893016461\"/>\n"
                                                                "    <nd ref=\"256042113\"/>\n"
                                                                "    <nd ref=\"1893016466\"/>\n"
                                                                "    <nd ref=\"1893016467\"/>\n"
                                                                "    <nd ref=\"1893016468\"/>\n"
                                                                "    <nd ref=\"95708423\"/>\n"
                                                                "    <tag k=\"bicycle\" v=\"no\"/>\n"
                                                                "    <tag k=\"destination\" v=\"Woodland\"/>\n"
                                                                "    <tag k=\"destination:ref\" v=\"CA 113 North\"/>\n"
                                                                "    <tag k=\"highway\" v=\"motorway_link\"/>\n"
                                                                "    <tag k=\"oneway\" v=\"yes\"/>\n"
                                                                "  </way>\n"
                                                                "</osm>"
    );

    auto OSMReader = std::make_shared< CXMLReader >(OSMDataSource);
    COpenStreetMap OpenStreetMap(OSMReader);

    EXPECT_EQ(OpenStreetMap.NodeCount(),8);
    EXPECT_EQ(OpenStreetMap.WayCount(),3);


    auto TempNode = OpenStreetMap.NodeByIndex(0);
    ASSERT_NE(TempNode,nullptr);
    EXPECT_EQ(TempNode, OpenStreetMap.NodeByID(TempNode->ID()));
    EXPECT_EQ(TempNode->ID(),95710049);
    auto TempLocation = CStreetMap::SLocation{38.5553012,-121.7876287};
    EXPECT_EQ(TempNode->Location(), TempLocation);
    EXPECT_EQ(TempNode->AttributeCount(), 1);
    EXPECT_EQ(TempNode->GetAttributeKey(0), "highway");
    EXPECT_EQ(TempNode->HasAttribute("highway"), true);
    EXPECT_EQ(TempNode->GetAttribute("highway"), "stop");
    
    TempNode = OpenStreetMap.NodeByIndex(1);
    ASSERT_NE(TempNode,nullptr);
    EXPECT_EQ(TempNode, OpenStreetMap.NodeByID(TempNode->ID()));
    EXPECT_EQ(TempNode->ID(),95710135);
    TempLocation = CStreetMap::SLocation{38.5469162,-121.8019329};
    EXPECT_EQ(TempNode->Location(), TempLocation);
    EXPECT_EQ(TempNode->AttributeCount(), 0);
    EXPECT_EQ(TempNode->GetAttributeKey(0), "");
    EXPECT_EQ(TempNode->HasAttribute("highway"), false);
    EXPECT_EQ(TempNode->GetAttribute("highway"), "");

    TempNode = OpenStreetMap.NodeByIndex(2);
    ASSERT_NE(TempNode,nullptr);
    EXPECT_EQ(TempNode, OpenStreetMap.NodeByID(TempNode->ID()));
    EXPECT_EQ(TempNode->ID(),95710149);
    TempLocation = CStreetMap::SLocation{38.5541719,-121.7685632};
    EXPECT_EQ(TempNode->Location(), TempLocation);
    EXPECT_EQ(TempNode->AttributeCount(), 0);
    EXPECT_EQ(TempNode->GetAttributeKey(0), "");
    EXPECT_EQ(TempNode->HasAttribute("highway"), false);
    EXPECT_EQ(TempNode->GetAttribute("highway"), "");

    TempNode = OpenStreetMap.NodeByIndex(3);
    ASSERT_NE(TempNode,nullptr);
    EXPECT_EQ(TempNode, OpenStreetMap.NodeByID(TempNode->ID()));
    EXPECT_EQ(TempNode->ID(),95710151);
    TempLocation = CStreetMap::SLocation{38.5557044,-121.7686195};
    EXPECT_EQ(TempNode->Location(),TempLocation);
    EXPECT_EQ(TempNode->AttributeCount(), 0);
    EXPECT_EQ(TempNode->GetAttributeKey(0), "");
    EXPECT_EQ(TempNode->HasAttribute("highway"), false);
    EXPECT_EQ(TempNode->GetAttribute("highway"), "");

    TempNode = OpenStreetMap.NodeByIndex(4);
    ASSERT_NE(TempNode,nullptr);
    EXPECT_EQ(TempNode, OpenStreetMap.NodeByID(TempNode->ID()));
    EXPECT_EQ(TempNode->ID(),95710158);
    TempLocation = CStreetMap::SLocation{38.5649688, -121.7599721};
    EXPECT_EQ(TempNode->Location(), TempLocation);
    EXPECT_EQ(TempNode->AttributeCount(), 0);
    EXPECT_EQ(TempNode->GetAttributeKey(0), "");
    EXPECT_EQ(TempNode->HasAttribute("highway"), false);
    EXPECT_EQ(TempNode->GetAttribute("highway"), "");

    TempNode = OpenStreetMap.NodeByIndex(5);
    ASSERT_NE(TempNode,nullptr);
    EXPECT_EQ(TempNode, OpenStreetMap.NodeByID(TempNode->ID()));
    EXPECT_EQ(TempNode->ID(),95710159);
    TempLocation = CStreetMap::SLocation{38.5643809,-121.7599814};
    EXPECT_EQ(TempNode->Location(), TempLocation);
    EXPECT_EQ(TempNode->AttributeCount(), 1);
    EXPECT_EQ(TempNode->GetAttributeKey(0), "highway");
    EXPECT_EQ(TempNode->HasAttribute("highway"), true);
    EXPECT_EQ(TempNode->GetAttribute("highway"), "turning_circle");

    TempNode = OpenStreetMap.NodeByIndex(6);
    ASSERT_NE(TempNode,nullptr);
    EXPECT_EQ(TempNode, OpenStreetMap.NodeByID(TempNode->ID()));
    EXPECT_EQ(TempNode->ID(),95710171);
    TempLocation = CStreetMap::SLocation{38.5611334,-121.7861528};
    EXPECT_EQ(TempNode->Location(), TempLocation);
    EXPECT_EQ(TempNode->AttributeCount(), 1);
    EXPECT_EQ(TempNode->GetAttributeKey(0), "highway");
    EXPECT_EQ(TempNode->HasAttribute("highway"), true);
    EXPECT_EQ(TempNode->GetAttribute("highway"), "stop");

    TempNode = OpenStreetMap.NodeByIndex(7);
    ASSERT_NE(TempNode,nullptr);
    EXPECT_EQ(TempNode, OpenStreetMap.NodeByID(TempNode->ID()));
    EXPECT_EQ(TempNode->ID(),95710201);
    TempLocation = CStreetMap::SLocation{38.5478828,-121.7128271};
    EXPECT_EQ(TempNode->Location(), TempLocation);
    EXPECT_EQ(TempNode->AttributeCount(), 6);
    EXPECT_EQ(TempNode->GetAttributeKey(0), "description");
    EXPECT_EQ(TempNode->GetAttributeKey(1), "direction");
    EXPECT_EQ(TempNode->GetAttributeKey(2), "fixme");
    EXPECT_EQ(TempNode->GetAttributeKey(3), "highway");
    EXPECT_EQ(TempNode->GetAttributeKey(4), "junction");
    EXPECT_EQ(TempNode->GetAttributeKey(5), "start_date");
    EXPECT_EQ(TempNode->HasAttribute("description"), true);
    EXPECT_EQ(TempNode->HasAttribute("direction"), true);
    EXPECT_EQ(TempNode->HasAttribute("fixme"), true);
    EXPECT_EQ(TempNode->HasAttribute("highway"), true);
    EXPECT_EQ(TempNode->HasAttribute("junction"), true);
    EXPECT_EQ(TempNode->HasAttribute("start_date"), true);
    EXPECT_EQ(TempNode->GetAttribute("description"), "A rather large roundabout with a field of cobblestones in the center");
    EXPECT_EQ(TempNode->GetAttribute("direction"), "anticlockwise");
    EXPECT_EQ(TempNode->GetAttribute("fixme"), "this roundabout lacks visual pop on the map. Looking at 5th & Cantrell");
    EXPECT_EQ(TempNode->GetAttribute("highway"), "mini_roundabout");
    EXPECT_EQ(TempNode->GetAttribute("junction"), "roundabout");
    EXPECT_EQ(TempNode->GetAttribute("start_date"), "2018-01-15");

    EXPECT_EQ(OpenStreetMap.NodeByID(789), nullptr);

    //Testing Ways
    auto TempWay = OpenStreetMap.WayByIndex(0);
    ASSERT_NE(TempWay,nullptr);
    EXPECT_EQ(TempWay, OpenStreetMap.WayByID(TempWay->ID()));
    EXPECT_EQ(TempWay->ID(),8706994);
    EXPECT_EQ(TempWay->NodeCount(), 18);
    EXPECT_EQ(TempWay->GetNodeID(0), 95705340);
    EXPECT_EQ(TempWay->GetNodeID(1), 364006811);
    EXPECT_EQ(TempWay->GetNodeID(2), 4399281354);
    EXPECT_EQ(TempWay->GetNodeID(3), 4399281352);
    EXPECT_EQ(TempWay->GetNodeID(4), 62322905);
    EXPECT_EQ(TempWay->GetNodeID(5), 291405427);
    EXPECT_EQ(TempWay->GetNodeID(6), 4399281349);
    EXPECT_EQ(TempWay->GetNodeID(7), 62232630);
    EXPECT_EQ(TempWay->GetNodeID(8), 4399281338);
    EXPECT_EQ(TempWay->GetNodeID(9), 62232628);
    EXPECT_EQ(TempWay->GetNodeID(10), 62232626);
    EXPECT_EQ(TempWay->GetNodeID(11), 4399281324);
    EXPECT_EQ(TempWay->GetNodeID(12), 62232623);
    EXPECT_EQ(TempWay->GetNodeID(13), 4399281313);
    EXPECT_EQ(TempWay->GetNodeID(14), 62232620);
    EXPECT_EQ(TempWay->GetNodeID(15), 62232618);
    EXPECT_EQ(TempWay->GetNodeID(16), 4399281306);
    EXPECT_EQ(TempWay->GetNodeID(17), 62232615);
    EXPECT_EQ(TempWay->AttributeCount(), 6);
    EXPECT_EQ(TempWay->GetAttributeKey(0), "bicycle");
    EXPECT_EQ(TempWay->GetAttributeKey(1), "destination");
    EXPECT_EQ(TempWay->GetAttributeKey(2), "destination:ref");
    EXPECT_EQ(TempWay->GetAttributeKey(3), "highway");
    EXPECT_EQ(TempWay->GetAttributeKey(4), "lanes");
    EXPECT_EQ(TempWay->GetAttributeKey(5), "oneway");
    EXPECT_EQ(TempWay->HasAttribute("bicycle"), true);
    EXPECT_EQ(TempWay->HasAttribute("destination"), true);
    EXPECT_EQ(TempWay->HasAttribute("destination:ref"), true);
    EXPECT_EQ(TempWay->HasAttribute("highway"), true);
    EXPECT_EQ(TempWay->HasAttribute("lanes"), true);
    EXPECT_EQ(TempWay->HasAttribute("oneway"), true);
    EXPECT_EQ(TempWay->GetAttribute("bicycle"), "no");
    EXPECT_EQ(TempWay->GetAttribute("destination"), "San Francisco");
    EXPECT_EQ(TempWay->GetAttribute("destination:ref"), "I 80 West");
    EXPECT_EQ(TempWay->GetAttribute("highway"), "motorway_link");
    EXPECT_EQ(TempWay->GetAttribute("lanes"), "1");
    EXPECT_EQ(TempWay->GetAttribute("oneway"), "yes");

    TempWay = OpenStreetMap.WayByIndex(1);
    ASSERT_NE(TempWay,nullptr);
    EXPECT_EQ(TempWay, OpenStreetMap.WayByID(TempWay->ID()));
    EXPECT_EQ(TempWay->ID(),10744397);
    EXPECT_EQ(TempWay->NodeCount(), 2);
    EXPECT_EQ(TempWay->GetNodeID(0), 95704046);
    EXPECT_EQ(TempWay->GetNodeID(1), 95704047);
    EXPECT_EQ(TempWay->AttributeCount(),3);
    EXPECT_EQ(TempWay->GetAttributeKey(0), "name");
    EXPECT_EQ(TempWay->GetAttributeKey(1), "name_1");
    EXPECT_EQ(TempWay->GetAttributeKey(2), "highway");
    EXPECT_EQ(TempWay->HasAttribute("name"), true);
    EXPECT_EQ(TempWay->HasAttribute("name_1"), true);
    EXPECT_EQ(TempWay->HasAttribute("highway"), true);
    EXPECT_EQ(TempWay->GetAttribute("name"), "Santa Cruz Way");
    EXPECT_EQ(TempWay->GetAttribute("name_1"), "Santa Cruz");
    EXPECT_EQ(TempWay->GetAttribute("highway"), "residential");

    TempWay = OpenStreetMap.WayByIndex(2);
    ASSERT_NE(TempWay,nullptr);
    EXPECT_EQ(TempWay, OpenStreetMap.WayByID(TempWay->ID()));
    EXPECT_EQ(TempWay->ID(),10745114);
    EXPECT_EQ(TempWay->NodeCount(), 12);
    EXPECT_EQ(TempWay->GetNodeID(0), 95707015);
    EXPECT_EQ(TempWay->GetNodeID(1), 3937107143);
    EXPECT_EQ(TempWay->GetNodeID(2), 1893016451);
    EXPECT_EQ(TempWay->GetNodeID(3), 1893016455);
    EXPECT_EQ(TempWay->GetNodeID(4), 95708422);
    EXPECT_EQ(TempWay->GetNodeID(5), 1893016459);
    EXPECT_EQ(TempWay->GetNodeID(6), 1893016461);
    EXPECT_EQ(TempWay->GetNodeID(7), 256042113);
    EXPECT_EQ(TempWay->GetNodeID(8), 1893016466);
    EXPECT_EQ(TempWay->GetNodeID(9), 1893016467);
    EXPECT_EQ(TempWay->GetNodeID(10), 1893016468);
    EXPECT_EQ(TempWay->GetNodeID(11), 95708423);
    EXPECT_EQ(TempWay->AttributeCount(),5);
    EXPECT_EQ(TempWay->GetAttributeKey(0), "bicycle");
    EXPECT_EQ(TempWay->GetAttributeKey(1), "destination");
    EXPECT_EQ(TempWay->GetAttributeKey(2), "destination:ref");
    EXPECT_EQ(TempWay->GetAttributeKey(3), "highway");
    EXPECT_EQ(TempWay->GetAttributeKey(4), "oneway");
    EXPECT_EQ(TempWay->HasAttribute("bicycle"), true);
    EXPECT_EQ(TempWay->HasAttribute("destination"), true);
    EXPECT_EQ(TempWay->HasAttribute("destination:ref"), true);
    EXPECT_EQ(TempWay->HasAttribute("highway"), true);
    EXPECT_EQ(TempWay->HasAttribute("oneway"), true);
    EXPECT_EQ(TempWay->GetAttribute("bicycle"), "no");
    EXPECT_EQ(TempWay->GetAttribute("destination"), "Woodland");
    EXPECT_EQ(TempWay->GetAttribute("destination:ref"), "CA 113 North");
    EXPECT_EQ(TempWay->GetAttribute("highway"), "motorway_link");
    EXPECT_EQ(TempWay->GetAttribute("oneway"), "yes");

    EXPECT_EQ(OpenStreetMap.WayByID(789), nullptr);
    EXPECT_EQ(TempWay->GetNodeID(789), CStreetMap::InvalidNodeID);

}

TEST(OSMTest, MissingWayFile){
    auto OSMDataSource = std::make_shared< CStringDataSource >("<?xml version='1.0' encoding='UTF-8'?>\n"
                                                                    "<osm version=\"0.6\" generator=\"osmconvert 0.8.5\">\n"
                                                                    "  <node id=\"1\" lat=\"38.5\" lon=\"-121.7\"/>\n"
                                                                    "  <node id=\"2\" lat=\"38.5\" lon=\"-121.8\"/>\n"
                                                                    "</osm>"
                                                                    );

    auto OSMReader = std::make_shared< CXMLReader >(OSMDataSource);
    COpenStreetMap OpenStreetMap(OSMReader);

    EXPECT_EQ(OpenStreetMap.NodeCount(),2);
    EXPECT_EQ(OpenStreetMap.WayCount(),0);

}

TEST(OSMTest, MissingNodeID){
    auto OSMDataSource = std::make_shared< CStringDataSource >("<?xml version='1.0' encoding='UTF-8'?>\n"
                                                                    "<osm version=\"0.6\" generator=\"osmconvert 0.8.5\">\n"
                                                                    "  <node id=\"2\" lat=\"38.5\" lon=\"-121.8\"/>\n"
                                                                    "  <node>\n"
                                                                    "  <way id=\"100\">\n"
                                                                    "    <nd ref=\"258592863\"/>\n"
                                                                    "    <nd ref=\"4399281377\"/>\n"
                                                                    "  </way>\n"
                                                                    "</osm>"
                                                                    );

    auto OSMReader = std::make_shared< CXMLReader >(OSMDataSource);
    COpenStreetMap OpenStreetMap(OSMReader);

    EXPECT_EQ(OpenStreetMap.NodeCount(),1); //The second node is invalid
    EXPECT_EQ(OpenStreetMap.WayCount(),0); //Stop parsing if we see an error

    auto TempWay = OpenStreetMap.WayByIndex(0);
    EXPECT_EQ(TempWay, nullptr);
}


TEST(OSMTest, MissingWayID){
    auto OSMDataSource = std::make_shared< CStringDataSource >("<?xml version='1.0' encoding='UTF-8'?>\n"
                                                                    "<osm version=\"0.6\" generator=\"osmconvert 0.8.5\">\n"
                                                                    "  <node id=\"1\" lat=\"38.5\" lon=\"-121.7\"/>\n"
                                                                    "  <node id=\"2\" lat=\"38.5\" lon=\"-121.8\"/>\n"
                                                                    "  <way>\n"
                                                                    "    <nd ref=\"258592863\"/>\n"
                                                                    "    <nd ref=\"4399281377\"/>\n"
                                                                    "  </way>\n"
                                                                    "</osm>"
                                                                    );

    auto OSMReader = std::make_shared< CXMLReader >(OSMDataSource);
    COpenStreetMap OpenStreetMap(OSMReader);

    EXPECT_EQ(OpenStreetMap.NodeCount(),2);
    EXPECT_EQ(OpenStreetMap.WayCount(),0);

    auto TempWay = OpenStreetMap.WayByIndex(0);
    EXPECT_EQ(TempWay, nullptr);
}


TEST(OSMTest, LessThanTwoNodes){
    auto OSMDataSource = std::make_shared< CStringDataSource >("<?xml version='1.0' encoding='UTF-8'?>\n"
                                                                    "<osm version=\"0.6\" generator=\"osmconvert 0.8.5\">\n"
                                                                    "  <node id=\"1\" lat=\"38.5\" lon=\"-121.7\"/>\n"
                                                                    "  <node id=\"2\" lat=\"38.5\" lon=\"-121.8\"/>\n"
                                                                    "  <way id=\"100\">\n"
                                                                    "    <nd ref=\"4399281377\"/>\n"
                                                                    "  </way>\n"
                                                                    "</osm>"
                                                                    );

    auto OSMReader = std::make_shared< CXMLReader >(OSMDataSource);
    COpenStreetMap OpenStreetMap(OSMReader);

    EXPECT_EQ(OpenStreetMap.NodeCount(),2);
    EXPECT_EQ(OpenStreetMap.WayCount(),0);

    auto TempWay = OpenStreetMap.WayByIndex(0);
    EXPECT_EQ(TempWay, nullptr);
}

TEST(OSMTest, EmptyFile){
    auto OSMDataSource = std::make_shared< CStringDataSource >("<?xml version='1.0' encoding='UTF-8'?>\n"
                                                                    "</osm>" 
                                                              );

    auto OSMReader = std::make_shared< CXMLReader >(OSMDataSource);
    COpenStreetMap OpenStreetMap(OSMReader);

    EXPECT_EQ(OpenStreetMap.NodeCount(),0);
    EXPECT_EQ(OpenStreetMap.WayCount(),0);
}
