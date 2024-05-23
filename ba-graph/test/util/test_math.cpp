#include <util/math.hpp>

#include <cassert>

using namespace ba_graph;

int main() {
    assert(power(2, 0) == 1);
    assert(power(2, 1) == 2);
    assert(power(3, 5) == 243);
    assert(power(5, 4) == 625);

    auto s = std::set({0, 1, 2, 3, 4, 5});
    auto subsets = all_subsets(s, 1);
    assert(subsets.size() == 6);
    for (auto set : subsets)
        assert(set.size() == 1);
    subsets = all_subsets(s, 4);
    assert(subsets.size() == 15);
    for (auto set : subsets)
        assert(set.size() == 4);

    assert(all_repetitive_variations(10, 1).size() == 10);
    assert(all_repetitive_variations(10, 0).size() == 1);
    assert(all_repetitive_variations(3, 3).size() == 27);
    assert(all_repetitive_variations(5, 4).size() == 625);

    return 0;
}
