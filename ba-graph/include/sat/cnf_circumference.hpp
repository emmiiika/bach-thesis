#ifndef BA_GRAPH_SAT_CNF_CIRCUMFERENCE_HPP
#define BA_GRAPH_SAT_CNF_CIRCUMFERENCE_HPP

#include "cnf.hpp"

#include <cassert>
#include <unordered_map>
#include <vector>

namespace ba_graph
{

// check if G has a cycle of length k >= 3
// if w != - 1 is given, the cycle has to containing w
inline CNF cnf_has_circuit(const Graph &G, int k, Number w=-1)
{
#ifdef BA_GRAPH_DEBUG
    assert(k >= 3);
    assert(w == -1 || G.contains(w));
#endif
    if (k > G.order()) return cnf_unsatisfiable;
    // variable x_{v, i} means that v is i-th vertex in the cycle
    // represented by vars[v][i]; i \in {0, 1, ..., k-1}
    std::unordered_map<Number, std::vector<int>> vars;
    int nextVar = 0;
    for (auto &r : G) {
        std::vector<int> v(k);
        for (int i = 0; i < k; ++i)
            v[i] = nextVar++;
        vars[r.n()] = v;
    }

    std::vector<std::vector<Lit>> result;
    result.reserve(G.order() * k * std::max(G.order(), k));
    // the cycle is a sequence of vertices
    // (1) every position of the sequence is used
    for (int i = 0; i < k; ++i) {
        if (w != -1 && i == 0) {
            result.push_back({Lit(vars[w][0], false)});
        } else {
            std::vector<Lit> clause;
            clause.reserve(k);
            for (auto &r : G)
                clause.push_back(Lit(vars[r.n()][i], false));
            result.push_back(clause);
        }
    }
    // (2) no position of the sequence is used twice
    for (int i = 0; i < k; ++i) {
        for (auto &u : G) {
            for (auto &v : G) {
                if (v.n() <= u.n()) continue;
                result.push_back({Lit(vars[u.n()][i], true), Lit(vars[v.n()][i], true)});
            }
        }
    }
    // (3) any vertex is used at most once
    for (int i = 0; i < k; ++i) {
        for (int j = i + 1; j < k; ++j) {
            for (auto &v : G) {
                result.push_back({Lit(vars[v.n()][i], true), Lit(vars[v.n()][j], true)});
            }
        }
    }
    // (4) there is no pair of consecutive vertices without an edge
    for (int i = 0; i < k; ++i) {
        for (auto &u : G) {
            std::vector<Lit> clause;
            clause.reserve(u.degree() + 1);
            clause.push_back(Lit(vars[u.n()][i], true));
            for (auto &inc : u)
                clause.push_back(Lit(vars[inc.n2()][(i+1) % k], false));
            result.push_back(clause);
        }
    }
    return std::pair(G.order() * k, result);
}

} // namespace ba_graph

#endif
