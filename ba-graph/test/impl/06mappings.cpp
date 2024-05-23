#include "gtest/gtest.h"

#include "implementation.h"

using namespace ba_graph;


class MappingsFixture : public ::testing::Test {
protected:
    Mapping<int> m;
    Mapping<int> m2;

    Vertex v;
    Vertex v2;

    void TearDown() override {
    }

    void SetUp() override {
        m.set(1,2);
        m.set(2,3);
        m.set(4,4);    
        m.set(6,7);

        m2.set(7,8);
        m2.set(0,9);
        m2.set(1,9);
        m2.set(2,10);
        m2.set(3,11);


        v=createV();
        v2=createV();
    }
};



TEST_F(MappingsFixture, 01_mappings_basic) {
    std::vector<Mapping<int>::value_type> s;   
    for(auto &a:m) s.push_back(a);

    ASSERT_EQ(s[0], (Mapping<int>::value_type({1, 2})));
    ASSERT_EQ(s[1], (Mapping<int>::value_type({2, 3})));
    ASSERT_EQ(s.size(), 3);

    ASSERT_EQ(m[5], 5);
    ASSERT_EQ(m.get(5), 5);
  
//    m[6]=7; //error
}

TEST_F(MappingsFixture, 02_mappings_compose) {
    m.compose_with(m2);
    ASSERT_EQ(m[0], 9);
    ASSERT_EQ(m[1], 10);
    ASSERT_EQ(m[2], 11);
    ASSERT_EQ(m[3], 11);
    ASSERT_EQ(m[4], 4);
    ASSERT_EQ(m[6], 8);
    ASSERT_EQ(m[7], 8);
    ASSERT_EQ(m[9], 9);   

    std::vector<Mapping<int>::value_type> s;   
    for(auto &a:m) s.push_back(a);
    ASSERT_EQ(s.size(), 6);
}


TEST_F(MappingsFixture, 03_mappings_exist) {
    Mapping<Id> m3; (void) m3;
    Mapping<Vertex> m4; (void) m4;
    Mapping<Edge> m5; (void) m5;
    Mapping<HalfEdge> m6; (void) m6;
    Mapping<Number> m7; (void) m7;
    Mapping<Location> m8; (void) m8;
    Mapping<LocationPair> m9; (void) m9;
    Mapping<Direction> m10; (void) m10;
}


TEST_F(MappingsFixture, 04_mappings_iterators) {
    auto it=std::begin(m);
    ASSERT_EQ(it->first, 1);
    ++it;
    ASSERT_EQ(it->first, 2);
    --it;
    ASSERT_EQ(it->first, 1);
    ++it;
    ++it;
    ASSERT_EQ(it->second, 7);
    ++it;
    ASSERT_EQ(it, std::end(m));

    auto it2=std::rbegin(m);
    ASSERT_EQ(it2->first, 6);
    ++it2;
    ASSERT_EQ(it2->first, 2);
    --it2;
    ASSERT_EQ(it2->first, 6);
    ++it2;
    ++it2;
    ASSERT_EQ(it2->first, 1);
    ++it2;
    ASSERT_EQ(it2, std::rend(m));
}


TEST_F(MappingsFixture, 05_labelings) {
    VertexLabeling<int> intl(5);
    intl.set(v, 6);
    ASSERT_EQ(intl[v], 6);
    ASSERT_EQ(intl.get(v), 6);
    ASSERT_EQ(intl[v2], 5);
    ASSERT_DEATH(intl.get(Vertex()),".*");

    intl.set(v2, 7);
    intl.set(v, 5);
    ASSERT_EQ(intl[v], 5);
    ASSERT_EQ(intl[v2], 7);

    std::set<Vertex> s;   
    for(auto &a:intl) s.insert(a.first);
    ASSERT_EQ(s.size(), 1);
    ASSERT_EQ(s.count(v2), 1);
}


TEST_F(MappingsFixture, 06_labelings_exist) {
    VertexLabeling<int> m3; (void) m3;
    EdgeLabeling<int> m4; (void) m4;
    HalfEdgeLabeling<int> m5; (void) m5;
    NumberLabeling<int> m6; (void) m6;
    LocationLabeling<int> m7; (void) m7;
    LocationPairLabeling<int> m8; (void) m8;
    DirectionLabeling<int> m9; (void) m9;
}


TEST_F(MappingsFixture, 07_labelings_iterators) {
    NumberLabeling<int> l(5);
    l.set(Number(5), 6);
    l.set(Number(6), 1);
    l.set(Number(4), 5);

    auto it=std::begin(l);
    ASSERT_EQ(it->second, 6);
    ++it;
    ASSERT_EQ(it->first, 6);
    --it;
    ASSERT_EQ(it->first, 5);
    ++it;
    ++it;
    ASSERT_EQ(it, std::end(l));

    auto it2=std::rbegin(l);
    ASSERT_EQ(it2->first, 6);
    ++it2;
    ASSERT_EQ(it2->first, 5);
    --it2;
    ASSERT_EQ(it2->first, 6);
    ++it2;
    ++it2;
    ASSERT_EQ(it2, std::rend(l));
}


TEST_F(MappingsFixture, 08_graph_number_labeling) {
   Graph g(createG());
   addV(g,1);
   addV(g,3);
   addV(g,5);

   NumberLabeling<int> l1(4);
   l1.set(3, 5);
   for(auto &pair : l1) {
       ASSERT_TRUE(pair.first==3);
       ASSERT_TRUE(pair.second==5);
   }
   GraphNumberLabeling<int> l2(5,g);
   l2.set(3,6);
   for(auto &pair : l2) {
       ASSERT_TRUE(pair.first==3);
       ASSERT_TRUE(pair.second==6);
   }

}
