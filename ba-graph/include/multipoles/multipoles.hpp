#ifndef BA_GRAPH_MULTIPOLES_MULTIPOLES_HPP
#define BA_GRAPH_MULTIPOLES_MULTIPOLES_HPP

#include <iostream>
#include <vector>

namespace ba_graph
{

class Connector
{
  public:
    std::vector<Number> numbers;

    Connector(const std::vector<Number> &vec_) : numbers(vec_) {}

    bool check_in(const Graph &g) const
    {
        if (numbers.size() != std::set<Number>(numbers.begin(), numbers.end()).size())
            return false;
        for (auto n : numbers) {
            if (!g.contains(n))
                return false;
            if (g[n].degree() != 1)
                return false;
        }
        return true;
    }

    Number &operator[](int i) { return numbers[i]; }
    const Number &operator[](int i) const { return numbers[i]; }

    int size() const { return numbers.size(); }

    std::vector<Number>::iterator begin() { return numbers.begin(); }
    std::vector<Number>::iterator end() { return numbers.end(); }
};

class Multipole
{
  public:
    std::vector<Connector> connectors;

    Multipole(const std::vector<Connector> &vc) : connectors(vc) {}

    Multipole(const Multipole &m) : connectors(m.connectors) {}

    bool check_in(const Graph &g) const
    {
        std::set<Number> s;
        size_t s_size = 0;
        for (const auto &p : connectors) {
            if (!p.check_in(g))
                return false;
            s_size += p.numbers.size();
            s.insert(p.numbers.begin(), p.numbers.end());
            if (s.size() != s_size)
                return false;
        }
        return true;
    }

    Connector &operator[](int i) { return connectors[i]; }
    const Connector &operator[](int i) const { return connectors[i]; }

    int size() const { return connectors.size(); }

    std::vector<Connector>::iterator begin() { return connectors.begin(); }
    std::vector<Connector>::iterator end() { return connectors.end(); }

    // concatenates all the connectors into one
    void flatten()
    {
        std::vector<Number> dangling;
        for (auto &c : connectors)
            for (auto n : c)
                dangling.push_back(n);
        connectors.clear();
        Connector newC(dangling);
        connectors.push_back(newC);
    }
};

inline std::ostream &operator<<(std::ostream &out, const Connector &c)
{
    out << "{";
    for (int i = 0; i < c.size(); ++i) {
        out << c[i].to_int();
        if (i < c.size() - 1)
            out << ", ";
    }
    out << "}";
    return out;
}

inline std::ostream &operator<<(std::ostream &out, const Multipole &m)
{
    out << "[";
    for (int i = 0; i < m.size(); ++i) {
        out << m[i];
        if (i < m.size() - 1)
            out << ", ";
    }
    out << "]";
    return out;
}

} // namespace ba_graph

#endif
