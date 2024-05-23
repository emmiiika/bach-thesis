#include "implementation.h"
#include <structures/trail.hpp>
#include <io/print_nice.hpp>
#include <cassert>

using namespace ba_graph;

int main() {
/*    Graph G1 = createG();
    addMultipleV(G1, {0, 1, 2, 3, 4});
    addMultipleE(G1, std::vector<Location>{{0,1}, {1,2}, {2,3}, {3,1}});

    Trail t(G1[0], {1,2,3,2,3,1,0});
    assert(t.length()==7);

    std::vector<Number> v;
    for(auto it=t.begin(G1[0]);it!=t.end();it++)
        v.push_back(it.r().n());
    assert(v==std::vector<Number>({0,1,2,3,2,3,1,0}));

    Trail t2({G1[0].v(),G1[1].v()}, {G1[0].find(Number(1))->e()});
    assert(t2.begin()->first==G1[0].v());
    assert(t2.begin()->second==G1[0].find(Number(1))->e());
    assert((++(t2.begin()))->first==G1[1].v());
    int count=0;
    for(const auto &p:t2) {count++; (void) p;}
    assert(count==2);

    assert(t2!=t);
    assert(t==Trail(G1[0], {1,2,3,2,3,1,0}));
    assert(t!=Trail(G1[0], {1,2,3,2,3,1,2}));

    assert(!t.is_path());
    assert(t2.is_path());
    assert(t.is_closed());
    assert(!Trail(G1[0], {1,2,3,2,3,1,2}).is_closed());

    Trail t3(G1[2], {1, 3, 2, 3});
    t.replace_subtrail(t.begin()+2,t.begin()+3,t3.begin(), t3.last_it()-2);
    assert(t==Trail(G1[0], {1,2,1,3,2,3,1,0}));
    t.replace_subtrail(t.begin()+2,t.begin()+4,t3);
    assert(t==Trail(G1[0], {1,2,1,3,2,3,2,3,1,0}));

*/
}
