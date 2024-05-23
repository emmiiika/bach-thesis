#ifndef BA_GRAPH_SAT_SOLVER_HPP
#define BA_GRAPH_SAT_SOLVER_HPP

#include "../config/configuration.hpp"

#include <cassert>
#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>
#include <stdexcept>

namespace ba_graph
{

class SatSolver
{
  public:
    enum OutputType { MINISAT, INTERNAL_CMSAT, SAT_COMPETITION };

    OutputType outputType;
    bool external;
    std::string command;

    SatSolver(OutputType ot, bool external, std::string cmd="")
            : outputType(ot), external(external), command(cmd)
    {
#ifdef BA_GRAPH_DEBUG
        if (external) assert(command != "");
#endif
    }

    SatSolver(Configuration cfg, int solverIndex)
    {
        external = true;
        auto ot = cfg.sat_solver_output_type(solverIndex);
        if (ot == "MINISAT")
            outputType = MINISAT;
        else if (ot == "SAT_COMPETITION")
            outputType = SAT_COMPETITION;
        else
            throw std::runtime_error("unsupported sat solver output type");
        command = cfg.sat_solver_command(solverIndex);
#ifdef BA_GRAPH_DEBUG
            if (external) assert(command != "");
#endif
    }
};

class AllSatSolver
{
  public:
    enum OutputType { BDD_MINISAT, INTERNAL_CMSAT };

    OutputType outputType;
    bool external;
    std::string command;

    AllSatSolver(OutputType ot, bool external, std::string cmd="")
            : outputType(ot), external(external), command(cmd)
    {
#ifdef BA_GRAPH_DEBUG
    if (external) assert(command != "");
#endif
    }

    AllSatSolver(Configuration cfg, int solverIndex)
    {
        external = true;
        auto ot = cfg.allsat_solver_output_type(solverIndex);
        if (ot == "BDD_MINISAT")
            outputType = BDD_MINISAT;
        else
            throw std::runtime_error("unsupported sat solver output type");
        command = cfg.allsat_solver_command(solverIndex);
    }
};

template <typename T>
bool enumerateListCallback(T &t, std::vector<T> *list)
{
    list->push_back(std::move(t));
    return true;
}

} // namespace ba_graph

#endif
