#include "gtest/gtest.h"

#include "implementation.h"

using namespace ba_graph;


class CreatingDeletingGraphFixture : public ::testing::Test {
protected:
    Vertex v1, v2;
    Edge e1, e2;

    void TearDown() override {

    }

    void SetUp() override {
        v1 = createV();
        v2 = createV();
        e1 = createE(v1, v2);
        e2 = createE(v1, v1);
    }
};


TEST_F(CreatingDeletingGraphFixture, 00_you_cannot_mess_up_factories) {
    Factory f1;
    Factory f2;
    Graph G1(createG());
    Graph G2(createG(f1));
    ASSERT_DEATH(addV(G2), ".*");
    ASSERT_DEATH(addV(G1,f1), ".*");
    ASSERT_DEATH(addV(G2,f2), ".*");
    ASSERT_FALSE(check(G1, f1));
    ASSERT_TRUE(check(G1));
    ASSERT_TRUE(check(G2, f1));
    ASSERT_FALSE(check(G2, f2));
    ASSERT_FALSE(check(G2));
}

TEST_F(CreatingDeletingGraphFixture, 01_order_size_degree) {
    //We can add vertices and edges into G,
    //if we add a vertex without supplementing one, factory creates a new one for us
    Graph G(createG());
    addV(G, v1);
    addV(G, v2);
    addV(G);
    addE(G, v1, v2);

    //Graph G now.contains three vertices and one edge
    ASSERT_EQ(G.order(),3);
    ASSERT_EQ(G.size(),1);
    //v1 is of degree 1, v2 is of degree 2
    //note that degree.contains nothing to do with a Vertex, it is property of Rotation
    ASSERT_EQ(G.find(RP::RP(v1))->degree(),1);
    ASSERT_EQ(G.find(RP::RP(v2))->degree(),1);
}

TEST_F(CreatingDeletingGraphFixture, 0x_add_loop) {
    Graph G(createG());
    addV(G, Num(1));
    auto [it1, it2]=addE(G, G[1], G[1], G[1].end(), G[1].end());
    assert(it1+1==it2);
    assert(it1->h1()==it2->h2());
}


