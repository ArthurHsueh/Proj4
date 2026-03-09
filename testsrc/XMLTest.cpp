#include <gtest/gtest.h>
#include "XMLReader.h"
#include "XMLWriter.h"
#include "XMLEntity.h"
#include "StringDataSource.h"
#include "StringDataSink.h"
#include "XMLEntity.h"
#include <vector>
#include <utility>

//Read Test
TEST(XMLReader, EmptyTest){
    std::string XMLString = "";
    std::shared_ptr<CStringDataSource> DataSource = std::make_shared<CStringDataSource>(XMLString);
    CXMLReader Reader(DataSource);
    SXMLEntity TempEntity;

    EXPECT_FALSE(Reader.ReadEntity(TempEntity));
}

TEST(XMLReader, CharDataTest){
    std::string XMLString = "<name>Bob</name>";
    std::shared_ptr<CStringDataSource> DataSource = std::make_shared<CStringDataSource>(XMLString);
    CXMLReader Reader(DataSource);
    SXMLEntity TempEntity;
    
    EXPECT_TRUE(Reader.ReadEntity(TempEntity));
    EXPECT_EQ(TempEntity.DType,SXMLEntity::EType::StartElement);
    EXPECT_EQ(TempEntity.DNameData, "name");
    EXPECT_TRUE(TempEntity.DAttributes.empty());

    EXPECT_TRUE(Reader.ReadEntity(TempEntity));
    EXPECT_EQ(TempEntity.DType, SXMLEntity::EType::CharData);
    EXPECT_EQ(TempEntity.DNameData, "Bob");
    EXPECT_TRUE(TempEntity.DAttributes.empty());

    EXPECT_TRUE(Reader.ReadEntity(TempEntity));
    EXPECT_EQ(TempEntity.DType, SXMLEntity::EType::EndElement);
    EXPECT_EQ(TempEntity.DNameData, "name");
    EXPECT_TRUE(TempEntity.DAttributes.empty());
}

TEST(XMLReader, AttributeTest){
    std::string XMLString = "<tag attr1=\"val1\">Some Data</tag>";
    std::shared_ptr<CStringDataSource> DataSource = std::make_shared<CStringDataSource>(XMLString);
    CXMLReader Reader(DataSource);
    SXMLEntity TempEntity;

    EXPECT_TRUE(Reader.ReadEntity(TempEntity));
    EXPECT_EQ(TempEntity.DType,SXMLEntity::EType::StartElement);
    EXPECT_EQ(TempEntity.DNameData,"tag");
    ASSERT_EQ(TempEntity.DAttributes.size(),1);
    EXPECT_EQ(TempEntity.AttributeValue("attr1"),"val1");

    EXPECT_TRUE(Reader.ReadEntity(TempEntity));
    EXPECT_EQ(TempEntity.DType,SXMLEntity::EType::CharData);
    EXPECT_EQ(TempEntity.DNameData,"Some Data");

    EXPECT_TRUE(Reader.ReadEntity(TempEntity));
    EXPECT_EQ(TempEntity.DType, SXMLEntity::EType::EndElement);
    EXPECT_EQ(TempEntity.DNameData,"tag");
    EXPECT_TRUE(TempEntity.DAttributes.empty());
}

