#ifndef BA_GRAPH_SAT_SOLVER_BDDMSAT_HPP
#define BA_GRAPH_SAT_SOLVER_BDDMSAT_HPP

#include "cnf.hpp"
#include "solver.hpp"
#include "../util/system.hpp"

#include <filesystem>
#include <fstream>
#include <vector>

// http://www.sd.is.uec.ac.jp/toda/code/cnf2obdd.html

namespace ba_graph
{

namespace internal
{

template <typename P, typename R, typename T>
bool all_sat_solutions_bddmsat(AllSatSolver bddSolver, const CNF &cnf,
        bool (*callback)(R &solution, P *param), P *callbackParam,
        R (*transform)(std::vector<bool> &, T *transformationData), T *transformationData,
        const std::string &tmp_dir = ".")
{
    std::string dimacsFile;
    while (1) {
        dimacsFile = tmp_file(tmp_dir).string();
        std::ifstream f(dimacsFile);
        if (!f)
            break; // file does not exist
    }
    std::filesystem::path tmpFileDimacsPath(tmp_dir);
    tmpFileDimacsPath /= dimacsFile;
    const std::string tmpFileDimacs = tmpFileDimacsPath.string();
    std::ofstream fdimacs(tmpFileDimacs);
    if (!fdimacs)
        throw std::runtime_error("cannot write file " + tmpFileDimacs);
    fdimacs << cnf_dimacs(cnf);
    fdimacs.close();
    const std::string outputFile = "output.txt";

    std::stringstream solveCommand;
    solveCommand << bddSolver.command << " " << tmpFileDimacs << " " << outputFile;
    std::string result = internal::execute_command(solveCommand.str().c_str());
    std::remove(tmpFileDimacs.c_str());

    std::ifstream fsolutions(outputFile);
    std::string line;
    int solutionCount = 0;
    while (std::getline(fsolutions, line)) {
        std::stringstream ss;
        ss << line;
        std::vector<bool> solution; solution.reserve(cnf.first);
        for (int var = 0; var < cnf.first; var++) {
            int x; ss >> x;
            if (x == 0)
                throw std::runtime_error("not all variables written, what does it mean?");
            solution.push_back(x > 0);
        }
        solutionCount++;
        if (callback != NULL) {
            R result = transform(solution, transformationData);
            if (!callback(result, callbackParam))
                break;
        }
    }
    std::remove(outputFile.c_str());
    return solutionCount > 0;
}

} // namespace internal

} // namespace ba_graph

#endif
