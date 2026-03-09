#include <gtest/gtest.h>
#include "DSVWriter.h"
#include "DSVReader.h"
#include "StringDataSink.h"
#include "StringDataSource.h"

//Write Test
TEST(DSVWriterTest, EmptyRowTest){
    std::shared_ptr<CStringDataSink> DataSink = std::make_shared<CStringDataSink>();
    CDSVWriter Writer(DataSink,',');

    EXPECT_TRUE(DataSink->String().empty());
    EXPECT_EQ(DataSink->String(),"");
    EXPECT_TRUE(Writer.WriteRow({}));
    EXPECT_EQ(DataSink->String(),"");

}

TEST(DSVWriterTest, EmptyStringRowTest){
    std::shared_ptr<CStringDataSink> DataSink = std::make_shared<CStringDataSink>();
    CDSVWriter Writer(DataSink,',');

    EXPECT_TRUE(DataSink->String().empty());
    EXPECT_EQ(DataSink->String(),"");
    EXPECT_TRUE(Writer.WriteRow({""}));
    EXPECT_EQ(DataSink->String(),"");

}

TEST(DSVWriterTest, SingleRowTest){
    std::shared_ptr<CStringDataSink> DataSink = std::make_shared<CStringDataSink>();
    CDSVWriter Writer(DataSink,',');

    EXPECT_TRUE(DataSink->String().empty());
    EXPECT_EQ(DataSink->String(),"");
    EXPECT_TRUE(Writer.WriteRow({"A","B","C"}));
    EXPECT_EQ(DataSink->String(),"A,B,C");

}

TEST(DSVWriterTest, SingleRowTestWithQuoteDelimiter){
    std::shared_ptr<CStringDataSink> DataSink = std::make_shared<CStringDataSink>();
    CDSVWriter Writer(DataSink,'\"');

    EXPECT_TRUE(DataSink->String().empty());
    EXPECT_EQ(DataSink->String(),"");
    EXPECT_TRUE(Writer.WriteRow({"A","B","C"}));
    EXPECT_EQ(DataSink->String(),"A,B,C");

}

TEST(DSVWriterTest, SingleRowTestQuoteall){
    std::shared_ptr<CStringDataSink> DataSink = std::make_shared<CStringDataSink>();
    CDSVWriter Writer(DataSink,',',true);

    EXPECT_TRUE(DataSink->String().empty());
    EXPECT_EQ(DataSink->String(),"");
    EXPECT_TRUE(Writer.WriteRow({"A","B","C"}));
    EXPECT_EQ(DataSink->String(),"\"A\",\"B\",\"C\"");

}

TEST(DSVWriterTest, SingleRowTestWithCommaDelimeter){ //Only put quotes around the specific string that requires it
    std::shared_ptr<CStringDataSink> DataSink = std::make_shared<CStringDataSink>();
    CDSVWriter Writer(DataSink,',');

    EXPECT_TRUE(DataSink->String().empty());
    EXPECT_EQ(DataSink->String(),"");
    EXPECT_TRUE(Writer.WriteRow({"A","B,C"}));
    EXPECT_EQ(DataSink->String(),"A,\"B,C\"");

}

TEST(DSVWriterTest, SingleRowTestWithTabDelimeter){ 
    std::shared_ptr<CStringDataSink> DataSink = std::make_shared<CStringDataSink>();
    CDSVWriter Writer(DataSink,'\t');

    EXPECT_TRUE(DataSink->String().empty());
    EXPECT_EQ(DataSink->String(),"");
    EXPECT_TRUE(Writer.WriteRow({"A","B,C"}));
    EXPECT_EQ(DataSink->String(),"A\tB,C");

}

