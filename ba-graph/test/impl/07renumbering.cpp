#include "gtest/gtest.h"

#include "implementation.h"
#include <utility>

using namespace ba_graph;


class RenumberingFixture : public ::testing::Test {
protected:
    Graph g;
    Edge e1, e2;

    RenumberingFixture(): g(createG()) {}
    virtual ~RenumberingFixture() {}

    void TearDown() override {
    }

    void SetUp() override {
        g=std::move(createG());
        addV(g,1); 
        addV(g,2); 
        addV(g,4);
        e1=addE(g,Location(1,1)).e();
        e2=addE(g,Location(1,2)).e();
    }
};



TEST_F(RenumberingFixture, 01_change_vertex_number) {
    change_vertex_number(g,1,3);
    ASSERT_TRUE(!g.contains(Number(1)));
    ASSERT_EQ(g[2].count(Number(3)), 1);
    ASSERT_EQ(g[3].count(Number(3)), 2);
    ASSERT_EQ(g[3].count(Number(2)), 1);

    auto ii=g[2].find(e2)->r2().find(IP::primary() && IP::e(e1));
    ASSERT_EQ(ii->h1(), e1.h1());
    ASSERT_EQ(ii->jump().h1(), e1.h2());
    ASSERT_EQ(ii->r2().find(e2)->r2().n(), 2);
}

TEST_F(RenumberingFixture, 02_swap_vertices) {
    swap_vertices(g,4,2);
    ASSERT_EQ(g[2].count(Number(1)), 0);
    ASSERT_EQ(g[4].count(Number(1)), 1);
    ASSERT_EQ(g[1].count(Number(4)), 1);
    ASSERT_EQ(g[1].count(Number(2)), 0);
}

TEST_F(RenumberingFixture, 03_renumber1) {
    Mapping<Number> map;
    map.set(2,3);
    map.set(1,5);
    renumber_vertices(g,map);
    ASSERT_FALSE(g.contains(Number(1)));
    ASSERT_FALSE(g.contains(Number(2)));
    ASSERT_TRUE(g.contains(Number(3)));
    ASSERT_TRUE(g.contains(Number(4)));
    ASSERT_TRUE(g.contains(Number(5)));
}

TEST_F(RenumberingFixture, 03_renumber2) {
    Mapping<Number> map;
    map.set(1,4);
    map.set(4,5);
    renumber_vertices(g,map);
    ASSERT_FALSE(g.contains(Number(1)));
    ASSERT_TRUE(g.contains(Number(2)));
    ASSERT_TRUE(g.contains(Number(4)));
    ASSERT_TRUE(g.contains(Number(5)));
    ASSERT_EQ(g[4].degree(), 3);
    ASSERT_EQ(g[5].degree(), 0);
}

TEST_F(RenumberingFixture, 04_renumber_consecutive) {
    renumber_consecutive(g);
    ASSERT_EQ(g[0].count(Number(2)), 0);
    ASSERT_EQ(g[1].count(Number(2)), 0);
    ASSERT_EQ(g[0].count(Number(0)), 2);
    ASSERT_EQ(g[1].count(Number(0)), 1);
}

TEST_F(RenumberingFixture, 05_swap_move_incidence) {
    {
        Graph g(createG());
        addV(g,1); addV(g,2); addV(g,4);
        Edge e1=addE(g,Location(1,1)).e();
        Edge e2=addE(g,Location(1,2)).e();
        swap_incidence(g,g[1].begin(),g[1].find(e2.h1()));
        swap_incidence(g,g[1].begin()+1,g[1].find(e1.h1()));
        ASSERT_TRUE(g[1].begin()->h1()==e2.h1());
        ASSERT_TRUE((g[1].begin()+1)->h1()==e1.h1());
        ASSERT_TRUE((g[1].end()-1)->h1()==e1.h2());
        move_incidence(g,g[1].end()-1,g[1].begin());
        ASSERT_TRUE(g[1][0].h1()==e1.h2());
        ASSERT_TRUE(g[1][1].h1()==e2.h1());
        ASSERT_TRUE(g[1][2].h1()==e1.h1());
    }
}
