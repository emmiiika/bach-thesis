#ifndef BA_GRAPH_UTIL_MATH_HPP
#define BA_GRAPH_UTIL_MATH_HPP

#include <cassert>
#include <set>
#include <vector>

namespace ba_graph {

inline int power(int a, int n)
{
#ifdef BA_GRAPH_DEBUG
    assert (n >= 0);
#endif
    switch (n) {
        case 0: return 1;
        case 1: return a;
        case 2: return a * a;
        case 3: return a * a * a;
    }
    int r = power(a, n/2);
    if (n % 2 == 0)
        return r * r;
    else
        return a * r * r;
}

// generate k-element subsets of a given set s
template <typename T>
std::set<std::set<T>> all_subsets(std::set<T> s, int k)
{
    if (k < 0 || unsigned(k) > s.size())
        return {};
    if (k == 0)
        return {{}};
    if (unsigned(k) == s.size())
        return std::set({s});
    auto smaller = all_subsets(s, k - 1);
    std::set<std::set<T>> result;
    for (auto ss : smaller) {
        for (auto x : s) {
            if (ss.count(x) > 0) continue;
            auto newS = ss;
            newS.insert(x);
            result.insert(newS);
        }
    }
    return result;
}

// generate variations of length k from elements 0, 1, ... n-1 (repetitions allowed)
inline std::vector<std::vector<int>> all_repetitive_variations(int n, int k)
{
    if (k < 0 || n < 0)
        return {};
    if (k == 0)
        return {{}};
    if (k > 0 && n == 0)
        return {};
    // now n > 0, k > 0
    auto shorter = all_repetitive_variations(n, k - 1);
    std::vector<std::vector<int>> result;
    for (auto sv : shorter) {
        for (int x = 0; x < n; ++x) {
            auto v = sv;
            v.push_back(x);
            result.push_back(v);
        }
    }
    return result;
}

} // namespace ba_graph

#endif
