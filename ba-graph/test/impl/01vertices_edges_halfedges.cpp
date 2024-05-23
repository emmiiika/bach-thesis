#include "gtest/gtest.h"

#include "implementation.h"
#include <set>
#include <unordered_set>

using namespace ba_graph;


class VerticesEdgesHalfedgesFixture : public ::testing::Test {
protected:
    Vertex v1, v2;
    Edge e1, e2;
    HalfEdge h11, h12, h21, h22;

    void TearDown() override {

    }

    void SetUp() override {
        v1 = createV();
        v2 = createV();
        e1 = createE(v1, v2);
        e2 = createE(v1, v1);
        h11 = e1.h1();
        h12 = e1.h2();
        h21 = e2.h1();
        h22 = e2.h2();
    }
};


/*************
Vertex
*************/


TEST_F(VerticesEdgesHalfedgesFixture, 01a_vertex_id_and_null) {
    ASSERT_NE(v1.id(), v2.id());
    ASSERT_NE(Vertex().id(), v2.id());
    ASSERT_EQ(v1.id(), v1.id());
    ASSERT_NE(v1.to_int(), v2.to_int());

    ASSERT_FALSE(v1.is_null());
    ASSERT_TRUE(Vertex().is_null());
}


TEST_F(VerticesEdgesHalfedgesFixture, 01b_vertex_compare) {
    ASSERT_NE(v1, v2);
    ASSERT_EQ(v1, v1);
    ASSERT_NE(v1, v2);
    ASSERT_NE(Vertex(), v1);

    Vertex v1b = createV();
    ASSERT_NE(v1, v1b);

    // v1<v2; //results unspecified, but deterministic, You can e.g. use set<>
    ASSERT_TRUE((v1 < v2) or (v2 < v1));
    // this definitely cannot be true
    ASSERT_FALSE(v1 < v1);
}


/*************
Edge
*************/

TEST_F(VerticesEdgesHalfedgesFixture, 02a_edge_id_and_null) {
    ASSERT_NE(e1.id(), e2.id());
    ASSERT_NE(Edge().id(), e2.id());
    ASSERT_EQ(e1.id(), e1.id());
    ASSERT_NE(e1.to_int(), e2.to_int());

    ASSERT_FALSE(e1.is_null());
    ASSERT_TRUE(Edge().is_null());
}


TEST_F(VerticesEdgesHalfedgesFixture, 02b_edge_compare) {
    ASSERT_NE(e1.id(), v2.id());
    ASSERT_NE(e1.to_int(), e2.to_int());

    ASSERT_EQ(e2, e2);
    ASSERT_TRUE((e1 < e2) or (e2 < e1));
    ASSERT_FALSE(e2 < e2);

    Edge e1b = createE(v1, v2);
    ASSERT_NE(e1, e1b); // they are parallel, not identic
    ASSERT_NE(e1.to_int(), e1b.to_int());
}


TEST_F(VerticesEdgesHalfedgesFixture, 02c_edge_endvertices) {
    ASSERT_FALSE(e1.is_loop());
    ASSERT_TRUE(e2.is_loop());

    ASSERT_EQ(v1, e1.v1());
    ASSERT_EQ(v2, e1.v2());
    ASSERT_EQ(std::pair(v1, v2), e1.v12());
    ASSERT_EQ(v1, e2.v1());
    ASSERT_EQ(v1, e2.v2());
    ASSERT_EQ(std::pair(v1, v1), e2.v12());

    ASSERT_EQ(v1, e1.other_v(v2));
    ASSERT_EQ(v2, e1.other_v(v1));
    ASSERT_EQ(v1, e2.other_v(v1));

    ASSERT_TRUE(e1.has_v(v1));
    ASSERT_TRUE(e1.has_v(v2));
    ASSERT_TRUE(e2.has_v(v1));
    ASSERT_FALSE(e2.has_v(v2));
    ASSERT_FALSE(Edge().has_v(v1));
}


/*************
Halfedge
*************/

TEST_F(VerticesEdgesHalfedgesFixture, 03a_halfedge_id_and_null) {   
    ASSERT_FALSE(h11.is_null());
    ASSERT_TRUE(HalfEdge().is_null());
}


