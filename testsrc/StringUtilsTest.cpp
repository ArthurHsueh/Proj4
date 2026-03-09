#include <gtest/gtest.h>
#include "StringUtils.h"
// Write small code first, add failing functions, add right tests that should pass
TEST(StringUtilsTest, SliceTest){ // length + n
    EXPECT_EQ(StringUtils::Slice("Hello World", 1, 11), "ello World");
    EXPECT_EQ(StringUtils::Slice("Hello World", 5, 0), " World");
    EXPECT_EQ(StringUtils::Slice("Hello World", 2, -3), "llo Wo");
    EXPECT_EQ(StringUtils::Slice("Hello World", -6, -3), " Wo");
    EXPECT_EQ(StringUtils::Slice("Hello World", 3, 7), "lo W");
    EXPECT_EQ(StringUtils::Slice("Hello World", 5, 3), "");
    EXPECT_EQ(StringUtils::Slice("Hello World", -2, -5), "");
    EXPECT_EQ(StringUtils::Slice("Hello World", -1, 3), "");


    EXPECT_EQ(StringUtils::Slice("Strawberry", 0, 5), "Straw");
    EXPECT_EQ(StringUtils::Slice("Strawberry", 2, 8), "rawber");
    EXPECT_EQ(StringUtils::Slice("Strawberry", 0, 0), "Strawberry");
    EXPECT_EQ(StringUtils::Slice("Strawberry", 3, 0), "awberry");
    EXPECT_EQ(StringUtils::Slice("Strawberry", 0, 100), "Strawberry");
    EXPECT_EQ(StringUtils::Slice("Strawberry", 100, 5), "");
    EXPECT_EQ(StringUtils::Slice("Strawberry", -7, 10), "awberry");
    EXPECT_EQ(StringUtils::Slice("Strawberry", -7, 3), "");
    EXPECT_EQ(StringUtils::Slice("Strawberry", -7, -2), "awber");
    EXPECT_EQ(StringUtils::Slice("Strawberry", -7, 0), "awberry");
    EXPECT_EQ(StringUtils::Slice("Strawberry", -100, 5), "Straw");
    EXPECT_EQ(StringUtils::Slice("Strawberry", 3, -5), "aw");
    EXPECT_EQ(StringUtils::Slice("Strawberry", 3, -100), "");
    EXPECT_EQ(StringUtils::Slice("Strawberry", -50, 50), "Strawberry");
    EXPECT_EQ(StringUtils::Slice("Strawberry", 50, -50), "");

    EXPECT_EQ(StringUtils::Slice(" ", 0, 5), " ");
    EXPECT_EQ(StringUtils::Slice(" ", 5, 0), "");
    EXPECT_EQ(StringUtils::Slice(" ", 0, -1), "");
    EXPECT_EQ(StringUtils::Slice(" ", -1, 1), " ");
    EXPECT_EQ(StringUtils::Slice(" ", 0, 1), " ");

    EXPECT_EQ(StringUtils::Slice("A\nB", -1, 3), "B");
    EXPECT_EQ(StringUtils::Slice("A\nB", -1, 2), "");
    EXPECT_EQ(StringUtils::Slice("A\nB", 0, 2), "A\n");
    EXPECT_EQ(StringUtils::Slice("A\nB", 1, 3), "\nB");
    EXPECT_EQ(StringUtils::Slice("A\nB", 1, 2), "\n");
    
    EXPECT_EQ(StringUtils::Slice("", -1, 3), "");
    EXPECT_EQ(StringUtils::Slice("", -1, 2), "");
    EXPECT_EQ(StringUtils::Slice("", 0, 2), "");
    EXPECT_EQ(StringUtils::Slice("", 1, 3), "");
    EXPECT_EQ(StringUtils::Slice("", 1, 2), "");
}

TEST(StringUtilsTest, Capitalize){ //Check in Python and REWORK POTEN
    EXPECT_EQ(StringUtils::Capitalize("hello"), std::string("Hello"));
    EXPECT_EQ(StringUtils::Capitalize("World"), std::string("World"));
    EXPECT_EQ(StringUtils::Capitalize("Hello World"), std::string("Hello world"));
    EXPECT_EQ(StringUtils::Capitalize("hello World"), std::string("Hello world"));
    EXPECT_EQ(StringUtils::Capitalize("HELLO WORLD"), std::string("Hello world"));
    EXPECT_EQ(StringUtils::Capitalize(" "), std::string(" "));
    EXPECT_EQ(StringUtils::Capitalize(""), std::string(""));
    EXPECT_EQ(StringUtils::Capitalize(" hello world"), std::string(" hello world"));
    EXPECT_EQ(StringUtils::Capitalize("\n hello world"), std::string("\n hello world"));
    EXPECT_EQ(StringUtils::Capitalize("98"), std::string("98"));
}