TEST(XMLReader, HardTest){
    std::string XMLString = "<letter id=\"789\" date=\"1/31/26\"><to>Bob</to><from>Joey</from><heading font=\"Times New Roman\">Appointment</heading><body>\"Where are we meeting tomorrow?\"</body></letter>";
    std::shared_ptr<CStringDataSource> DataSource = std::make_shared<CStringDataSource>(XMLString);
    CXMLReader Reader(DataSource);
    SXMLEntity TempEntity;

    EXPECT_TRUE(Reader.ReadEntity(TempEntity));
    EXPECT_EQ(TempEntity.DType,SXMLEntity::EType::StartElement);
    EXPECT_EQ(TempEntity.DNameData, "letter");
    ASSERT_EQ(TempEntity.DAttributes.size(),2);
    EXPECT_EQ(TempEntity.AttributeValue("id"),"789");
    EXPECT_EQ(TempEntity.AttributeValue("date"),"1/31/26");

    EXPECT_TRUE(Reader.ReadEntity(TempEntity));
    EXPECT_EQ(TempEntity.DType,SXMLEntity::EType::StartElement);
    EXPECT_EQ(TempEntity.DNameData, "to");
    ASSERT_EQ(TempEntity.DAttributes.size(),0);

    EXPECT_TRUE(Reader.ReadEntity(TempEntity));
    EXPECT_EQ(TempEntity.DType,SXMLEntity::EType::CharData);
    EXPECT_EQ(TempEntity.DNameData, "Bob");

    EXPECT_TRUE(Reader.ReadEntity(TempEntity));
    EXPECT_EQ(TempEntity.DType, SXMLEntity::EType::EndElement);
    EXPECT_EQ(TempEntity.DNameData, "to");
    EXPECT_TRUE(TempEntity.DAttributes.empty());

    EXPECT_TRUE(Reader.ReadEntity(TempEntity));
    EXPECT_EQ(TempEntity.DType,SXMLEntity::EType::StartElement);
    EXPECT_EQ(TempEntity.DNameData, "from");
    ASSERT_EQ(TempEntity.DAttributes.size(),0);

    EXPECT_TRUE(Reader.ReadEntity(TempEntity));
    EXPECT_EQ(TempEntity.DType,SXMLEntity::EType::CharData);
    EXPECT_EQ(TempEntity.DNameData,"Joey");

    EXPECT_TRUE(Reader.ReadEntity(TempEntity));
    EXPECT_EQ(TempEntity.DType, SXMLEntity::EType::EndElement);
    EXPECT_EQ(TempEntity.DNameData, "from");
    EXPECT_TRUE(TempEntity.DAttributes.empty());

    EXPECT_TRUE(Reader.ReadEntity(TempEntity));
    EXPECT_EQ(TempEntity.DType,SXMLEntity::EType::StartElement);
    EXPECT_EQ(TempEntity.DNameData, "heading");
    ASSERT_EQ(TempEntity.DAttributes.size(),1);
    EXPECT_EQ(TempEntity.AttributeValue("font"),"Times New Roman");

    EXPECT_TRUE(Reader.ReadEntity(TempEntity));
    EXPECT_EQ(TempEntity.DType,SXMLEntity::EType::CharData);
    EXPECT_EQ(TempEntity.DNameData, "Appointment");

    EXPECT_TRUE(Reader.ReadEntity(TempEntity));
    EXPECT_EQ(TempEntity.DType, SXMLEntity::EType::EndElement);
    EXPECT_EQ(TempEntity.DNameData,"heading");
    EXPECT_TRUE(TempEntity.DAttributes.empty());

    EXPECT_TRUE(Reader.ReadEntity(TempEntity));
    EXPECT_EQ(TempEntity.DType,SXMLEntity::EType::StartElement);
    EXPECT_EQ(TempEntity.DNameData, "body");
    ASSERT_EQ(TempEntity.DAttributes.size(),0);

    EXPECT_TRUE(Reader.ReadEntity(TempEntity));
    EXPECT_EQ(TempEntity.DType,SXMLEntity::EType::CharData);
    EXPECT_EQ(TempEntity.DNameData, "\"Where are we meeting tomorrow?\"");

    EXPECT_TRUE(Reader.ReadEntity(TempEntity));
    EXPECT_EQ(TempEntity.DType, SXMLEntity::EType::EndElement);
    EXPECT_EQ(TempEntity.DNameData, "body");
    EXPECT_TRUE(TempEntity.DAttributes.empty());

    EXPECT_TRUE(Reader.ReadEntity(TempEntity));
    EXPECT_EQ(TempEntity.DType, SXMLEntity::EType::EndElement);
    EXPECT_EQ(TempEntity.DNameData, "letter");
    EXPECT_TRUE(TempEntity.DAttributes.empty());
}

TEST(XMLReader, CharDataTestWithSkipData){
    std::string XMLString = "<name>Bob</name>";
    std::shared_ptr<CStringDataSource> DataSource = std::make_shared<CStringDataSource>(XMLString);
    CXMLReader Reader(DataSource);
    SXMLEntity TempEntity;
    
    EXPECT_TRUE(Reader.ReadEntity(TempEntity));
    EXPECT_EQ(TempEntity.DType,SXMLEntity::EType::StartElement);
    EXPECT_EQ(TempEntity.DNameData, "name");
    EXPECT_TRUE(TempEntity.DAttributes.empty());

    EXPECT_TRUE(Reader.ReadEntity(TempEntity, true));
    EXPECT_EQ(TempEntity.DType,SXMLEntity::EType::EndElement);
    EXPECT_EQ(TempEntity.DNameData, "name");
    EXPECT_TRUE(TempEntity.DAttributes.empty());
}