TEST_F(VerticesEdgesHalfedgesFixture, 03b_halfedge_compare) {
    ASSERT_NE(h11, h21);
    ASSERT_NE(HalfEdge(), h11);
    ASSERT_EQ(h11, h11);
    ASSERT_EQ(h21, h21);
    ASSERT_NE(h11, h12);
    ASSERT_NE(h21, h22);

    ASSERT_TRUE((h11 < h12) or (h12 < h11));
    ASSERT_FALSE(h11 < h11);
    ASSERT_FALSE(h21 < h21);

    Edge e1b = createE(v1, v2);
    ASSERT_NE(h11, e1b.h1()); // they are parallel, not identic
}


TEST_F(VerticesEdgesHalfedgesFixture, 03c_halfedge_endvertices) {
    ASSERT_FALSE(h11.is_loop());
    ASSERT_TRUE(h21.is_loop());
    ASSERT_TRUE(h22.is_loop());

    ASSERT_EQ(v1, h11.v1());
    ASSERT_EQ(v2, h11.v2());
    ASSERT_EQ(v2, h12.v1());
    ASSERT_EQ(v1, h12.v2());
    ASSERT_EQ(std::pair(v1, v2), h11.v12());
    ASSERT_EQ(v2, h11.v2());
    ASSERT_EQ(v1, h21.v2());
    ASSERT_EQ(std::pair(v1, v1), h21.v12());
}

TEST_F(VerticesEdgesHalfedgesFixture, 03c_halfedge_halfedges) {
    ASSERT_EQ(h12, h11.h2());
    ASSERT_EQ(h11, h12.h2());
    ASSERT_EQ(h22, h21.h2());
    ASSERT_EQ(h21, h22.h2());
}

/*********************
Edges and Halfedges related stuff
*********************/

TEST_F(VerticesEdgesHalfedgesFixture, 04_edges_halfedges) {
    ASSERT_EQ(h11, e1.h1());
    ASSERT_EQ(h12, e1.h2());
    ASSERT_EQ(h21, e2.h1());
    ASSERT_EQ(h22, e2.h2());
    // parent edge
    ASSERT_EQ(e1, h11.e());
    ASSERT_EQ(e1, h12.e());
    ASSERT_EQ(e2, h21.e());
    ASSERT_EQ(e2, h22.e());
    // dart-reversing involution
    ASSERT_EQ(h11, h12.h2());
    ASSERT_EQ(h12, h11.h2());
    ASSERT_EQ(h21, h22.h2());
    ASSERT_EQ(h22, h21.h2());
}

TEST_F(VerticesEdgesHalfedgesFixture, 05_pairs) {
    //edges and halfedges return pairs, there are multiple ways to deal with this
    //.first, .second (use this if you want only one value)
    ASSERT_TRUE(e1.v12().first == v1 and e1.v12().second == v2);
    //get the pair and do whatever you want
    auto p = e1.v12();
    ASSERT_EQ(p, (std::pair<Vertex, Vertex>(v1, v2)));
    //structured binding to new variables
    auto[v1x, v2x] = e1.v12();
    ASSERT_TRUE(v1x == v1 and v2x == v2);

    v1x = Vertex();
    //std::tie and std::ignore
    std::tie(v1x, std::ignore) = e1.v12();
    ASSERT_EQ(v1x, v1);
    //get
    ASSERT_EQ(v1, std::get<0>(e1.v12()));
}


/*************
Number
*************/

TEST_F(VerticesEdgesHalfedgesFixture, 06_numbers) {
    ASSERT_TRUE(Number().is_null());
    ASSERT_FALSE(Number(0).is_null());
    ASSERT_FALSE(Number(1).is_null());

    ASSERT_EQ(Number(1), Number(1));
    ASSERT_NE(Number(0), Number());
    ASSERT_NE(Number(), Number(1));
    ASSERT_NE(Number(0), Number(1));

    ASSERT_EQ(Number(5)+3, Number(8));
    ASSERT_EQ(Number(5)-3, Number(2));
}


