#ifndef BA_GRAPH_INVARIANTS_DEGREE_HPP
#define BA_GRAPH_INVARIANTS_DEGREE_HPP

namespace ba_graph
{

inline int max_deg(const Graph &G)
{
    int m = 0;
    for (auto &r : G) {
        if (r.degree() > m)
            m = r.degree();
    }
    return m;
}

// returns one of the vertices of G with maximum degree or -1, if G is empty
inline Number max_deg_vertex(const Graph &G)
{
    int m = 0;
    Number v = -1;
    for (auto &r : G) {
        if (r.degree() > m) {
            m = r.degree();
            v = r.n();
        }
    }
    return v;
}

inline int min_deg(const Graph &G)
{
    if (G.size() == 0)
        return 0;
    int m = 2 * G.size() + 1;
    for (auto &r : G) {
        if (r.degree() < m)
            m = r.degree();
    }
    return m;
}

// returns one of the vertices of G with minimum degree or -1, if G is empty
inline Number min_deg_vertex(const Graph &G)
{
    if (G.order() == 0) return -1;
    int m = 2 * G.size() + 1;
    Number v = -1;
    for (auto &r : G) {
        if (r.degree() < m) {
            m = r.degree();
            v = r.n();
        }
    }
    return v;
}

inline double avg_deg(const Graph &g)
{
    int sum = 0;
    for (auto &r : g) {
        sum += r.degree();
    }
    return sum / g.order();
}

inline std::vector<int> degree_sequence(const Graph &g)
{
    std::vector<int> result;
    result.reserve(g.order());
    for (auto &r : g)
        result.push_back(r.degree());
    std::sort(result.begin(), result.end(), [](int d1, int d2) { return d1 > d2; });
    return result;
}

}  // namespace end

#endif