TEST(StringUtilsTest, Upper){
    EXPECT_EQ(StringUtils::Upper("hello"), std::string("HELLO"));
    EXPECT_EQ(StringUtils::Upper("WORLD"), std::string("WORLD"));
    EXPECT_EQ(StringUtils::Upper("HeLlO wOrLd"), std::string("HELLO WORLD"));
    EXPECT_EQ(StringUtils::Upper("APPLE, banana"), std::string("APPLE, BANANA"));
    EXPECT_EQ(StringUtils::Upper(" a "), std::string(" A "));
    EXPECT_EQ(StringUtils::Upper("   "), std::string("   "));
    EXPECT_EQ(StringUtils::Upper(""), std::string(""));
    EXPECT_EQ(StringUtils::Upper("\nt\t"), std::string("\nT\t"));
}

TEST(StringUtilsTest, Lower){
    EXPECT_EQ(StringUtils::Lower("HELLO"), std::string("hello"));
    EXPECT_EQ(StringUtils::Lower("world"), std::string("world"));
    EXPECT_EQ(StringUtils::Lower("HeLlO wOrLd"), std::string("hello world"));
    EXPECT_EQ(StringUtils::Lower("APPLE, banana"), std::string("apple, banana"));
    EXPECT_EQ(StringUtils::Lower(" A "), std::string(" a "));
    EXPECT_EQ(StringUtils::Lower("   "), std::string("   "));
    EXPECT_EQ(StringUtils::Lower(""), std::string(""));
    EXPECT_EQ(StringUtils::Lower("\nT\t"), std::string("\nt\t"));
}

TEST(StringUtilsTest, LStrip){
    EXPECT_EQ(StringUtils::LStrip(" Hello"), std::string("Hello"));
    EXPECT_EQ(StringUtils::LStrip("World"), std::string("World"));
    EXPECT_EQ(StringUtils::LStrip("Hello World!"), std::string("Hello World!"));
    EXPECT_EQ(StringUtils::LStrip(" World "), std::string("World "));
    EXPECT_EQ(StringUtils::LStrip("\tApple"), std::string("Apple"));
    EXPECT_EQ(StringUtils::LStrip(" \n Banana"), std::string("Banana"));
    EXPECT_EQ(StringUtils::LStrip("\t\n  Orange"), std::string("Orange"));
    EXPECT_EQ(StringUtils::LStrip("\t\n Strawberry    Blueberry\t!"), std::string("Strawberry    Blueberry\t!"));
    EXPECT_EQ(StringUtils::LStrip("Apple, Banana, Orange"), std::string("Apple, Banana, Orange"));
    EXPECT_EQ(StringUtils::LStrip("  "), std::string(""));
    EXPECT_EQ(StringUtils::LStrip("\n"), std::string(""));
    EXPECT_EQ(StringUtils::LStrip(""), std::string(""));
}

TEST(StringUtilsTest, RStrip){
    EXPECT_EQ(StringUtils::RStrip("Hello "), std::string("Hello"));
    EXPECT_EQ(StringUtils::RStrip("World"), std::string("World"));
    EXPECT_EQ(StringUtils::RStrip("Hello World!"), std::string("Hello World!"));
    EXPECT_EQ(StringUtils::RStrip(" World "), std::string(" World"));
    EXPECT_EQ(StringUtils::RStrip("Apple\t"), std::string("Apple"));
    EXPECT_EQ(StringUtils::RStrip("Banana \n "), std::string("Banana"));
    EXPECT_EQ(StringUtils::RStrip("Orange  \n\t"), std::string("Orange"));
    EXPECT_EQ(StringUtils::RStrip("\t\n Strawberry    Blueberry\t!"), std::string("\t\n Strawberry    Blueberry\t!"));
    EXPECT_EQ(StringUtils::RStrip("Apple, Banana, Orange"), std::string("Apple, Banana, Orange"));
    EXPECT_EQ(StringUtils::RStrip("  "), std::string(""));
    EXPECT_EQ(StringUtils::RStrip("\n"), std::string(""));
    EXPECT_EQ(StringUtils::RStrip(""), std::string(""));
}

