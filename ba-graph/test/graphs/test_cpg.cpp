#include "gtest/gtest.h"

#include "implementation.h"

#include <io/print_nice.hpp>
#include <graphs/cpg.hpp>
#include <structures/permutation.hpp>

//#include <cassert>

using namespace ba_graph;

TEST(CPGtest, CreationTest) {
    permutation<5> s = {0, 2, 4, 3, 1};
    permutation<5> p;

    Graph gph = cycle_permutation_graph(p);
    ASSERT_EQ(gph.order(), 10);
    for (auto &r : gph) {
        ASSERT_EQ(r.degree(), 3);
    }

    // std::cerr << "A prism:" << std::endl << gph << std::endl;


    Graph gph1 = cycle_permutation_graph(s);
    ASSERT_EQ(gph.order(), 10);
    for (auto &r : gph) {
        ASSERT_EQ(r.degree(), 3);
    }

    // std::cerr << "A graph:" << std::endl << gph1 << std::endl;

    permutation<5> t = {1, 0, 4, 3, 2};
    EXPECT_DEATH({
                     auto gph2 = cycle_permutation_graph(t);
                 }, "Assertion *");
}