TEST(XMLReader, MediumTestWithSkipDataAndOtherFunctions){
    std::string XMLString = "<name first=\"Bob\"><stats><gender>male</gender><height>5\'9\"</height><weight>180</weight></stats></name>";
    std::shared_ptr<CStringDataSource> DataSource = std::make_shared<CStringDataSource>(XMLString);
    CXMLReader Reader(DataSource);
    SXMLEntity TempEntity;

    EXPECT_FALSE(Reader.End());

    EXPECT_TRUE(Reader.ReadEntity(TempEntity));
    EXPECT_EQ(TempEntity.DType,SXMLEntity::EType::StartElement);
    EXPECT_EQ(TempEntity.DNameData, "name");
    ASSERT_EQ(TempEntity.DAttributes.size(), 1);
    EXPECT_EQ(TempEntity.AttributeValue("first"), "Bob");
    EXPECT_EQ(TempEntity.AttributeValue("second"), ""); //Attribute doesn't exist

    EXPECT_TRUE(Reader.ReadEntity(TempEntity));
    EXPECT_EQ(TempEntity.DType,SXMLEntity::EType::StartElement);
    EXPECT_EQ(TempEntity.DNameData, "stats");
    ASSERT_EQ(TempEntity.DAttributes.size(), 0);

    EXPECT_TRUE(Reader.ReadEntity(TempEntity));
    EXPECT_EQ(TempEntity.DType,SXMLEntity::EType::StartElement);
    EXPECT_EQ(TempEntity.DNameData, "gender");
    ASSERT_EQ(TempEntity.DAttributes.size(), 0);

    EXPECT_TRUE(Reader.ReadEntity(TempEntity, true));
    EXPECT_EQ(TempEntity.DType,SXMLEntity::EType::EndElement);
    EXPECT_EQ(TempEntity.DNameData, "gender");

    EXPECT_TRUE(Reader.ReadEntity(TempEntity));
    EXPECT_EQ(TempEntity.DType,SXMLEntity::EType::StartElement);
    EXPECT_EQ(TempEntity.DNameData, "height");
    ASSERT_EQ(TempEntity.DAttributes.size(), 0);

    EXPECT_TRUE(Reader.ReadEntity(TempEntity));
    EXPECT_EQ(TempEntity.DType,SXMLEntity::EType::CharData);
    EXPECT_EQ(TempEntity.DNameData, "5\'9\"");

    EXPECT_TRUE(Reader.ReadEntity(TempEntity));
    EXPECT_EQ(TempEntity.DType,SXMLEntity::EType::EndElement);
    EXPECT_EQ(TempEntity.DNameData, "height");

    EXPECT_TRUE(Reader.ReadEntity(TempEntity));
    EXPECT_EQ(TempEntity.DType,SXMLEntity::EType::StartElement);
    EXPECT_EQ(TempEntity.DNameData, "weight");
    ASSERT_EQ(TempEntity.DAttributes.size(), 0);

    EXPECT_TRUE(Reader.ReadEntity(TempEntity, true));
    EXPECT_EQ(TempEntity.DType,SXMLEntity::EType::EndElement);
    EXPECT_EQ(TempEntity.DNameData, "weight");

    EXPECT_TRUE(Reader.ReadEntity(TempEntity, true));
    EXPECT_EQ(TempEntity.DType,SXMLEntity::EType::EndElement);
    EXPECT_EQ(TempEntity.DNameData, "stats");

    EXPECT_TRUE(Reader.ReadEntity(TempEntity, true));
    EXPECT_EQ(TempEntity.DType,SXMLEntity::EType::EndElement);
    EXPECT_EQ(TempEntity.DNameData, "name");

    EXPECT_TRUE(Reader.End());
    EXPECT_FALSE(Reader.ReadEntity(TempEntity, true));
}

TEST(XMLReader, InvalidSkipDataTest){
    std::string XMLString = "<name>Bob</name>";
    std::shared_ptr<CStringDataSource> DataSource = std::make_shared<CStringDataSource>(XMLString);
    CXMLReader Reader(DataSource);
    SXMLEntity TempEntity;
    
    EXPECT_TRUE(Reader.ReadEntity(TempEntity, true));
    EXPECT_EQ(TempEntity.DType,SXMLEntity::EType::StartElement);
    EXPECT_EQ(TempEntity.DNameData, "name");
    EXPECT_TRUE(TempEntity.DAttributes.empty());

    EXPECT_TRUE(Reader.ReadEntity(TempEntity, true));
    EXPECT_EQ(TempEntity.DType,SXMLEntity::EType::EndElement);
    EXPECT_EQ(TempEntity.DNameData, "name");

    EXPECT_FALSE(Reader.ReadEntity(TempEntity, true));
}

