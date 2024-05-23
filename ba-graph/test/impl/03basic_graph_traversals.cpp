#include "gtest/gtest.h"

#include "implementation.h"

using namespace ba_graph;

class BasicGraphTraversalsFixture : public ::testing::Test {
protected:
    Vertex v1, v2, v3;
    Edge e;
    Graph G;

    BasicGraphTraversalsFixture(): G(createG()) {}
    virtual ~BasicGraphTraversalsFixture() {}

    void TearDown() override {
    }

    void SetUp() override {
        v1=createV();
        v2=createV();
        v3=createV();
        e=createE(v1,v2);

        G=std::move(createG());

        addV(G, v1, 3);
        addV(G, v2, 2);
        addV(G, v3, 1);
        addE(G, e);
        addE(G, v1, v1);
        addE(G, v2, v3);
    }
};



TEST_F(BasicGraphTraversalsFixture, 01_graph_to_rotation) {
    //we can acces neighbourhoods using numbers
    ASSERT_EQ(v1, G[3].v());
    ASSERT_EQ(v2, G[2].v());
    ASSERT_EQ(v3, G[1].v());

    ASSERT_EQ(3, G[3].n());
    ASSERT_EQ(2, G[2].n());
    ASSERT_EQ(1, G[1].n());

    //if we just know a vertex we can find it -
    //find returns an iterator over Rotations pointing at the place where the vertex is
    ASSERT_EQ(G.find(v1)->v(), v1);
    //if search fails we return .end()
    ASSERT_EQ(G.find(createV()), G.end());

    //if we are not sure about the number we may search, but this is much slower than G[n]
    ASSERT_EQ(G.find(Number(4)), G.end());
}



TEST_F(BasicGraphTraversalsFixture, 02_rotation_to_incidence) {
    ASSERT_EQ(G[3][0].e(), e);
    ASSERT_EQ(G[3][1].v2(), v1);
    ASSERT_EQ(G[3][2].v2(), v1);
    ASSERT_FALSE(G[3][0].is_loop());
    ASSERT_TRUE(G[3][2].is_loop());

    ASSERT_EQ(G[3].find(v1)->v2(), v1);
    ASSERT_EQ(G[3].find(v2)->v2(), v2);
    //if search fails we return .end()
    ASSERT_EQ(G[3].find(createV()), G[3].end());
    ASSERT_EQ(G[1].find(Number(3)), G[1].end());
}


TEST_F(BasicGraphTraversalsFixture, 03a_graph_to_incidence_via_location) {
    Edge e1=G[3][1].e();
    Edge e2=addE(G, v1, v2).e(); //we have a multiedge
    Edge e3=addE(G, v2, v1).e(); //we have a multiedge
    Edge e4=addE(G, v1, v1).e(); //we have a multiloop

    std::vector<Edge> ve1, expected;
    expected=std::vector<Edge>({e, e1, e1, e2, e3, e4, e4});
    for(auto &inc: G[3]) ve1.push_back(inc.e());    
    ASSERT_EQ(ve1, expected);

    ASSERT_EQ(G[Loc(3,2)].v1(), v1);   
    ASSERT_EQ(G[Loc(3,2)].v2(), v2);   
    ASSERT_EQ(G[Loc(3,2)].e(), e);   
    ASSERT_EQ(G[Loc(3,2,0)].e(), e);   
    ASSERT_EQ(G[Loc(3,2,1)].e(), e2);   
    ASSERT_EQ(G[Loc(3,2,2)].e(), e3);   
    ASSERT_EQ(G[Loc(2,3,0)].e(), e);   
    ASSERT_EQ(G[Loc(2,3,1)].e(), e2);   
    ASSERT_EQ(G[Loc(2,3,2)].e(), e3);   

    ASSERT_EQ(G[Loc(3,3,0)].e(), e1); 
    ASSERT_EQ(G[Loc(3,3,1)].e(), e1); 
    ASSERT_EQ(G[Loc(3,3,2)].e(), e4);
    ASSERT_EQ(G[Loc(3,3,3)].e(), e4);

    swap_incidence(G, G[3].begin()+1, G[3].begin()+6); //primary edges are +1/+5 before and +5/+6 after exchange
    swap_incidence(G, G[3].begin()+3, G[3].begin()+4);

    std::vector<Edge> ve2;
    expected=std::vector<Edge>({e, e4, e1, e3, e2, e4, e1});
    for(auto &inc: G[3]) ve2.push_back(inc.e());    
    ASSERT_EQ(ve2, expected);

    ASSERT_EQ(G[Loc(3,2,0)].e(), e);   
    ASSERT_EQ(G[Loc(3,2,1)].e(), e2);   
    ASSERT_EQ(G[Loc(3,2,2)].e(), e3);   

    ASSERT_EQ(G[Loc(3,3)].e(),e4);
    ASSERT_EQ(G[Loc(3,3,1)].e(),e4);
    ASSERT_EQ(G[Loc(3,3,2)].e(),e1);
    ASSERT_EQ(G[Loc(3,3,3)].e(),e1);

    swap_incidence(G, G[2].begin()+2, G[2].begin()+3);

    ASSERT_EQ(G[Loc(3,2,0)].e(), e);   
    ASSERT_EQ(G[Loc(3,2,1)].e(), e3);   
    ASSERT_EQ(G[Loc(3,2,2)].e(), e2);   
}