TEST(DSVWriterTest, SingleRowTestWithQuote){
    std::shared_ptr<CStringDataSink> DataSink = std::make_shared<CStringDataSink>();
    CDSVWriter Writer(DataSink,',');

    EXPECT_TRUE(DataSink->String().empty());
    EXPECT_EQ(DataSink->String(),"");
    EXPECT_TRUE(Writer.WriteRow({"A","B","\"C\""}));
    EXPECT_EQ(DataSink->String(),"A,B,\"\"\"C\"\"\""); //Outer quote, double quotes to replace "

}

TEST(DSVWriterTest, SingleRowTestWithNewline){
    std::shared_ptr<CStringDataSink> DataSink = std::make_shared<CStringDataSink>();
    CDSVWriter Writer(DataSink,',');

    EXPECT_TRUE(DataSink->String().empty());
    EXPECT_EQ(DataSink->String(),"");
    EXPECT_TRUE(Writer.WriteRow({"A","B","C\n"}));
    EXPECT_EQ(DataSink->String(),"A,B,\"C\n\"");

}

TEST(DSVWriterTest, SingleRowTestWithExclamtionDelimiterQuoteNewLine){
    std::shared_ptr<CStringDataSink> DataSink = std::make_shared<CStringDataSink>();
    CDSVWriter Writer(DataSink,'!'); //space?

    EXPECT_TRUE(DataSink->String().empty());
    EXPECT_EQ(DataSink->String(),"");
    EXPECT_TRUE(Writer.WriteRow({"Apple! \"Orange\"! Banana\n"}));
    EXPECT_EQ(DataSink->String(),"\"Apple! \"\"Orange\"\"! Banana\n\"");

}


TEST(DSVWriterTest, SingleRowTestEdgeCase){
    std::shared_ptr<CStringDataSink> DataSink = std::make_shared<CStringDataSink>();
    CDSVWriter Writer(DataSink,','); //space?

    EXPECT_TRUE(DataSink->String().empty());
    EXPECT_EQ(DataSink->String(),"");
    EXPECT_TRUE(Writer.WriteRow({"Banana","A\",\""}));
    EXPECT_EQ(DataSink->String(),"Banana,\"A\"\",\"\"\"");

}


TEST(DSVWriterTest, MultiRowTest){
    std::shared_ptr<CStringDataSink> DataSink = std::make_shared<CStringDataSink>();
    CDSVWriter Writer(DataSink,',');

    EXPECT_TRUE(DataSink->String().empty());
    EXPECT_EQ(DataSink->String(),"");
    EXPECT_TRUE(Writer.WriteRow({"A","B","C"}));
    EXPECT_TRUE(Writer.WriteRow({"D","E","F"}));
    EXPECT_TRUE(Writer.WriteRow({"G","H","I"}));
    EXPECT_EQ(DataSink->String(),"A,B,C\nD,E,F\nG,H,I");
}
    

TEST(DSVWriterTest, MultiRowTestWithDelimiter){
    std::shared_ptr<CStringDataSink> DataSink = std::make_shared<CStringDataSink>();
    CDSVWriter Writer(DataSink,',');

    EXPECT_TRUE(DataSink->String().empty());
    EXPECT_EQ(DataSink->String(),"");
    EXPECT_TRUE(Writer.WriteRow({"A,B","C,D"}));
    EXPECT_TRUE(Writer.WriteRow({"E","F","G"}));
    EXPECT_EQ(DataSink->String(),"\"A,B\",\"C,D\"\nE,F,G");
}
    

TEST(DSVWriterTest, MultiRowTestWithDelimiterQuoteEmptyRow){
    std::shared_ptr<CStringDataSink> DataSink = std::make_shared<CStringDataSink>();
    CDSVWriter Writer(DataSink,',');

    EXPECT_TRUE(DataSink->String().empty());
    EXPECT_EQ(DataSink->String(),"");
    EXPECT_TRUE(Writer.WriteRow({"A,B",""}));
    EXPECT_TRUE(Writer.WriteRow({}));
    EXPECT_TRUE(Writer.WriteRow({""}));
    EXPECT_TRUE(Writer.WriteRow({"\"Hello\"","World!"}));
    EXPECT_EQ(DataSink->String(), "\"A,B\",\n\n\n\"\"\"Hello\"\"\",World!");

}

