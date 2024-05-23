#include "gtest/gtest.h"

#include "implementation.h"

using namespace ba_graph;


class GraphConstructingFunctionsFixture : public ::testing::Test {
protected:
    void TearDown() override {
    }

    void SetUp() override {
    }
};


//Graph.contains no copy constructor,, so it cannot be copied, but it.contains a move constructor.
//So if tou create an auxiliary graph that would be deleted anyway
Graph create1(Factory &f=static_factory) {
    Graph G(createG(f));
    addV(G,f);
    return G;
}

//you can also create a function that modifies only a newly created graph
Graph create2(Graph &&G, Factory &f=static_factory) {
    addV(G,f);
    return std::move(G);
}

/* This fails
Graph create3(Graph &&G, Factory &f=static_factory) {
    return G;
}
*/



TEST_F(GraphConstructingFunctionsFixture, 01move_semantics_test) {
    Graph G(createG());
    ASSERT_TRUE(G.order()==0);

    Graph H1(create1()); //move semantics used
    ASSERT_TRUE(H1.order()==1);
    Graph H2=create1(); //move semantics used
    ASSERT_TRUE(H2.order()==1);

//    Graph H3=H2; //but this fails - no copying allowed, use library function,
       //where you have to explicitly state how you want to copy

    Graph H4=create2(create1());
    ASSERT_TRUE(H4.order()==2);
//    Graph H5=create2(H2); this fails, no copying allowed
    Graph H6=create2(createG());
    ASSERT_TRUE(H6.order()==1);

// we can swap stuff
    std::swap(H4,H6);
    ASSERT_TRUE(H4.order()==1);
    ASSERT_TRUE(H6.order()==2);

}
