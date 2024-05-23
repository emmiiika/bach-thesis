#ifndef BA_GRAPH_INVARIANTS_DISTANCE_HPP
#define BA_GRAPH_INVARIANTS_DISTANCE_HPP

#include <cassert>
#include <climits>
#include <queue>

namespace ba_graph
{

// stops at depth cutoff (bfs is not continued from vertices at distance >= cutoff)
inline int distance(const Graph &G, Number v1, Number v2, int cutoff=INT_MAX)
{
#ifdef BA_GRAPH_DEBUG
    assert(G.contains(v1) && G.contains(v2));
#endif
    std::map<Number, int> dist;
    for (auto &r : G) {
        dist[r.n()] = -1;
    }
    dist[v1] = 0;

    std::queue<const Rotation *> q;
    q.push(&G[v1]);

    bool finished = false;
    while (!finished && !q.empty()) {
        const Rotation &r = *q.front();
        if (dist[r.n()] >= cutoff)
            break; // all the remaining vertices are at distance at least cutoff + 1
        for (auto &i : r) {
            const Rotation &neighbour = i.r2();
            if (dist[neighbour.n()] == -1) {
                q.push(&neighbour);
                dist[neighbour.n()] = dist[r.n()] + 1;
                if (r.n() == v2) {
                    finished = true;
                    break;
                }
            }
        }
        q.pop();
    }

    return dist[v2];
}

// distances from a set of vertices to the remaining reachable vertices
inline std::map<Number, int> distances(const Graph &G, std::set<Number> startingPoints)
{
#ifdef BA_GRAPH_DEBUG
    assert(startingPoints.size() > 0);
    for (auto &v : startingPoints)
        assert(G.contains(v));
#endif
    std::map<Number, int> dist;

    std::queue<const Rotation *> q;
    for (auto &v : startingPoints) {
        dist[v] = 0;
        q.push(&G[v]);
    }

    while (!q.empty()) {
        const Rotation &r = *q.front();
        for (auto &i : r) {
            const Rotation &neighbour = i.r2();
            if (dist.count(neighbour.n()) == 0) {
                q.push(&neighbour);
                dist[neighbour.n()] = dist[r.n()] + 1;
            }
        }
        q.pop();
    }
    return dist;
}

inline std::map<Number, int>  distances(const Graph &G, Number w)
{
#ifdef BA_GRAPH_DEBUG
    assert(G.contains(w));
#endif
    return distances(G, std::set<Number>({w}));
}

inline int distance(const Graph &G, Location l1, Location l2, int cutoff = INT_MAX)
{
#ifdef BA_GRAPH_DEBUG
    assert(G.contains(l1) && G.contains(l2));
#endif
    int d = G.order() + 1, d2;

    d2 = distance(G, l1.n1(), l2.n1(), cutoff);
    d = (d2 == -1) ? d : std::min(d, d2);

    d2 = distance(G, l1.n1(), l2.n2(), cutoff);
    d = (d2 == -1) ? d : std::min(d, d2);

    d2 = distance(G, l1.n2(), l2.n1(), cutoff);
    d = (d2 == -1) ? d : std::min(d, d2);

    d2 = distance(G, l1.n2(), l2.n2(), cutoff);
    d = (d2 == -1) ? d : std::min(d, d2);

    return (d <= G.order()) ? d : -1;
}

inline int distance(const Graph &G, Edge e1, Edge e2, int cutoff = INT_MAX)
{
#ifdef BA_GRAPH_DEBUG
    assert(G.contains(e1.v1(), e1) && G.contains(e2.v2(), e2));
#endif
    Location l1 = G.find(e1.v1(), e1).second->l();
    Location l2 = G.find(e2.v1(), e2).second->l();
    return distance(G, l1, l2, cutoff);
}

// TODO possibly inefficient and could be done better?
inline std::vector<std::vector<Number>> components(const Graph &G)
{
    std::vector<std::vector<Number>> result;
    std::set<Number> notProcessed;
    for (auto &r : G)
        notProcessed.insert(r.n());

    while (notProcessed.size() > 0) {
        auto d = distances(G, *notProcessed.begin());
        std::vector<Number> component;
        for (auto it = d.begin(); it != d.end(); it++) {
            component.push_back(it->first);
            notProcessed.erase(it->first);
        }
        result.push_back(component);
    }
    return result;
}

}  // namespace end

#endif
