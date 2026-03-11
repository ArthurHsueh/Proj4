#include <gtest/gtest.h>
#include "DijkstraPathRouter.h"

TEST(DijkstraPathRouter, SimpleTest){
    CDijkstraPathRouter PathRouter;

    EXPECT_EQ(PathRouter.VertexCount(),0);
    auto VertexID = PathRouter.AddVertex(std::string("foo"));
    EXPECT_EQ(PathRouter.VertexCount(),1);
    auto VetextTag = std::any_cast<std::string>(PathRouter.GetVertexTag(VertexID));
    EXPECT_EQ(VetextTag,"foo");
}

TEST(DijkstraPathRouter, ShortestPath){
    CDijkstraPathRouter PathRouter;
    /*
                5   
       A ---> B --->C
          4  2|  ___^
              V / 1
              D
    
    */
    auto VertexA = PathRouter.AddVertex(std::string("A"));
    auto VertexB = PathRouter.AddVertex(std::string("B"));
    auto VertexC = PathRouter.AddVertex(std::string("C"));
    auto VertexD = PathRouter.AddVertex(std::string("D"));
    EXPECT_EQ(PathRouter.VertexCount(),4);
    EXPECT_TRUE(PathRouter.AddEdge(VertexA,VertexB,4.0));
    EXPECT_TRUE(PathRouter.AddEdge(VertexB,VertexC,5.0));
    EXPECT_TRUE(PathRouter.AddEdge(VertexB,VertexD,2.0));
    EXPECT_TRUE(PathRouter.AddEdge(VertexD,VertexC,1.0));
    std::vector<CPathRouter::TVertexID> Path;
    EXPECT_EQ(PathRouter.FindShortestPath(VertexA,VertexC,Path), 7.0);
    std::vector<CPathRouter::TVertexID> ExpectedPath{VertexA,VertexB,VertexD,VertexC};
    EXPECT_EQ(Path,ExpectedPath);
}

TEST(DijkstraPathRouter, ShortestPathHardTest){
    CDijkstraPathRouter PathRouter;

    auto A = PathRouter.AddVertex(std::string("A"));
    auto B = PathRouter.AddVertex(std::string("B"));
    auto C = PathRouter.AddVertex(std::string("C"));
    auto D = PathRouter.AddVertex(std::string("D"));
    auto E = PathRouter.AddVertex(std::string("E"));
    auto F = PathRouter.AddVertex(std::string("F"));
    auto G = PathRouter.AddVertex(std::string("G"));
    auto H = PathRouter.AddVertex(std::string("H"));
    EXPECT_EQ(PathRouter.VertexCount(),8);
    EXPECT_TRUE(PathRouter.AddEdge(A,B,2.5));
    EXPECT_TRUE(PathRouter.AddEdge(A,C,5.0));
    EXPECT_TRUE(PathRouter.AddEdge(B,D,2.0));
    EXPECT_TRUE(PathRouter.AddEdge(B,C,3.0));
    EXPECT_TRUE(PathRouter.AddEdge(C,E,2.0));
    EXPECT_TRUE(PathRouter.AddEdge(E,F,2.0));
    EXPECT_TRUE(PathRouter.AddEdge(D,G,1.0));
    EXPECT_TRUE(PathRouter.AddEdge(G,H,1.0));
    EXPECT_TRUE(PathRouter.AddEdge(F,H,10.0, true));
    std::vector<CPathRouter::TVertexID> Path;
    EXPECT_EQ(PathRouter.FindShortestPath(A,H,Path),6.5);
    std::vector<CPathRouter::TVertexID> ExpectedPath{A,B,D,G,H};
    EXPECT_EQ(Path,ExpectedPath);
    EXPECT_EQ(std::any_cast<std::string>(PathRouter.GetVertexTag(0)), "A");
    EXPECT_EQ(std::any_cast<std::string>(PathRouter.GetVertexTag(7)), "H");
    EXPECT_THROW(std::any_cast<std::string>(PathRouter.GetVertexTag(100)), std::bad_any_cast);
}

TEST(DijkstraPathRouter, ShortestPathUnreachableTest){
    CDijkstraPathRouter PathRouter;
    /*
                5   
       A ---> B --->C
          4      ___^
                / 1
              D
    
    */
    auto VertexA = PathRouter.AddVertex(std::string("A"));
    auto VertexB = PathRouter.AddVertex(std::string("B"));
    auto VertexC = PathRouter.AddVertex(std::string("C"));
    auto VertexD = PathRouter.AddVertex(std::string("D"));
    EXPECT_EQ(PathRouter.VertexCount(),4);
    EXPECT_TRUE(PathRouter.AddEdge(VertexA,VertexB,4.0));
    EXPECT_TRUE(PathRouter.AddEdge(VertexB,VertexC,5.0));
    EXPECT_TRUE(PathRouter.AddEdge(VertexD,VertexC,1.0));
    std::vector<CPathRouter::TVertexID> Path;
    EXPECT_EQ(PathRouter.FindShortestPath(VertexA,VertexD,Path), std::numeric_limits<double>::max());
    std::vector<CPathRouter::TVertexID> ExpectedPath{};
    EXPECT_EQ(Path,ExpectedPath);
}

TEST(DijkstraPathRouter, ShortestPathSelfTest){
    CDijkstraPathRouter PathRouter;
    /*  

       A --
       ^   | 9
       | --

    */
    auto VertexA = PathRouter.AddVertex(std::string("A"));
    EXPECT_EQ(PathRouter.VertexCount(),1);
    EXPECT_TRUE(PathRouter.AddEdge(VertexA,VertexA,9.0));
    std::vector<CPathRouter::TVertexID> Path;
    EXPECT_EQ(PathRouter.FindShortestPath(VertexA,VertexA,Path), 0.0);
    std::vector<CPathRouter::TVertexID> ExpectedPath{0};
    EXPECT_EQ(Path,ExpectedPath);
    EXPECT_TRUE(PathRouter.Precompute(std::chrono::steady_clock::now()));
}