//Write Test
TEST(XMLWriter, EmptyTest){
    std::shared_ptr<CStringDataSink> DataSink = std::make_shared<CStringDataSink>();
    CXMLWriter Writer(DataSink);
    SXMLEntity Entity1;

    EXPECT_FALSE(Writer.WriteEntity(Entity1));
}

TEST(XMLWriter, SimpleTest){    
    std::shared_ptr<CStringDataSink> DataSink = std::make_shared<CStringDataSink>();
    CXMLWriter Writer(DataSink);
    SXMLEntity Entity1 = {
        .DType = SXMLEntity::EType::StartElement, 
        .DNameData = "tag",
    };
    SXMLEntity Entity2 = {
        .DType = SXMLEntity::EType::EndElement, 
        .DNameData = "tag",
    };

    EXPECT_TRUE(Writer.WriteEntity(Entity1));
    EXPECT_TRUE(Writer.WriteEntity(Entity2));
    EXPECT_EQ(DataSink->String(), "<tag></tag>");

}

TEST(XMLWriter, CharDataTest){    
    std::shared_ptr<CStringDataSink> DataSink = std::make_shared<CStringDataSink>();
    CXMLWriter Writer(DataSink);
    SXMLEntity Entity1 = {
        .DType = SXMLEntity::EType::StartElement, 
        .DNameData = "name",
    };
    SXMLEntity Entity2 = {
        .DType = SXMLEntity::EType::CharData, 
        .DNameData = "Bob",
    };
    SXMLEntity Entity3 = {
        .DType = SXMLEntity::EType::EndElement, 
        .DNameData = "name",
    };

    EXPECT_TRUE(Writer.WriteEntity(Entity1));
    EXPECT_TRUE(Writer.WriteEntity(Entity2));
    EXPECT_TRUE(Writer.WriteEntity(Entity3));
    EXPECT_EQ(DataSink->String(), "<name>Bob</name>");
}

TEST(XMLWriter, AttributeTest){    
    std::shared_ptr<CStringDataSink> DataSink = std::make_shared<CStringDataSink>();
    CXMLWriter Writer(DataSink);
    SXMLEntity Entity1 = {
        .DType = SXMLEntity::EType::StartElement, 
        .DNameData = "tag",
        .DAttributes = std::vector<std::pair<std::string, std::string>>{std::make_pair("attr1", "val1")}
    };
    SXMLEntity Entity2 = {
        .DType = SXMLEntity::EType::CharData, 
        .DNameData = "Some Data",
    };
    SXMLEntity Entity3 = {
        .DType = SXMLEntity::EType::EndElement, 
        .DNameData = "tag",
    };

    EXPECT_TRUE(Writer.WriteEntity(Entity1));
    EXPECT_TRUE(Writer.WriteEntity(Entity2));
    EXPECT_TRUE(Writer.WriteEntity(Entity3));
    EXPECT_EQ(DataSink->String(), "<tag attr1=\"val1\">Some Data</tag>");
}