TEST(StringUtilsTest, Strip){
    EXPECT_EQ(StringUtils::Strip(" Hello "), std::string("Hello"));
    EXPECT_EQ(StringUtils::Strip("World"), std::string("World"));
    EXPECT_EQ(StringUtils::Strip("Hello World!"), std::string("Hello World!"));
    EXPECT_EQ(StringUtils::Strip(" World "), std::string("World"));
    EXPECT_EQ(StringUtils::Strip("Apple\t"), std::string("Apple"));
    EXPECT_EQ(StringUtils::Strip(" \n Banana \n "), std::string("Banana"));
    EXPECT_EQ(StringUtils::Strip("\n\t Orange"), std::string("Orange"));
    EXPECT_EQ(StringUtils::Strip("\t\n Strawberry    Blueberry\t!"), std::string("Strawberry    Blueberry\t!"));
    EXPECT_EQ(StringUtils::Strip("Apple, Banana, Orange"), std::string("Apple, Banana, Orange"));
    EXPECT_EQ(StringUtils::Strip("\n Apple,\tBanana,\tOrange \t"), std::string("Apple,\tBanana,\tOrange"));
    EXPECT_EQ(StringUtils::Strip("  "), std::string(""));
    EXPECT_EQ(StringUtils::Strip("\n"), std::string(""));
    EXPECT_EQ(StringUtils::Strip(""), std::string(""));
}

TEST(StringUtilsTest, Center){
    EXPECT_EQ(StringUtils::Center("Hello", 11, '5'), std::string("555Hello555"));
    EXPECT_EQ(StringUtils::Center("Hello", 10, '5'), std::string("55Hello555"));
    EXPECT_EQ(StringUtils::Center("World", 16, '*'), std::string("*****World******"));
    EXPECT_EQ(StringUtils::Center("Banana", 1, '0'), std::string("Banana"));
    EXPECT_EQ(StringUtils::Center("Apple", -1, '*'), std::string("Apple"));
    EXPECT_EQ(StringUtils::Center("Grape", 5, '*'), std::string("Grape"));
    EXPECT_EQ(StringUtils::Center("Berry", 6, '*'), std::string("Berry*"));
    EXPECT_EQ(StringUtils::Center("Strawberry", 11, '*'), std::string("*Strawberry"));
    EXPECT_EQ(StringUtils::Center("Orange", 19, '!'), std::string("!!!!!!!Orange!!!!!!"));
    EXPECT_EQ(StringUtils::Center("", 10, '?'), std::string("??????????"));
    EXPECT_EQ(StringUtils::Center("", 0, '!'), std::string(""));
    EXPECT_EQ(StringUtils::Center("999", 9, '9'), std::string("999999999"));
}

TEST(StringUtilsTest, LJust){
    EXPECT_EQ(StringUtils::LJust("Hello", 7, '*'), std::string("Hello**"));
    EXPECT_EQ(StringUtils::LJust("World", 10, '*'), std::string("World*****"));
    EXPECT_EQ(StringUtils::LJust("Apple", 0, '*'), std::string("Apple"));
    EXPECT_EQ(StringUtils::LJust("Orange", -5, '*'), std::string("Orange"));
    EXPECT_EQ(StringUtils::LJust("", 9, '*'), std::string("*********"));
    EXPECT_EQ(StringUtils::LJust("", 0, '!'), std::string(""));
    EXPECT_EQ(StringUtils::LJust("Test", 4, '*'), std::string("Test"));
}

TEST(StringUtilsTest, RJust){
    EXPECT_EQ(StringUtils::RJust("Hello", 6, '*'), std::string("*Hello"));
    EXPECT_EQ(StringUtils::RJust("World", 7, '*'), std::string("**World"));
    EXPECT_EQ(StringUtils::RJust("Grape", 1, '*'), std::string("Grape"));
    EXPECT_EQ(StringUtils::RJust("Banana", -3, '*'), std::string("Banana"));
    EXPECT_EQ(StringUtils::RJust("00000000", 9, '1'), std::string("100000000"));
    EXPECT_EQ(StringUtils::RJust("", 1, '!'), std::string("!"));
    EXPECT_EQ(StringUtils::RJust("Yes", 3, '*'), std::string("Yes"));
}