TEST_F(CreatingDeletingGraphFixture, 02_other_features) {
    Graph G2(createG());
    //The suggested way to organize vertices is using Numbers -> intetegers representing the vertex within the graph
    addV(G2, v1, 1);
    addV(G2, v2, 2);
    Vertex v3=addV(G2, 3).v();
    Number number=addV(G2).n();
    //number of the new vertex is unique
    ASSERT_TRUE(number!=1 && number!=2 && number!=3);
    addE(G2, Location(1, 2));
    addE(G2, Location(2, 3));
    addE(G2, Location(1, 2));
    addE(G2, e1); //this is also an edge between 1 (v1) and 2 (v2)
    addE(G2, Location(3, number));
    //Graph G2 now.contains and edges
    ASSERT_TRUE(G2.order()==4);
    ASSERT_TRUE(G2.size()==5);
    //Numbers are the faster way to find get to the Rotation object (e.g. to get vertex degree)
    ASSERT_TRUE(G2[1].degree()==3);
    ASSERT_TRUE(G2[2].degree()==4);
    ASSERT_TRUE(G2[3].degree()==2);
    ASSERT_TRUE(G2[number].degree()==1);

    auto res21=addE(G2,G2[1],G2[2],G2[1].begin()+3,G2[2].begin());
    auto res22=addE(G2,G2[2],G2[2],G2[2].begin()+3,G2[2].begin()+1);
    ASSERT_TRUE(G2[1].degree()==4);
    ASSERT_TRUE(G2[2].degree()==7);
    ASSERT_TRUE(res21.first==G2[1].begin()+3);
    ASSERT_TRUE(res21.second==G2[2].begin());
    ASSERT_TRUE(res22.first==G2[2].begin()+4);
    ASSERT_TRUE(res22.second==G2[2].begin()+1);
    ASSERT_TRUE((G2[2].begin()+4)->n2()==2);
    ASSERT_TRUE((G2[2].begin()+1)->n2()==2);
    deleteE(G2,Location(1,2,3));
    deleteE(G2,Location(2,2));

    //We can create a seccond graph
    Graph H(createG());
    addMultipleV(H, {v1, v2, v3}, 1);
    addE(H, Location(1, 2));
    addE(H, Loc(2, 3));
    addE(H, e1);
    addE(H, Loc(3, 3));
    //now even when the vertices 1 of G and H are equal
    ASSERT_TRUE(G2[1].v()==H[1].v());
    //if we delete a vertex from F it does not affect H
    deleteV(G2,1);
    ASSERT_TRUE(G2.order()==3);
    ASSERT_TRUE(G2.size()==2);
    ASSERT_TRUE(G2[2].degree()==1);
    ASSERT_TRUE(G2[3].degree()==2);
    ASSERT_TRUE(G2[number].degree()==1);
    ASSERT_TRUE(H.order()==3);
    ASSERT_TRUE(H.size()==4);

    //we can create many vertices at once
    {
        Graph H2(createG());
        int res=addMultipleV(H2, 5).to_int();
        addE(H2, Location(res, res+4));
        ASSERT_TRUE(H2[res].degree()==1);
    }
    {
        Graph H2(createG());
        addMultipleV(H2, 5, 2);
        addE(H2, Location(2, 6));
        ASSERT_TRUE(H2[6].degree()==1);
        int res=addMultipleV(H2, 5).to_int();
        ASSERT_TRUE(res>6);
        addE(H2, Location(res, res+4));
        ASSERT_TRUE(H2[res].degree()==1);
    }
    {
        Graph H2(createG());
        Vertex v1=createV();
        Vertex v2=createV();
        addMultipleV(H2, {v1,v2}, {1, 2});
        addMultipleV(H2, {3,4,5});
        addMultipleV(H2, {createV(), createV()});
        addMultipleE(H2, Locs({{3,3},{5,2}})); //Locations
        addMultipleE(H2, {{v1,v2}, {v1,v1}}); //pairs of vertices
        addMultipleE(H2, {createE(v1,v2),createE(v2,v2)}); //Locations
        ASSERT_TRUE(H2.order()==7);
        ASSERT_TRUE(H2.size()==6);

        for(auto it=H2.begin();it!=H2.end();) {
            if (it->isolated())
                it=deleteV(H2,it);
            else
                it++;
        }
        ASSERT_TRUE(H2.order()==4);
        ASSERT_TRUE(H2.size()==6);

        addMultipleE(H2, Locs({{3,3},{3,1}, {5,3}})); //Directions
        ASSERT_TRUE(H2.order()==4);
        ASSERT_TRUE(H2.size()==9);
        for(auto it=H2[3].begin();it!=H2[3].end();) {
            if (it->is_loop())
                it=deleteE(H2,it);
            else
                it++;
        }
        ASSERT_TRUE(H2.order()==4);
        ASSERT_TRUE(H2.size()==7);
    }








//    To create and modify Vertices, Edges and Graphs a Factory is used. If you use ordinary
//    function cV(), cE() then Factory static_factory is used. If we want to use more than
//    one Factory we can make and use our own.

    Factory myFactory;
    Vertex w1=createV(myFactory);
    Vertex w2=createV(myFactory);
    Edge f1=createE(w1, w2, myFactory);
    Edge f2=createE(w1, w1, myFactory);
    {
        Graph G(createG(myFactory));
        addV(G, w1, myFactory);
        addV(G, w2, myFactory);
        addE(G, f1, myFactory);
        addE(G, f2, myFactory);
        deleteV(G,w1, myFactory);
        ASSERT_TRUE(G.order()==1);
        ASSERT_TRUE(G.size()==0);
        addE(G, *(G.find(w2)),*(G.find(w2)), myFactory);
        ASSERT_TRUE(G.size()==1);
    }
}