TEST(XMLWriter, HardTest){    
    std::shared_ptr<CStringDataSink> DataSink = std::make_shared<CStringDataSink>();
    CXMLWriter Writer(DataSink);
    SXMLEntity Entity1 = {
        .DType = SXMLEntity::EType::StartElement, 
        .DNameData = "letter",
        .DAttributes = std::vector<std::pair<std::string, std::string>>{std::make_pair("id", "789"), std::make_pair("date", "1/31/26")}
    };
    SXMLEntity Entity2 = {
        .DType = SXMLEntity::EType::StartElement, 
        .DNameData = "to",
    };
    SXMLEntity Entity3 = {
        .DType = SXMLEntity::EType::CharData, 
        .DNameData = "Bob",
    };
    SXMLEntity Entity4 = {
        .DType = SXMLEntity::EType::EndElement, 
        .DNameData = "to",
    };
    SXMLEntity Entity5 = {
        .DType = SXMLEntity::EType::StartElement, 
        .DNameData = "from",
    };
    SXMLEntity Entity6= {
        .DType = SXMLEntity::EType::CharData, 
        .DNameData = "Joey",
    };
    SXMLEntity Entity7 = {
        .DType = SXMLEntity::EType::EndElement, 
        .DNameData = "from",
    };
    SXMLEntity Entity8 = {
        .DType = SXMLEntity::EType::StartElement, 
        .DNameData = "heading",
        .DAttributes = std::vector<std::pair<std::string, std::string>>{std::make_pair("font", "Times New Roman")}
    };
    SXMLEntity Entity9 = {
        .DType = SXMLEntity::EType::CharData, 
        .DNameData = "Appointment",
    };
    SXMLEntity Entity10 = {
        .DType = SXMLEntity::EType::EndElement, 
        .DNameData = "heading",
    };
    SXMLEntity Entity11 = {
        .DType = SXMLEntity::EType::StartElement, 
        .DNameData = "body",
    };
    SXMLEntity Entity12 = {
        .DType = SXMLEntity::EType::CharData, 
        .DNameData = "\"Where are we meeting tomorrow?\"",
    };
    SXMLEntity Entity13 = {
        .DType = SXMLEntity::EType::EndElement, 
        .DNameData = "body",
    };
    SXMLEntity Entity14 = {
        .DType = SXMLEntity::EType::EndElement, 
        .DNameData = "letter",
    };

    EXPECT_TRUE(Writer.WriteEntity(Entity1));
    EXPECT_TRUE(Writer.WriteEntity(Entity2));
    EXPECT_TRUE(Writer.WriteEntity(Entity3));
    EXPECT_TRUE(Writer.WriteEntity(Entity4));
    EXPECT_TRUE(Writer.WriteEntity(Entity5));
    EXPECT_TRUE(Writer.WriteEntity(Entity6));
    EXPECT_TRUE(Writer.WriteEntity(Entity7));
    EXPECT_TRUE(Writer.WriteEntity(Entity8));
    EXPECT_TRUE(Writer.WriteEntity(Entity9));
    EXPECT_TRUE(Writer.WriteEntity(Entity10));
    EXPECT_TRUE(Writer.WriteEntity(Entity11));
    EXPECT_TRUE(Writer.WriteEntity(Entity12));
    EXPECT_TRUE(Writer.WriteEntity(Entity13));
    EXPECT_TRUE(Writer.WriteEntity(Entity14));
    EXPECT_EQ(DataSink->String(), "<letter id=\"789\" date=\"1/31/26\"><to>Bob</to><from>Joey</from><heading font=\"Times New Roman\">Appointment</heading><body>&quot;Where are we meeting tomorrow?&quot;</body></letter>");
}

TEST(XMLWriter, EscapeCharacterTest){    
    std::shared_ptr<CStringDataSink> DataSink = std::make_shared<CStringDataSink>();
    CXMLWriter Writer(DataSink);
    SXMLEntity Entity1 = {
        .DType = SXMLEntity::EType::StartElement, 
        .DNameData = "tag",
    };
    SXMLEntity Entity2 = {
        .DType = SXMLEntity::EType::CharData, 
        .DNameData = "Apple & Orange > Banana < Coconut",
    };
    SXMLEntity Entity3 = {
        .DType = SXMLEntity::EType::EndElement, 
        .DNameData = "tag",
    };

    EXPECT_TRUE(Writer.WriteEntity(Entity1));
    EXPECT_TRUE(Writer.WriteEntity(Entity2));
    EXPECT_TRUE(Writer.WriteEntity(Entity3));
    EXPECT_EQ(DataSink->String(), "<tag>Apple &amp; Orange &gt; Banana &lt; Coconut</tag>");
}

TEST(XMLWriter, SimpleCompleteElementTest){    
    std::shared_ptr<CStringDataSink> DataSink = std::make_shared<CStringDataSink>();
    CXMLWriter Writer(DataSink);
    SXMLEntity Entity1 = {
        .DType = SXMLEntity::EType::CompleteElement, 
        .DNameData = "test",
    };

    EXPECT_TRUE(Writer.WriteEntity(Entity1));
    EXPECT_EQ(DataSink->String(), "<test/>");
}

