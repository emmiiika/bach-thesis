#include "implementation.h"

#include <io/print_nice.hpp>

#include <graphs.hpp>
#include <sat/solver_cmsat.hpp>
#include <sat/exec_factors.hpp>

#include <cassert>

using namespace ba_graph;

bool cb(std::vector<Edge> &, int *count)
{
    (*count)++;
    return true;
}

int main()
{
    CMSatSolver solver;
    CMAllSatSolver allSolver;

    auto s = std::set({0, 1, 2, 3, 4, 5});
    auto subsets = all_subsets(s, 1);
    assert(subsets.size() == 6);
    for (auto set : subsets)
        assert(set.size() == 1);
    subsets = all_subsets(s, 4);
    assert(subsets.size() == 15);
    for (auto set : subsets)
        assert(set.size() == 4);

    Graph g = std::move(empty_graph(4));
    addE(g, Location(0, 1));
    addE(g, Location(1, 2));
    addE(g, Location(1, 3));
    addE(g, Location(2, 3));
    assert(has_perfect_matching_sat(solver, g));
    int c = 0;
    assert(perfect_matchings_enumerate_sat(allSolver, g, cb, &c));
    assert(c == 1);
    assert(perfect_matchings_list_sat(allSolver, g).size() == 1);
    deleteV(g, 0);
    assert(!has_perfect_matching_sat(solver, g));

    g = std::move(circuit(11));
    assert(!has_perfect_matching_sat(solver, g));
    c = 0;
    assert(!perfect_matchings_enumerate_sat(allSolver, g, cb, &c));
    assert(c == 0);

    g = std::move(circuit(8));
    assert(has_perfect_matching_sat(solver, g));
    c = 0;
    assert(perfect_matchings_enumerate_sat(allSolver, g, cb, &c));
    assert(c == 2);

    g = std::move(complete_graph(4));
    assert(has_perfect_matching_sat(solver, g));
    c = 0;
    assert(perfect_matchings_enumerate_sat(allSolver, g, cb, &c));
    assert(c == 3);

    g = std::move(create_petersen());
    assert(has_perfect_matching_sat(solver, g));
    assert(has_perfect_matching_sat(solver, g, std::vector<Vertex> {g[0].v()}));
    c = 0;
    assert(perfect_matchings_enumerate_sat(allSolver, g, cb, &c));
    assert(c == 6);
    assert(perfect_matchings_list_sat(allSolver, g).size() == 6);
    assert(perfect_matchings_list_sat(allSolver, g, std::vector<Vertex> {g[0].v(), g[1].v()}).size() == 8);
    deleteV(g, 0);
    assert(!has_perfect_matching_sat(solver, g));
    assert(perfect_matchings_list_sat(allSolver, g).size() == 0);

    g = std::move(complete_graph(5));
    assert(!has_kfactor_sat(solver, g, 1));
    assert(has_kfactor_sat(solver, g, 2));
    assert(!has_kfactor_sat(solver, g, 3));
    assert(has_kfactor_sat(solver, g, 4));
    assert(!has_kfactor_sat(solver, g, 5));
    assert(kfactors_list_sat(allSolver, g, 1, static_factory).size() == 0);
    assert(kfactors_list_sat(allSolver, g, 2, static_factory).size() == 12);
    assert(kfactors_list_sat(allSolver, g, 3, static_factory).size() == 0);
    assert(kfactors_list_sat(allSolver, g, 4, static_factory).size() == 1);
    assert(kfactors_list_sat(allSolver, g, 5, static_factory).size() == 0);

    g = std::move(create_petersen());
    assert(has_kfactor_sat(solver, g, 1));
    assert(has_kfactor_sat(solver, g, 2));
    assert(has_kfactor_sat(solver, g, 3));
    assert(!has_kfactor_sat(solver, g, 4));
    assert(kfactors_list_sat(allSolver, g, 1, static_factory).size() == 6);
    assert(kfactors_list_sat(allSolver, g, 1, static_factory, std::vector<Number> {g[0].n(), g[1].n()}).size() == 8);
    assert(kfactors_list_sat(allSolver, g, 1, static_factory, std::vector<Vertex> {g[0].v(), g[1].v()}).size() == 8);
    assert(kfactors_list_sat(allSolver, g, 1, static_factory, std::vector<Number> {g[0].n()}).size() == 6);
    assert(kfactors_list_sat(allSolver, g, 2, static_factory).size() == 6);
    assert(kfactors_list_sat(allSolver, g, 3, static_factory).size() == 1);
    assert(kfactors_list_sat(allSolver, g, 4, static_factory).size() == 0);
    assert(kfactors_list_sat(allSolver, g, 5, static_factory).size() == 0);

    return 0;
}