TEST(DSVWriterTest, EmptyRowShouldNotAffectNextRow){
    std::shared_ptr<CStringDataSink> DataSink = std::make_shared<CStringDataSink>();
    CDSVWriter Writer(DataSink, ',');

    EXPECT_TRUE(DataSink->String().empty());
    EXPECT_EQ(DataSink->String(), "");

    EXPECT_TRUE(Writer.WriteRow({}));      
    EXPECT_TRUE(Writer.WriteRow({"A"})); 
    EXPECT_EQ(DataSink->String(), "\nA");
}

//Read Test
TEST(DSVReaderTest, EmptyRowTest){
    std::shared_ptr<CStringDataSource> DataSource = std::make_shared<CStringDataSource>("");
    CDSVReader Reader(DataSource, ',');
    std::vector<std::string> row = {};

    EXPECT_TRUE(DataSource->End());
    EXPECT_FALSE(Reader.ReadRow(row));
    EXPECT_EQ(row,std::vector<std::string>{});
    EXPECT_TRUE(Reader.End());

}

TEST(DSVReaderTest, SingleRowTest){
    std::shared_ptr<CStringDataSource> DataSource = std::make_shared<CStringDataSource>("A,B,C");
    CDSVReader Reader(DataSource, ',');
    std::vector<std::string> row = {};

    EXPECT_FALSE(DataSource->End());
    EXPECT_TRUE(Reader.ReadRow(row));
    EXPECT_EQ(row,(std::vector<std::string>{"A","B","C"}));
    EXPECT_TRUE(Reader.End());
    
}

TEST(DSVReaderTest, SingleRowEdgeCase1){
    std::shared_ptr<CStringDataSource> DataSource = std::make_shared<CStringDataSource>("A,");
    CDSVReader Reader(DataSource, ',');
    std::vector<std::string> row = {};

    EXPECT_FALSE(DataSource->End());
    EXPECT_TRUE(Reader.ReadRow(row));
    EXPECT_EQ(row,(std::vector<std::string>{"A", ""}));
    EXPECT_TRUE(Reader.End());
    
}

TEST(DSVReaderTest, SingleRowTestEdgeCase2){
    std::shared_ptr<CStringDataSource> DataSource = std::make_shared<CStringDataSource>("A,\"\"\"\"\"\"");
    CDSVReader Reader(DataSource, ',');
    std::vector<std::string> row = {};

    EXPECT_FALSE(DataSource->End());
    EXPECT_TRUE(Reader.ReadRow(row));
    EXPECT_EQ(row,(std::vector<std::string>{"A","\"\""}));
    EXPECT_TRUE(Reader.End());
    
}

TEST(DSVReaderTest, SingleRowTestWithQuoteDelimiter){
    std::shared_ptr<CStringDataSource> DataSource = std::make_shared<CStringDataSource>("A,B,C");
    CDSVReader Reader(DataSource, '\"');
    std::vector<std::string> row = {};

    EXPECT_FALSE(DataSource->End());
    EXPECT_TRUE(Reader.ReadRow(row));
    EXPECT_EQ(row,(std::vector<std::string>{"A","B","C"}));
    EXPECT_TRUE(Reader.End());

}

TEST(DSVReaderTest, SingleRowTestQuoteall){
    std::shared_ptr<CStringDataSource> DataSource = std::make_shared<CStringDataSource>("\"A\",\"B\",\"C\"");
    CDSVReader Reader(DataSource, ',');
    std::vector<std::string> row = {};

    EXPECT_FALSE(DataSource->End());
    EXPECT_TRUE(Reader.ReadRow(row));
    EXPECT_EQ(row,(std::vector<std::string>{"A","B","C"}));
    EXPECT_TRUE(Reader.End());
    
}