TEST_F(BasicGraphTraversalsFixture, 03b_graph_to_incidence_via_direction) {
    Edge e1=G[3][1].e();
    Edge e2=addE(G, v1, v2).e(); //we have a multiedge
    Edge e3=addE(G, v2, v1).e(); //we have a multiedge
    Edge e4=addE(G, v1, v1).e(); //we have a multiloop

    ASSERT_EQ(G[Dir(3,2)].e(), e);   
    ASSERT_EQ(G[Dir(3,2,0)].e(), e);   
    ASSERT_EQ(G[Dir(3,2,1)].e(), e2);   
    ASSERT_EQ(G[Dir(2,3,0)].e(), e3);   

    ASSERT_EQ(G[Dir(3,3,0)].e(), e1);
    ASSERT_EQ(G[Dir(3,3,1)].e(), e4);
}

TEST_F(BasicGraphTraversalsFixture, 04_location_pair_test) {
    std::set<LocationPair> s;
    for(auto &inc: G[2]) s.insert(inc.l());
    for(auto &inc: G[3]) s.insert(inc.l());
    ASSERT_EQ(s.size(), 3);
    ASSERT_EQ(s.count(Loc(1, 2)), 1);
    ASSERT_EQ(s.count(Loc(3, 2)), 1);
    ASSERT_EQ(s.count(Loc(2, 3)), 1);
    ASSERT_EQ(s.count(Loc(3, 3)), 1);
    ASSERT_EQ(s.count(Loc(1, 3)), 0);
}


TEST_F(BasicGraphTraversalsFixture, 05_graph_and_rotation_basic_methods) {
    ASSERT_EQ(G.order(), 3);
    ASSERT_EQ(G.size(), 3);

    ASSERT_EQ(G[3].n(), 3);
    ASSERT_EQ(G[3].v(), v1);

    ASSERT_EQ(G[1].degree(), 1);
    ASSERT_EQ(G[2].degree(), 2);
    ASSERT_EQ(G[3].degree(), 3);

    ASSERT_FALSE(G[1].isolated());    
    addV(G, 4);
    ASSERT_TRUE(G[4].isolated());
    addE(G, Loc(4, 4));
    ASSERT_FALSE(G[4].isolated());
   
    ASSERT_EQ(G[3].neighbours(), std::set<Number>({2, 3}));    
}


TEST_F(BasicGraphTraversalsFixture, 06_incidence_basic_methods) {
    ASSERT_EQ(G[3][0].right().h1(), G[3][1].h1());
    ASSERT_EQ(G[3][1].right().h1(), G[3][2].h1());
    ASSERT_EQ(G[3][2].right().h1(), G[3][0].h1());

    ASSERT_EQ(G[3][0].left().h1(), G[3][2].h1());
    ASSERT_EQ(G[3][1].left().h1(), G[3][0].h1());
    ASSERT_EQ(G[3][2].left().h1(), G[3][1].h1());

    ASSERT_EQ(G[3][0].jump().h1(), G[2][0].h1());
    ASSERT_EQ(G[3][1].jump().h1(), G[3][2].h1());
    ASSERT_EQ(G[3][2].jump().h1(), G[3][1].h1());

    ASSERT_EQ(G[3][1].h2(), G[3][2].h1());
    ASSERT_EQ(G[3][1].h1(), G[3][2].h2());

    ASSERT_EQ(G[3][0].e(), e);
    ASSERT_EQ(G[3][0].v1(), v1);
    ASSERT_EQ(G[3][0].v2(), v2);

    ASSERT_EQ(&(G[3][0].r1()), &(G[3]));
    ASSERT_EQ(&(G[3][0].r2()), &(G[2]));

    ASSERT_EQ(G[3][0].n1(), 3);
    ASSERT_EQ(G[3][0].n2(), 2);

    ASSERT_TRUE(G[3][0].is_primary());
    ASSERT_FALSE(G[2][0].is_primary());
    ASSERT_FALSE(G[3][0].is_loop());
    ASSERT_TRUE(G[3][1].is_loop());

    ASSERT_EQ(G[3][0].l(), Loc(3,2));
    ASSERT_EQ(G[3][1].l(), Loc(3,3,0));
    ASSERT_EQ(G[3][2].l(), Loc(3,3,1));

    ASSERT_EQ(G[3][0].d(), Dir(3,2));
    ASSERT_EQ(G[3][1].d(), Dir(3,3,0));
    ASSERT_EQ(G[3][2].d(), Dir(3,3,0));
}


