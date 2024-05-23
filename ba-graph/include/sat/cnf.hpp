#ifndef BA_GRAPH_SAT_CNF_HPP
#define BA_GRAPH_SAT_CNF_HPP

#include <iostream>
#include <sstream>
#include <vector>

namespace ba_graph
{

class Lit
{
    int x;

    Lit(int y) : x(y) {}

  public:
    Lit(int var, bool isInverted) : x(var + var + isInverted) {}

    bool neg() const { return x & 1; }
    int var() const { return x >> 1; }

    Lit operator~() const { return Lit(x ^ 1); }

    bool operator==(const Lit &p) const { return x == p.x; }
    bool operator!=(const Lit &p) const { return x != p.x; }
};

inline std::ostream &operator<<(std::ostream &os, const Lit &lit)
{
    os << (lit.neg() ? "-" : "") << (lit.var() + 1);
    return os;
}

inline std::ostream &operator<<(std::ostream &os, const std::vector<Lit> &lits)
{
    for (std::vector<Lit>::size_type i = 0; i < lits.size(); i++) {
        os << lits[i];
        if (i != lits.size() - 1)
            os << " ";
    }
    return os;
}

// number of variables, clauses as vectors of literals; variables numbered from 0
typedef std::vector<Lit> Clause;
typedef std::pair<int, std::vector<Clause>> CNF;

static const CNF cnf_unsatisfiable = {1, {{Lit(0, false)}, {Lit(0, true)}}};
static const CNF cnf_satisfiable = {1, {{Lit(0, false)}}};

inline std::string cnf_dimacs(const CNF &cnf)
{
    std::stringstream out;
    auto varsCount = cnf.first;
    auto clauses = cnf.second;
    out << "p cnf " << varsCount << " " << clauses.size() << std::endl;
    for (auto &clause : clauses) {
        std::string separator = "";
        for (auto &l : clause) {
            out << separator << l;
            separator = " ";
        }
        out << separator << 0 << std::endl;
    }
    return out.str();
}

// transforms a given formula in DNF into an equisatisfiable formula in CNF,
// using one new variable for each clause in the DNF formula;
// the new variables are numbered from firstNewVariable
inline std::vector<Clause> dnf_to_cnf(std::vector<Clause> dnf, int firstNewVariable)
{
    Clause z;
    std::vector<Clause> cnf;
    int nextNewVar = firstNewVariable;
    for (Clause c : dnf) {
        z.push_back(Lit(nextNewVar, false));
        for (Lit l : c)
            cnf.push_back({Lit(nextNewVar, true), l});
        nextNewVar++;
    } 
    cnf.push_back(z);
    return cnf;
}

} // namespace ba_graph

#endif