TEST(StringUtilsTest, Replace){
    EXPECT_EQ(StringUtils::Replace("Hello", "e", "a"), std::string("Hallo"));
    EXPECT_EQ(StringUtils::Replace("Hello", "ll", "tt"), std::string("Hetto"));
    EXPECT_EQ(StringUtils::Replace("Hello", "ll", " "), std::string("He o"));
    EXPECT_EQ(StringUtils::Replace("Hello World", "l", ""), std::string("Heo Word"));
    EXPECT_EQ(StringUtils::Replace("Hello World", "a", "e"), std::string("Hello World"));
    EXPECT_EQ(StringUtils::Replace("679258793", "92", "29"), std::string("672958793"));
    EXPECT_EQ(StringUtils::Replace("Biology, Ecology, Psychology", "gy", "gist"), std::string("Biologist, Ecologist, Psychologist"));
    EXPECT_EQ(StringUtils::Replace("ROY G BIV", "ROY ", "Red, Orange, Yellow "), std::string("Red, Orange, Yellow G BIV"));
    EXPECT_EQ(StringUtils::Replace("United States of America", "United States of America", "USA"), std::string("USA"));
    EXPECT_EQ(StringUtils::Replace("AAAAA", "AAA", "A"), std::string("AAA"));
    EXPECT_EQ(StringUtils::Replace("AAAAA", "AA", "AAA"), std::string("AAAAAAA"));
    EXPECT_EQ(StringUtils::Replace("AAAAA", "AA", "aa"), std::string("aaaaA"));
    EXPECT_EQ(StringUtils::Replace("Apple", "p", ""), std::string("Ale"));
    EXPECT_EQ(StringUtils::Replace("Banana", "an", ""), std::string("Ba"));
    EXPECT_EQ(StringUtils::Replace("Banana", "na", ""), std::string("Ba"));
    EXPECT_EQ(StringUtils::Replace("Berry", "", "*"), std::string("*B*e*r*r*y*")); //Edge Case
    EXPECT_EQ(StringUtils::Replace("000", "", "1"), std::string("1010101")); //Edge Case
    EXPECT_EQ(StringUtils::Replace("     ", "", "*"), std::string("* * * * * *")); //Edge Case
    EXPECT_EQ(StringUtils::Replace("     ", " ", "*"), std::string("*****"));
    EXPECT_EQ(StringUtils::Replace("   ", "  ", "*"), std::string("* "));
    EXPECT_EQ(StringUtils::Replace("Hello World!\n I'm taking ECS 34 this quarter.\n", " ", "*"), std::string("Hello*World!\n*I'm*taking*ECS*34*this*quarter.\n"));
    EXPECT_EQ(StringUtils::Replace("Hello World!\n I'm taking ECS 34 this quarter.\n", "\n", "--"), std::string("Hello World!-- I'm taking ECS 34 this quarter.--"));
}

TEST(StringUtilsTest, Split){
    EXPECT_EQ(StringUtils::Split("Hello", "l"), (std::vector<std::string>{"He", "", "o"}));
    EXPECT_EQ(StringUtils::Split("Helllo", "l"), (std::vector<std::string>{"He", "", "", "o"}));
    EXPECT_EQ(StringUtils::Split("World", "w"), (std::vector<std::string>{"World"}));
    EXPECT_EQ(StringUtils::Split("Hello World  ", "d"), (std::vector<std::string>{"Hello Worl", "  "}));
    EXPECT_EQ(StringUtils::Split("Banana", "a"), (std::vector<std::string>{"B", "n", "n", ""}));
    EXPECT_EQ(StringUtils::Split("Banana", "an"), (std::vector<std::string>{"B", "", "a"}));
    EXPECT_EQ(StringUtils::Split("Banana", "na"), (std::vector<std::string>{"Ba", "", ""}));
    EXPECT_EQ(StringUtils::Split("Banana", "ana"), (std::vector<std::string>{"B", "na"}));
    EXPECT_EQ(StringUtils::Split("", ","), (std::vector<std::string>{""}));
    EXPECT_EQ(StringUtils::Split("Apple Orange Banana"), (std::vector<std::string>{"Apple", "Orange", "Banana"}));
    EXPECT_EQ(StringUtils::Split(" Apple  Orange  Banana "), (std::vector<std::string>{"Apple", "Orange", "Banana"})); //Change
    EXPECT_EQ(StringUtils::Split("Apple\nOrange\nBanana"), (std::vector<std::string>{"Apple", "Orange", "Banana"})); //Confirmed
    EXPECT_EQ(StringUtils::Split("Apple\n\t Orange \vBanana "), (std::vector<std::string>{"Apple", "Orange", "Banana"}));
    EXPECT_EQ(StringUtils::Split("Banana"), (std::vector<std::string>{"Banana"}));
    EXPECT_EQ(StringUtils::Split(" \n \t "), (std::vector<std::string>{}));
    EXPECT_EQ(StringUtils::Split(""), (std::vector<std::string>{}));

}

