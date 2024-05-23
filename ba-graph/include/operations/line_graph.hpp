#ifndef BA_GRAPH_OPERATIONS_LINE_GRAPH_HPP
#define BA_GRAPH_OPERATIONS_LINE_GRAPH_HPP

#include <vector>

namespace ba_graph
{

// the second part maps edges of the original graph to vertices of the line graph
inline std::pair<Graph, std::map<Edge, Number>> line_graph_with_map(const Graph &G, Factory &f = static_factory)
{
    Graph H(createG(f));
    std::map<Edge, Number> edgeToVertex;
    int num = 0;
    for (auto e : G.list(RP::all(), IP::primary(), IT::e())) {
        auto v = addV(H, num, f).v();
        edgeToVertex[e] = num;
        num++;
    }

    for (const auto &r : G) {
        std::vector<Edge> ve;
        ve.reserve(G.size());
        for (const auto &i : r)
            ve.push_back(i.e());
        for (std::vector<int>::size_type i = 0; i < ve.size(); ++i)
            for (std::vector<int>::size_type j = i + 1; j < ve.size(); ++j)
                addE(H, Location(edgeToVertex[ve[i]], edgeToVertex[ve[j]]), f);
    }

    return std::pair(std::move(H), edgeToVertex);
}

inline Graph line_graph(const Graph &G, Factory &f = static_factory)
{
    return line_graph_with_map(G, f).first;
}

} // namespace ba_graph

#endif