TEST(XMLWriter, CompleteElementTest){    
    std::shared_ptr<CStringDataSink> DataSink = std::make_shared<CStringDataSink>();
    CXMLWriter Writer(DataSink);
    SXMLEntity Entity1 = {
        .DType = SXMLEntity::EType::CompleteElement, 
        .DNameData = "tag",
    };
    Entity1.SetAttribute("", "test");
    SXMLEntity Entity2 = {
        .DType = SXMLEntity::EType::CompleteElement, 
        .DNameData = "name",
    };
    Entity2.SetAttribute("first", "Bob");
    EXPECT_TRUE(Entity2.AttributeExists("first"));
    EXPECT_FALSE(Entity2.AttributeExists("last"));

    EXPECT_TRUE(Writer.WriteEntity(Entity1));
    EXPECT_TRUE(Writer.WriteEntity(Entity2));
    EXPECT_EQ(DataSink->String(), "<tag/><name first=\"Bob\"/>");
}

TEST(XMLWriter, CompleteElementTestWithAttributes){    
    std::shared_ptr<CStringDataSink> DataSink = std::make_shared<CStringDataSink>();
    CXMLWriter Writer(DataSink);
    SXMLEntity Entity1 = {
        .DType = SXMLEntity::EType::CompleteElement, 
        .DNameData = "tag",
    };
    Entity1.SetAttribute("age", "20");
    Entity1.SetAttribute("gender", "male");
    Entity1.SetAttribute("age", "21");
    Entity1.SetAttribute("first", "Bob");

    EXPECT_TRUE(Writer.WriteEntity(Entity1));
    EXPECT_EQ(DataSink->String(), "<tag age=\"21\" gender=\"male\" first=\"Bob\"/>");
}

TEST(XMLWriter, SimpleTestWithFlush){    
    std::shared_ptr<CStringDataSink> DataSink = std::make_shared<CStringDataSink>();
    CXMLWriter Writer(DataSink);
    SXMLEntity Entity1 = {
        .DType = SXMLEntity::EType::StartElement, 
        .DNameData = "tag",
    };
    SXMLEntity Entity2 = {
        .DType = SXMLEntity::EType::EndElement, 
        .DNameData = "tag",
    };

    EXPECT_TRUE(Writer.WriteEntity(Entity1));
    EXPECT_TRUE(Writer.Flush());
    EXPECT_EQ(DataSink->String(), "<tag></tag>");
}

TEST(XMLWriter, FlushTest){    //CAGNE
    std::shared_ptr<CStringDataSink> DataSink = std::make_shared<CStringDataSink>();
    CXMLWriter Writer(DataSink);
    SXMLEntity Entity1 = {
        .DType = SXMLEntity::EType::StartElement, 
        .DNameData = "name",
    };
    SXMLEntity Entity2 = {
        .DType = SXMLEntity::EType::StartElement, 
        .DNameData = "id",
    };
    SXMLEntity Entity3 = {
        .DType = SXMLEntity::EType::StartElement, 
        .DNameData = "age",
    };
    SXMLEntity Entity4 = {
        .DType = SXMLEntity::EType::StartElement, 
        .DNameData = "gender",
    };
    SXMLEntity Entity5 = {
        .DType = SXMLEntity::EType::StartElement, 
        .DNameData = "height",
    };
    SXMLEntity Entity6 = {
        .DType = SXMLEntity::EType::CharData, 
        .DNameData = "&",
    };
    SXMLEntity Entity7 = {
        .DType = SXMLEntity::EType::EndElement, 
        .DNameData = "height",
    };
   SXMLEntity Entity8 = {
        .DType = SXMLEntity::EType::StartElement, 
        .DNameData = "weight",
    };
    SXMLEntity Entity9= {
        .DType = SXMLEntity::EType::CharData, 
        .DNameData = "'",
    };

    EXPECT_TRUE(Writer.WriteEntity(Entity1));
    EXPECT_TRUE(Writer.WriteEntity(Entity2));
    EXPECT_TRUE(Writer.WriteEntity(Entity3));
    EXPECT_TRUE(Writer.WriteEntity(Entity4));
    EXPECT_TRUE(Writer.WriteEntity(Entity5));
    EXPECT_TRUE(Writer.WriteEntity(Entity6));
    EXPECT_TRUE(Writer.WriteEntity(Entity7));
    EXPECT_TRUE(Writer.WriteEntity(Entity8));
    EXPECT_TRUE(Writer.WriteEntity(Entity9));
    EXPECT_TRUE(Writer.Flush());
    EXPECT_EQ(DataSink->String(), "<name><id><age><gender><height>&amp;</height><weight>&apos;</weight></gender></age></id></name>");
}