TEST_F(BasicGraphTraversalsFixture, 07_graph_iterators) {
    std::set<Vertex> sv=std::set<Vertex>();

    //we can iterate through the rotations in the old-fasioned way
    for(auto it=G.begin();it!=G.end();it++)
       sv.insert(it->v());
    ASSERT_TRUE(sv.size()==3);
    ASSERT_TRUE(*(sv.find(v1))==v1);
    ASSERT_TRUE(*(sv.find(v2))==v2);
    ASSERT_TRUE(*(sv.find(v3))==v3);

    //Or we can iterate Neighbourhoods using range based loops
    sv=std::set<Vertex>();
    for(const auto &rot : G)
       sv.insert(rot.v());
    ASSERT_TRUE(sv.size()==3);
    ASSERT_TRUE(*(sv.find(v1))==v1);
    ASSERT_TRUE(*(sv.find(v2))==v2);
    ASSERT_TRUE(*(sv.find(v3))==v3);

    //this fails as we are not allowed to make our own Rotations
    //for(auto it : G)
    //we always acces graphs and rotations by refference

    //we may ommit const, The interrface of Rotation does not allow us to do nothing bad anyways
    sv=std::set<Vertex>();
    for(auto &rot : G)
       sv.insert(rot.v());
    ASSERT_TRUE(sv.size()==3);


    //btw this should not fail
    Graph G2(createG());
    sv=std::set<Vertex>();
    for(auto &rot : G2)
       sv.insert(rot.v());
    ASSERT_TRUE(sv.size()==0);
}


TEST_F(BasicGraphTraversalsFixture, 08_incidence_iterators) {
    //To search incidences we use find with predicates
    ASSERT_TRUE(G[3].find(IP::IP(v2))->h1()==e.h1());
    ASSERT_TRUE(G[3].find(IP::IP(v1))->is_loop());
    ASSERT_TRUE(G[3].find(IP::IP(e))->e()==e);
    ASSERT_TRUE(G[3].find(IP::IP(e.h1()))->e()==e);
    //if we want to check if the element exist we have many posibilities
    auto it2=G[3].find(IP::IP(createV()));
    ASSERT_TRUE(it2==G[3].end());  //this is best iw we plat no use it2 eventualy
    ASSERT_TRUE(!G[3].contains(IP::IP(Edge())));
    ASSERT_TRUE(G[3].count(IP::IP(e.h2()))==0);




    //we can iterate through the rotations in the old-fasioned way
    std::set<Vertex> sv=std::set<Vertex>();
    std::set<Edge> se=std::set<Edge>();
    for(auto it=G[3].begin();it!=G[3].end();it++)
       se.insert(it->e());
    ASSERT_TRUE(se.size()==2);
    ASSERT_TRUE(*(se.find(e))==e);

    //Or we can iterate Neighbourhoods using range based loops
    std::set<HalfEdge> sh;
    for(const auto &rotPos : G[3])
       sh.insert(rotPos.h1());
    ASSERT_TRUE(sh.size()==3);

    //this fails as we are not allowed to make our own Incidences
    //for(auto it : G[3])
    //we always acces Graphs, Rotations and Incidences by refference

    //we may ommit const, The interface of Rotation does not allow us to do nothing bad anyways
    sv=std::set<Vertex>();
    for(auto &rotPos : G[3]) {
       ASSERT_TRUE(rotPos.r1().v()==v1);
       sv.insert(rotPos.r2().v());
    }
    ASSERT_TRUE(sv.size()==2);


    //btw this should not fail
    Graph G3(createG());
    addV(G3,1);
    sv=std::set<Vertex>();
    for(auto &rotPos : G3[1])
       sv.insert(rotPos.r2().v());
    ASSERT_TRUE(sv.size()==0);
}