TEST(DSVReaderTest, SingleRowTestWithCommaDelimeter){
    std::shared_ptr<CStringDataSource> DataSource = std::make_shared<CStringDataSource>("A,\"B,C\"");
    CDSVReader Reader(DataSource, ',');
    std::vector<std::string> row = {};

    EXPECT_FALSE(DataSource->End());
    EXPECT_TRUE(Reader.ReadRow(row));
    EXPECT_EQ(row,(std::vector<std::string>{"A","B,C"}));
    EXPECT_TRUE(Reader.End());
    
}

TEST(DSVReaderTest, SingleRowTestWithTabDelimeter){
    std::shared_ptr<CStringDataSource> DataSource = std::make_shared<CStringDataSource>("A\tB,C");
    CDSVReader Reader(DataSource, '\t');
    std::vector<std::string> row = {};

    EXPECT_FALSE(DataSource->End());
    EXPECT_TRUE(Reader.ReadRow(row));
    EXPECT_EQ(row,(std::vector<std::string>{"A","B,C"}));
    EXPECT_TRUE(Reader.End());
    
}

TEST(DSVReaderTest, SingleRowTestWithQuote){
    std::shared_ptr<CStringDataSource> DataSource = std::make_shared<CStringDataSource>("A,B,\"\"\"C\"\"\"");
    CDSVReader Reader(DataSource, ',');
    std::vector<std::string> row = {};

    EXPECT_FALSE(DataSource->End());
    EXPECT_TRUE(Reader.ReadRow(row));
    EXPECT_EQ(row,(std::vector<std::string>{"A","B","\"C\""}));
    EXPECT_TRUE(Reader.End());
    
}

TEST(DSVReaderTest, SingleRowTestWithNewline){
    std::shared_ptr<CStringDataSource> DataSource = std::make_shared<CStringDataSource>("A,B,\"C\n\"");
    CDSVReader Reader(DataSource, ',');
    std::vector<std::string> row = {};

    EXPECT_FALSE(DataSource->End());
    EXPECT_TRUE(Reader.ReadRow(row));
    EXPECT_EQ(row,(std::vector<std::string>{"A","B","C\n"}));
    EXPECT_TRUE(Reader.End());
    
}

TEST(DSVReaderTest, SingleRowTestWithExclamtionDelimiterQuoteNewLine){
    std::shared_ptr<CStringDataSource> DataSource = std::make_shared<CStringDataSource>("\"Apple! \"\"Orange\"\"! Banana\n\"");
    CDSVReader Reader(DataSource, '!');
    std::vector<std::string> row = {};

    EXPECT_FALSE(DataSource->End());
    EXPECT_TRUE(Reader.ReadRow(row));
    EXPECT_EQ(row,(std::vector<std::string>{"Apple! \"Orange\"! Banana\n"}));
    EXPECT_TRUE(Reader.End());
    
}

TEST(DSVReaderTest, SingleRowTestEdgeCase){
    std::shared_ptr<CStringDataSource> DataSource = std::make_shared<CStringDataSource>("Banana,\"A\"\",\"\"\"");
    CDSVReader Reader(DataSource, ',');
    std::vector<std::string> row = {};

    EXPECT_FALSE(DataSource->End());
    EXPECT_TRUE(Reader.ReadRow(row));
    EXPECT_EQ(row,(std::vector<std::string>{"Banana","A\",\""}));
    EXPECT_TRUE(Reader.End());
    
}