TEST(StringUtilsTest, Join){
    EXPECT_EQ(StringUtils::Join(" ", {"Hello", "World"}), std::string("Hello World"));
    EXPECT_EQ(StringUtils::Join("  ", {"Apple", "Orange", "Banana"}), std::string("Apple  Orange  Banana"));
    EXPECT_EQ(StringUtils::Join("", {"Apple", "Orange", "Banana"}), std::string("AppleOrangeBanana"));
    EXPECT_EQ(StringUtils::Join("", {"Coconut"}), std::string("Coconut"));
    EXPECT_EQ(StringUtils::Join("!", {"Coconut"}), std::string("Coconut"));
    EXPECT_EQ(StringUtils::Join("0", {"1", "11", "111", "11", "1"}), std::string("1011011101101"));
    EXPECT_EQ(StringUtils::Join("", {"1", "11", "111", "11", "1"}), std::string("111111111"));
    EXPECT_EQ(StringUtils::Join("0", {}), std::string(""));
    EXPECT_EQ(StringUtils::Join(", ", {"Strawberry", "Blueberry", "Raspberry"}), std::string("Strawberry, Blueberry, Raspberry"));
    EXPECT_EQ(StringUtils::Join("\n", {"Strawberry", "Blueberry", "Raspberry"}), std::string("Strawberry\nBlueberry\nRaspberry"));
    EXPECT_EQ(StringUtils::Join("Fruit", {"Apple", "Orange", "Banana"}), std::string("AppleFruitOrangeFruitBanana"));
}

TEST(StringUtilsTest, ExpandTabs){
    EXPECT_EQ(StringUtils::ExpandTabs("Hello\tWorld", 8), "Hello   World"); //defual (If tab is negative or 0, removes all white spaces)
    EXPECT_EQ(StringUtils::ExpandTabs("Apple\tOrange Banana", 1), "Apple Orange Banana"); 
    EXPECT_EQ(StringUtils::ExpandTabs("Apple\tOrange Banana", 67), "Apple                                                              Orange Banana");
    EXPECT_EQ(StringUtils::ExpandTabs("Apple\tOrange Banana"), "Apple   Orange Banana");
    EXPECT_EQ(StringUtils::ExpandTabs("Apple\tOrange Banana", -1), "AppleOrange Banana");
    EXPECT_EQ(StringUtils::ExpandTabs(""), "");
    EXPECT_EQ(StringUtils::ExpandTabs("", 10), "");
    EXPECT_EQ(StringUtils::ExpandTabs(" ", 10), " ");
    EXPECT_EQ(StringUtils::ExpandTabs(" "), " ");
    EXPECT_EQ(StringUtils::ExpandTabs("\t"), "    ");
    EXPECT_EQ(StringUtils::ExpandTabs("\t", 5), "     ");
    EXPECT_EQ(StringUtils::ExpandTabs("\t", 10), "          ");
    EXPECT_EQ(StringUtils::ExpandTabs("\t\t\t\t\t", 1), "     ");
    EXPECT_EQ(StringUtils::ExpandTabs("Coconut\n\tMango\tRice"), "Coconut\n    Mango   Rice");
    EXPECT_EQ(StringUtils::ExpandTabs("Coconut\n\tMango\tRice", 9), "Coconut\n         Mango    Rice");
    EXPECT_EQ(StringUtils::ExpandTabs("Coconut\n\tMango\tRice", 0), "Coconut\nMangoRice");
}

TEST(StringUtilsTest, EditDistance){
    EXPECT_EQ(StringUtils::EditDistance("Hello", "World"), 4);
    EXPECT_EQ(StringUtils::EditDistance("kitten", "mitten"), 1);
    EXPECT_EQ(StringUtils::EditDistance("apple", "orange"), 5);
    EXPECT_EQ(StringUtils::EditDistance("Apple", "Orange"), 5);
    EXPECT_EQ(StringUtils::EditDistance("Apple", "Orange", true), 5);
    EXPECT_EQ(StringUtils::EditDistance("Apple Orange", "Orange"), 6);
    EXPECT_EQ(StringUtils::EditDistance("Apple Orange Banana", "Orange"), 13);
    EXPECT_EQ(StringUtils::EditDistance("Apple", "apple"), 1);
    EXPECT_EQ(StringUtils::EditDistance("Apple", "apple", true), 0);
    EXPECT_EQ(StringUtils::EditDistance(" ", "", true), 1);
    EXPECT_EQ(StringUtils::EditDistance("\n\t", "", true), 2);
    EXPECT_EQ(StringUtils::EditDistance("", "", true), 0);
}

