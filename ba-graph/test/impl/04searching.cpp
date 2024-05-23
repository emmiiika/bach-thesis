#include "gtest/gtest.h"

#include "implementation.h"

using namespace ba_graph;


class SearchingFixture : public ::testing::Test {
protected:
    void TearDown() override {
    }

    void SetUp() override {
    }
};


TEST_F(SearchingFixture, 01_mess) {
    //We have a graph
    Vertex v1=createV();
    Vertex v2=createV();
    Vertex v3=createV();
    Graph G(createG());
    Edge e=createE(v1,v2);
    addV(G, v1, 3);
    addV(G, v2, 2);
    addV(G, v3, 1);
    addE(G, v1, v1);
    addE(G, v2, v3);

    //we have find functions
    ASSERT_TRUE(G.find(RP::RP(v1))->n()==3);
    ASSERT_TRUE(G.find(v2)->n()==2); //we can get rid of the predicate RP:RP
    ASSERT_TRUE(G.find(RP::RP(Vertex()))==G.end());
    ASSERT_TRUE(G.find(RP::RP(Number(3)))->n()==3);
    ASSERT_TRUE(G.find(v1)->n()==3);
    ASSERT_TRUE(G.find(Number(3))->n()==3);
    ASSERT_TRUE(G.find(RP::RP(Number(6)))==G.end());
    ASSERT_TRUE(G.count(RP::RP(v1))==1);
    ASSERT_TRUE(G.count(RP::RP(Vertex()))==0);
    ASSERT_TRUE(G.count(Number(3))==1);
    ASSERT_TRUE(G.count(RP::all())==3);
    ASSERT_TRUE(G.contains(RP::RP(v1)));
    ASSERT_TRUE(!G.contains(RP::RP(Vertex())));
    ASSERT_TRUE(G.contains(Number(3)));
    ASSERT_TRUE(!G.contains(RP::RP(Number(6))));
    ASSERT_TRUE(G.list(Number(3))[0]->n()==3);

    ASSERT_TRUE(G.find(Number(3))->n()==3);
    ASSERT_TRUE(G.find(Number(4))==G.end());
    ASSERT_TRUE(G.contains(Number(3)));
    ASSERT_FALSE(G.contains(Number(4)));
    

    //we can find incidences
    auto [rotation_it, incidence_it]=G.find(RP::RP(v2),IP::IP(v3));
    ASSERT_TRUE(rotation_it->n()==2 && incidence_it->n2()==1);
    ASSERT_TRUE(G.find(RP::RP(Number(6)),IP::IP(v3)).first==G.end());

    ASSERT_TRUE(G.count(RP::RP(v1),IP::IP(v1))==2);
    ASSERT_TRUE(!G.contains(RP::RP(v1),IP::IP(v3)));
    ASSERT_TRUE(G.count(v1,v1)==2);
    ASSERT_TRUE(!G.contains(v1,IP::IP(v3)));
    ASSERT_TRUE(G.count(v1,v1)==2);
    ASSERT_TRUE(!G.contains(v1,v3));
    auto vpi=G.list(v1,v1);
    ASSERT_TRUE(vpi[0]->jump().h1()==vpi[1]->h1());

    ASSERT_TRUE(G.find(Location(3,3,1))->l()==Location(3,3,1));
    ASSERT_TRUE(G.find(Location(3,3,1))->jump().l()==Location(3,3,0));

    ASSERT_TRUE(G.list(RP::all(),IP::all()).size()==4);
    ASSERT_TRUE(G.list(RP::all(),IP::primary()).size()==2);


    //but there are much more ways to search edges
    //we can search a number
    ASSERT_TRUE(G[2].find(IP::IP(Number(1)))!=G[2].end());
    ASSERT_TRUE(G[2].find(Number(1))!=G[2].end()); //we can get rid of IP::OP
    ASSERT_TRUE(G[2].find(Number(0))==G[2].end());
    ASSERT_TRUE(G[2].find(Number(1))->r2().n()==1);
    ASSERT_TRUE(G[1].find(Number(2))->r2().n()==2);
    ASSERT_TRUE(G[1].find(IP::primary() && IP::n2(2))==G[1].end());
    ASSERT_TRUE(G[2].contains(Number(1)));
    ASSERT_TRUE(!G[2].contains(IP::IP(Number(0))));
    ASSERT_TRUE(G[2].count(Number(1))==1);
    ASSERT_TRUE(G[2].count(Number(0))==0);
    ASSERT_TRUE(G[3].count(Number(3))==2);
    auto inc_list=G[3].list(Number(3));
    ASSERT_TRUE(inc_list[0]->jump().h1()==inc_list[1]->h1());


    //we can search a vertex
    ASSERT_TRUE(G[2].find(v3)!=G[2].end());
    ASSERT_TRUE(G[2].find(v2)==G[2].end());
    ASSERT_TRUE(G[2].find(IP::IP(v3))->r2().v()==v3);
    ASSERT_TRUE(G[1].find(v2)->r2().v()==v2);
    ASSERT_TRUE(G[1].find(IP::primary() && IP::v2(v2))==G[1].end());
    ASSERT_TRUE(G[2].contains(IP::IP(G[1].v())));
    ASSERT_TRUE(G[2].count(G[1].v())==1);
    ASSERT_TRUE(G[3].count(G[3].v())==2);
    ASSERT_TRUE(G[3].find(IP::IP(v1) && IP::ith(0))->r2().v()==v1);
    ASSERT_TRUE(G[3].find(IP::v2(v1) && IP::ith(1))->r2().v()==v1);
    ASSERT_TRUE(G[3].find(IP::v2(v1) && IP::ith(0))->h2()==G[3].find(IP::v2(v1) && IP::ith(1))->h1());
    ASSERT_TRUE(G[3].find(IP::v2(v1) && IP::ith(2))==G[3].end());
    inc_list=G[3].list(v1);
    ASSERT_TRUE(inc_list[0]->jump().h1()==inc_list[1]->h1());

    //we can search an edge
    ASSERT_TRUE(G[2].find(IP::IP(Edge()))==G[2].end());
    ASSERT_TRUE(G[2].find(e)==G[2].end());
    Edge e2=G[2].find(Number(1))->e();
    ASSERT_TRUE(G[2].find(IP::IP(e2))!=G[2].end());
    ASSERT_TRUE(G[2].find(e2)->e()==e2);
    ASSERT_TRUE(G[2].contains(Edge())==0);
    ASSERT_TRUE(G[2].count(e2)==1);
    ASSERT_TRUE(G[3].list(G[3].find(Number(3))->e()).size()==2);

    //we can search an halfedge
    ASSERT_TRUE(G[2].find(IP::IP(HalfEdge()))==G[2].end());

    //we can search a Location
    ASSERT_TRUE(G[3].find((Location(3,3,1)))->l()==Location(3,3,1));
    ASSERT_TRUE(G[2].contains(Location(2,1)));
    ASSERT_TRUE(G[2].contains(Location(1,2)));

    //we can search a Direction
    ASSERT_TRUE(G[3].find((Dir(3,3,0)))->d()==Dir(3,3,0));
    ASSERT_TRUE(G[2].contains(IP::IP(Dir(2,1))));
    ASSERT_DEATH(G[2].contains(IP::IP(Dir(1,2))), ".*");

    auto numvec2=G[3].list(IP::IP(Number(3)),IT::v2());
    ASSERT_TRUE(numvec2[0] == v1);
    ASSERT_TRUE(numvec2[1] == v1);

    auto numvec4=G[3].list(Location(3,3,1), IT::v2());
    ASSERT_TRUE(numvec4[0] == v1);
    ASSERT_TRUE(numvec4.size()==1);
    numvec2=G.list(v1,v1,IT::v2());
    ASSERT_TRUE(numvec2[0] == v1 && numvec2.size()==2);
    numvec2=G.list(Number(3),RT::v());
    ASSERT_TRUE(numvec2[0] == v1 && numvec2.size()==1);
    //btw you do not need these functions as much because you can use lambdas
    numvec2=G.list(Number(3),[](RotationIterator it) {return it->v();});
    ASSERT_TRUE(numvec2[0] == v1 && numvec2.size()==1);

    //and we have nice predicates, but we have to help the compiler a bit
    addE(G,Location(2,3));
    addE(G,Location(1,3));
    ASSERT_TRUE(G[3].list(IP::in<Number>({2,3})).size()==3);
    ASSERT_TRUE(G[3].list(IP::in<Vertex>({v1,v2})).size()==3);
    ASSERT_TRUE(G[3].list(IP::primary_in<Number>({1,2,3})).size()==1);


    //to put something as a function it just should have operator()
    //taking the respective Iterator
    class degree_spy {
    public:
        int sum;
        degree_spy(): sum(0) {}
        int operator()(RotationIterator j) {
            sum+=j->degree();
            return 1;
        }
        bool operator()(const Rotation &j) {
            sum+=j.degree();
            return true;
        }
    };
    class incrementator {
    public:
        int sum;
        incrementator(): sum(0) {}
        int operator()(IncidenceIterator j) {
            (void)j;
            sum++;
            return 1;
        }
        bool operator()(const Incidence &j) {
            (void)j;
            sum++;
            return true;
        }
    };

    degree_spy ds;
    G.list(RP::all(), ds);
    ASSERT_TRUE(ds.sum==8);
    //but you stil can use temporaries
    G.list(RP::all(), degree_spy());

    //you can even put lambda there`
    G.list(RP::all(),IP::all(),
            [](IncidenceIterator it) {return it->h1();} );
    //and you can spy also incidences
    incrementator inc;
    G.list(RP::all(), IP::all(), inc);
    ASSERT_TRUE(inc.sum==8);
    //the same works for rotations - and you can use lambdas
    //now you can even use auto .. it cannot be mistaken
//    G[3].list(IP::all(), [](auto it) {return it->h1();} );
    inc.sum=0;
    G[3].list(IP::all(),inc);
    ASSERT_TRUE(inc.sum==4);


    //Predicates can spy, too
    ds.sum=1;
    G.list(ds);
    ASSERT_TRUE(ds.sum==9);

    //and we can sort
    auto sorted_list=G.list(RP::all(),[](RotationIterator r1, RotationIterator r2) {return r1->degree()<r2->degree();});
    ASSERT_TRUE(sorted_list[2]->v()==v1);
    ASSERT_TRUE(sorted_list[1]->degree()==2);
    ASSERT_TRUE(sorted_list[0]->degree()==2);
    //and transform
    auto transformed_list=G.list(RP::all(),
        [](RotationIterator r1, RotationIterator r2) {return r1->degree()<r2->degree();},
        [](RotationIterator r) {return r->degree();});
    ASSERT_TRUE(transformed_list[2]==4);
    ASSERT_TRUE(transformed_list[1]==2);
    ASSERT_TRUE(transformed_list[0]==2);

    //and transform
    auto transformed_list2=G.list(RP::all(), RC::pair(RC::degree(),RC::n()),RT::v());
    ASSERT_TRUE(transformed_list2[2]==v1);
    ASSERT_TRUE(transformed_list2[1]==v2);
    ASSERT_TRUE(transformed_list2[0]==v3);

    auto c=IP::count();
    G[3].list(IP::degree2_is(4) && c);
    ASSERT_TRUE(c.get_count()==2);
    G[3].list(IP::degree2_is(4) && IP::all());
    G[3].list(c && IP::all());
    G[3].list(c && c);
    ASSERT_TRUE(c.get_count()==14);
    G[3].list((IP::degree2_is(4) || IP::degree2_is(2)) && c);
    ASSERT_TRUE(c.get_count()==18);
    G[3].list((IP::degree2_is(4) || c) && c);
    ASSERT_TRUE(c.get_count()==24);
    G[3].list((IP::degree2_is(4) || !c) && c);
    ASSERT_TRUE(c.get_count()==28);

    auto c2=RP::count();
    G.list((!RP::n(3) && c2));
    ASSERT_TRUE(c2.get_count()==2);
    G.list((!RP::n(3) && c2) || c2);
    ASSERT_TRUE(c2.get_count()==5);


    //and transform
    auto transformed_list3=G.list(!RP::n(2), !IP::loop(), IC::pair(IC::degree1(),IC::pair(IC::n1(),IC::n2())), IT::h1());
    ASSERT_TRUE(transformed_list3.size()==4);
    ASSERT_TRUE(transformed_list3[3]==G[3].find(Number(2))->h1());
    ASSERT_TRUE(transformed_list3[2]==G[3].find(Number(1))->h1());
    ASSERT_TRUE(transformed_list3[1]==G[1].find(Number(3))->h1());
    ASSERT_TRUE(transformed_list3[0]==G[1].find(Number(2))->h1());


}
