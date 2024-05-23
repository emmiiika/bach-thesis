#ifndef BA_GRAPH_SAT_EXEC_SOLVER_HPP
#define BA_GRAPH_SAT_EXEC_SOLVER_HPP

#include <filesystem>

#include "../util/system.hpp"

#include "cnf.hpp"
#include "solver.hpp"


namespace ba_graph
{

namespace internal
{

inline std::vector<bool> identity(std::vector<bool> &r, void *) { return r; }

} // namespace internal

// assumes the solver supports DIMACS input
// assumes the solver output is in accordance with SAT competition format
//     or minisat
inline bool satisfiable(const SatSolver &solver, const CNF &cnf, const std::string &tmp_dir = ".")
{
    if (solver.external) {
        std::string dimacsFile;
        while (1) {
            dimacsFile = tmp_file(tmp_dir).string();
            std::ifstream f(dimacsFile);
            if (!f)
                break; // file does not exist
        }
        std::ofstream fout(dimacsFile);
        if (!fout)
            throw std::runtime_error("cannot write file " + dimacsFile);
        fout << cnf_dimacs(cnf);
        fout.close();

        std::string solveCommand = solver.command + " " + dimacsFile;
        std::string result = internal::execute_command(solveCommand.c_str());
        std::remove(dimacsFile.c_str());
        if (solver.outputType == solver.MINISAT) {
            if (result.find("UNSATISFIABLE\n") != std::string::npos)
                return false;
            else if (result.find("SATISFIABLE\n") != std::string::npos)
                return true;
            else
                throw std::runtime_error("SAT solver failure");
        } else if (solver.outputType == solver.SAT_COMPETITION) {
            if (result.find("s UNSATISFIABLE\n") != std::string::npos)
                return false;
            else if (result.find("s SATISFIABLE\n") != std::string::npos)
                return true;
            else
                throw std::runtime_error("SAT solver failure");
        } else {
            throw std::logic_error("unsupported external solver type");
        }
    } else if (solver.outputType == solver.INTERNAL_CMSAT) {
#ifdef BA_GRAPH_SAT_SOLVER_CMSAT_HPP
        return internal::satisfiable_cmsat(cnf);
#else
        throw std::logic_error("compiled without support for solver.INTERNAL_CMSAT");
#endif
    }
    throw std::logic_error("unsupported solver type");
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
template <typename P, typename R, typename T>
bool all_sat_solutions(const AllSatSolver &solver, const CNF &cnf,
        bool (*callback)(R &solution, P *param), P *callbackParam,
        R (*transform)(std::vector<bool> &, T *transformationData), T *transformationData,
        const std::string &tmp_dir = ".")
{
    if (solver.outputType == solver.BDD_MINISAT) {
#ifdef BA_GRAPH_SAT_SOLVER_BDDMSAT_HPP
        return internal::all_sat_solutions_bddmsat(solver, cnf, callback, callbackParam,
                transform, transformationData, tmp_dir);
#else
        throw std::logic_error("compiled without support for solver.BDD_MINISAT");
#endif
    } else if (solver.outputType == solver.INTERNAL_CMSAT) {
#ifdef BA_GRAPH_SAT_SOLVER_CMSAT_HPP
        return internal::all_sat_solutions_cmsat(cnf, callback, callbackParam, transform, transformationData);
#else
        throw std::logic_error("compiled without support for solver.INTERNAL_CMSAT");
#endif
    } else
        throw std::logic_error("unsupported allsat solver type");
}
#pragma GCC diagnostic pop

template <typename P>
bool all_sat_solutions(const AllSatSolver &solver, const CNF &cnf,
        bool (*callback)(std::vector<bool> &solution, P *param), P *callbackParam,
        const std::string &tmp_dir = ".")
{
    return all_sat_solutions(solver, cnf, callback, callbackParam,
            internal::identity, (void *)NULL, tmp_dir);
}

}  // namespace end

#endif

