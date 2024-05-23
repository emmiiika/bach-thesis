#ifndef BA_GRAPH_SAT_SOLVER_CMSAT_HPP
#define BA_GRAPH_SAT_SOLVER_CMSAT_HPP

// https://github.com/msoos/cryptominisat
#include <cryptominisat5/cryptominisat.h>

#include "cnf.hpp"
#include "cnf_circular_colouring.hpp"
#include "cnf_colouring.hpp"
#include "cnf_factors.hpp"
#include "solver.hpp"

#include <set>
#include <vector>

namespace ba_graph
{

class CMSatSolver : public SatSolver
{
  public:
    CMSatSolver() : SatSolver(INTERNAL_CMSAT, false) {}
};

class CMAllSatSolver : public AllSatSolver
{
  public:
    CMAllSatSolver() : AllSatSolver(INTERNAL_CMSAT, false) {}
};

namespace internal
{

inline std::vector<CMSat::Lit> cnf_clause_to_cmsat_clause(Clause clause)
{
    std::vector<CMSat::Lit> result;
    for (Lit l : clause)
        result.push_back(CMSat::Lit(l.var(), l.neg()));
    return result;
}

inline bool satisfiable_cmsat(const CNF &cnf)
{
    auto varsCount = cnf.first;
    auto clauses = cnf.second;
    CMSat::SATSolver solver;
    solver.set_num_threads(1);
    solver.new_vars(varsCount);
    for (auto &clause : clauses)
        solver.add_clause(internal::cnf_clause_to_cmsat_clause(clause));

    CMSat::lbool result = solver.solve();
    if (result == CMSat::l_Undef)
        throw std::runtime_error("failed to solve a SAT instance");
    return result == CMSat::l_True;
}

// if callback returns false, we stop generating more solutions
// returns true iff the given formula is satisfiable
// works reasonably fast for up to 20 000 solutions with 100 vars,
// then progress slows down significantly
// maxSolutions defaults to -1
template <typename P, typename R, typename T>
bool all_sat_solutions_cmsat(const CNF &cnf,
        bool (*callback)(R &solution, P *param), P *callbackParam,
        R (*transform)(std::vector<bool> &, T *transformationData), T *transformationData,
        int maxSolutions = -1)
{
    auto varsCount = cnf.first;
    auto clauses = cnf.second;
    CMSat::SATSolver solver;
    solver.set_num_threads(1);
    solver.new_vars(varsCount);
    for (auto &clause : clauses)
        solver.add_clause(internal::cnf_clause_to_cmsat_clause(clause));

    int solutionCount = 0;
    while (true) {
        CMSat::lbool r = solver.solve();
        if (r == CMSat::l_Undef)
            throw std::runtime_error("failed to solve a SAT instance");
        if (r == CMSat::l_True)
            ++solutionCount;
        else
            break;

        std::vector<bool> solution; solution.reserve(solver.nVars());
        std::vector<CMSat::Lit> bannedSolution; bannedSolution.reserve(solver.nVars());
        for (uint32_t var = 0; var < solver.nVars(); var++) {
            CMSat::lbool varLValue = solver.get_model()[var];
            if (varLValue != CMSat::l_Undef) {
                bool varValue = (varLValue == CMSat::l_True) ? true : false;
                solution.push_back(varValue);
                bannedSolution.push_back(CMSat::Lit(var, varValue));
            } else {
                throw std::runtime_error("l_Undef appeared, what does it mean?");
            }
        }

        if (callback != NULL) {
            R result = transform(solution, transformationData);
            if (!callback(result, callbackParam))
                break;
        }
        if ((maxSolutions != -1) && (solutionCount >= maxSolutions))
            break;
        solver.add_clause(bannedSolution);
    }
    return solutionCount > 0;
}

} // namespace internal

} // namespace ba_graph

#endif