TEST(DSVReaderTest, MultiRowTest){
    std::shared_ptr<CStringDataSource> DataSource = std::make_shared<CStringDataSource>("A,B,C\nD,E,F\nG,H,I");
    CDSVReader Reader(DataSource, ',');
    std::vector<std::string> row = {};

    EXPECT_FALSE(DataSource->End());
    EXPECT_TRUE(Reader.ReadRow(row));
    EXPECT_EQ(row,(std::vector<std::string>{"A","B","C"}));
    EXPECT_TRUE(Reader.ReadRow(row));
    EXPECT_EQ(row,(std::vector<std::string>{"D","E","F"}));
    EXPECT_TRUE(Reader.ReadRow(row));
    EXPECT_EQ(row,(std::vector<std::string>{"G","H","I"}));
    EXPECT_TRUE(Reader.End());
    
}

TEST(DSVReaderTest, MultiRowTestWithDelimiter){
    std::shared_ptr<CStringDataSource> DataSource = std::make_shared<CStringDataSource>("\"A,B\",\"C,D\"\nE,F,G");
    CDSVReader Reader(DataSource, ',');
    std::vector<std::string> row = {};

    EXPECT_FALSE(DataSource->End());
    EXPECT_TRUE(Reader.ReadRow(row));
    EXPECT_EQ(row,(std::vector<std::string>{"A,B","C,D"}));
    EXPECT_TRUE(Reader.ReadRow(row));
    EXPECT_EQ(row,(std::vector<std::string>{"E","F","G"}));
    EXPECT_TRUE(Reader.End());
    
}

TEST(DSVReaderTest, MultiRowTestWithDelimiterQuoteEmptyRow){
    std::shared_ptr<CStringDataSource> DataSource = std::make_shared<CStringDataSource>("\"A,B\",\n\n\n\"\"\"Hello\"\"\",World!");
    CDSVReader Reader(DataSource, ',');
    std::vector<std::string> row = {};

    EXPECT_FALSE(DataSource->End());
    EXPECT_TRUE(Reader.ReadRow(row));
    EXPECT_EQ(row,(std::vector<std::string>{"A,B",""}));
    EXPECT_TRUE(Reader.ReadRow(row));
    EXPECT_EQ(row,(std::vector<std::string>{""})); 
    EXPECT_TRUE(Reader.ReadRow(row));
    EXPECT_EQ(row,(std::vector<std::string>{""}));
    EXPECT_TRUE(Reader.ReadRow(row));
    EXPECT_EQ(row,(std::vector<std::string>{"\"Hello\"","World!"}));
    EXPECT_TRUE(Reader.End());
    
}

TEST(DSVReaderTest, QuoteDelimiterAsCommaTest){
    std::shared_ptr<CStringDataSource> DataSource = std::make_shared<CStringDataSource>("A,B\n");
    CDSVReader Reader(DataSource, '"'); 
    std::vector<std::string> row = {};

    EXPECT_FALSE(DataSource->End());
    EXPECT_TRUE(Reader.ReadRow(row));
    EXPECT_EQ(row, (std::vector<std::string>{"A", "B"}));
    EXPECT_TRUE(Reader.End());
}

TEST(DSVReaderTest, TrailingDelimiterAtEOFShouldKeepEmptyField){
    std::shared_ptr<CStringDataSource> DataSource = std::make_shared<CStringDataSource>("A,");
    CDSVReader Reader(DataSource, ',');
    std::vector<std::string> row = {};

    EXPECT_FALSE(DataSource->End());
    EXPECT_TRUE(Reader.ReadRow(row));
    EXPECT_EQ(row, (std::vector<std::string>{"A", ""}));
    EXPECT_TRUE(Reader.End());
}

TEST(DSVReaderTest, TrailingDelimiterBeforeNewlineShouldKeepEmptyField){
    std::shared_ptr<CStringDataSource> DataSource = std::make_shared<CStringDataSource>("A,\n");
    CDSVReader Reader(DataSource, ',');
    std::vector<std::string> row = {};

    EXPECT_FALSE(DataSource->End());
    EXPECT_TRUE(Reader.ReadRow(row));
    EXPECT_EQ(row, (std::vector<std::string>{"A", ""}));
    EXPECT_TRUE(Reader.End());
}
