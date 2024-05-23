#include "implementation.h"

#include <io/oldBA.hpp>

#include <cassert>
#include <cstdio>

using namespace ba_graph;

int main()
{
    std::string file1 = "test_oldBA_1.txt";
    std::string file1_contents = R"(3

1
2
1 1 1
0 0 0

2
4
1 2 3
0 2 3
0 1 3
0 1 2

3
5
1 2 3 4 4
0
0
0
0 0
)";
    std::ofstream f1out(file1); f1out << file1_contents; f1out.close();

    auto vg=read_oldBA_file(file1);
    assert(vg.size() == 3);
    assert(vg[0].order()==2);
    assert(vg[0].size()==3);
    assert(vg[0][0].count(Number(1))==3);
    assert(vg[1].order()==4);
    assert(vg[1].size()==6);
    assert(vg[1][0].count(Number(1))==1);
    assert(vg[2].order()==5);
    assert(vg[2].size()==5);

    std::stringstream s;
    s << write_oldBA(vg[1]);
    auto g1 = read_oldBA_from_stream(s, true);
    assert(vg[1].order()==4);
    assert(vg[1].size()==6);
    assert(vg[1][0].count(Number(1))==1);

    std::string file2 = "test_oldBA_2.txt";
    write_oldBA_file(file2, vg.begin(), vg.end(), vg.size());
    std::ifstream f1(file1, std::ifstream::ate | std::ifstream::binary);
    std::ifstream f2(file2, std::ifstream::ate | std::ifstream::binary);
    assert(f1.tellg() == f2.tellg());
    remove(file2.c_str());
    remove(file1.c_str());
}