/*************
Location
*************/
TEST_F(VerticesEdgesHalfedgesFixture, 07a_locations) {
    ASSERT_TRUE(Location().is_null());
    ASSERT_FALSE(Location(2, 1).is_null());

    ASSERT_TRUE(Location(2, 1)!=Location(1, 2));
    ASSERT_TRUE(Location(2, 1)==Location(2, 1, 0));
    ASSERT_TRUE(Location(2, 1)!=Location(2, 1, 1));
    ASSERT_TRUE(Location(2, 1, 2)!=Location(2, 1, 1));
    ASSERT_TRUE(Location(2, 1).reverse()==Location(1, 2));
    ASSERT_TRUE(Location(2, 1, 1).reverse()==Location(1, 2, 1));
    ASSERT_TRUE(Location(2, 1, 1).reverse()!=Location(1, 2));
    ASSERT_TRUE(Location(2, 2, 1).reverse()==Location(2, 2));
    ASSERT_TRUE(Location(2, 2, 4).reverse()==Location(2, 2, 5));

    ASSERT_TRUE(Location(2, 1) < Location(1, 2) or Location(1, 2) < Location(2, 1));
    ASSERT_FALSE(Location(2, 1) < Location(2, 1, 0));

    ASSERT_TRUE(Location(2, 1).n1()==2);
    ASSERT_TRUE(Location(2, 1).n2()==1);
    ASSERT_TRUE(Location(2, 1).index()==0);
}

/*************
LocationPair
*************/
TEST_F(VerticesEdgesHalfedgesFixture, 07b_location_pairs) {
    ASSERT_TRUE(LocationPair().is_null());
    ASSERT_FALSE(LocationPair(2, 1).is_null());

    ASSERT_TRUE(LocationPair(2, 1)==LocationPair(1, 2));
    ASSERT_TRUE(LocationPair(2, 1)==LocationPair(2, 1, 0));
    ASSERT_TRUE(LocationPair(2, 1)!=LocationPair(2, 1, 1));
    ASSERT_TRUE(LocationPair(2, 1, 2)!=LocationPair(2, 1, 1));
    ASSERT_TRUE(LocationPair(2, 2, 2)==LocationPair(2, 2, 3));
    ASSERT_TRUE(LocationPair(2, 2, 3)!=LocationPair(2, 2, 4));
}

/*************
Direction
*************/

TEST_F(VerticesEdgesHalfedgesFixture, 08_directions) {
    ASSERT_TRUE(Direction().is_null());
    ASSERT_FALSE(Direction(2, 1).is_null());

    ASSERT_TRUE(Direction(2, 1)!=Direction(1, 2));
    ASSERT_TRUE(Direction(2, 1)==Direction(2, 1, 0));
    ASSERT_TRUE(Direction(2, 1)!=Direction(2, 1, 1));
    ASSERT_TRUE(Direction(2, 1, 2)!=Direction(2, 1, 1));

    ASSERT_TRUE(Direction(2, 1).n1()==2);
    ASSERT_TRUE(Direction(2, 1).n2()==1);
    ASSERT_TRUE(Direction(2, 1).index()==0);

}


/*************
Hashes
*************/

TEST_F(VerticesEdgesHalfedgesFixture, 09_hashes) {
    std::unordered_set<Vertex> uv;
    std::set<Vertex> ov;

    std::unordered_set<Edge> ue;
    std::set<Edge> oe;

    std::unordered_set<HalfEdge> uh;
    std::set<HalfEdge> oh;

    std::unordered_set<Location> ul;
    std::set<Location> ol;

    std::unordered_set<LocationPair> ulp;
    std::set<LocationPair> olp;
    ASSERT_EQ(std::hash<LocationPair>()(Location(1,2)), std::hash<LocationPair>()(Location(1,2,0)));
    ASSERT_EQ(std::hash<LocationPair>()(Location(1,2)), std::hash<LocationPair>()(Location(2,1,0)));
    ASSERT_EQ(std::hash<LocationPair>()(Location(2,1,2)), std::hash<LocationPair>()(Location(1,2,2)));
    ASSERT_EQ(std::hash<LocationPair>()(Location(2,2,2)), std::hash<LocationPair>()(Location(2,2,3)));

    std::unordered_set<Direction> ud;
    std::set<Direction> od;
}

