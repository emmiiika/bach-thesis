#ifndef BA_GRAPH_IO_OLDBA_HPP
#define BA_GRAPH_IO_OLDBA_HPP

#include "../graphs/basic.hpp"
#include "../operations/copies.hpp"

#include <string>
#include <iostream>
#include <fstream>
#include <sstream>


namespace ba_graph
{

namespace internal
{

inline std::string getline_ba(std::istream &in, bool skip_empty_lines=true)
{
    std::string s;
    while (1) {
        std::getline(in, s);
        // fix non-unix end-of-line character
        if ((s.size() > 0) && (s[s.size() - 1] == '\r'))
            s.resize(s.size() - 1);
        if (s.size() > 0) {
            if (s[0] == '{')
                continue; // skip a comment
            else
                break; // we have the line
        } else {
            // an empty line
            if (skip_empty_lines)
                continue;
            else
                break;
        }
    }
    return s;
}

} // namespace internal

inline Graph read_oldBA_from_stream(std::istream &in, bool numberRemoved, Factory &f=static_factory)
{
    if (!numberRemoved)
        internal::getline_ba(in); // we are ignoring the graph number
    std::stringstream line = std::stringstream(internal::getline_ba(in));
    int order; line >> order;
    Graph G(empty_graph(order, f));
    for (int i = 0; i < order; i++) {
        // do not skip empty lines because of isolated vertices
        line = std::stringstream(internal::getline_ba(in, false));
        auto it = std::istream_iterator<int>(line);
        auto numbers = std::vector<int>(it, std::istream_iterator<int>());
        for (int v : numbers) {
            if (i < v)
                addE(G, Location(i, v), f);
        }
    }
    return G;
}

// TODO check for errors?
inline std::vector<Graph> read_oldBA_stream(std::istream &in, Factory &f=static_factory)
{
    int count;
    std::stringstream line = std::stringstream(internal::getline_ba(in));
    line >> count;
    std::vector<Graph> graphs;
    for (int i = 0; i < count; i++)
        graphs.emplace_back(std::move(read_oldBA_from_stream(in, false, f)));
    return graphs;
}

inline std::vector<Graph> read_oldBA_file(const std::string &fileName, Factory &f=static_factory)
{
    std::ifstream fin(fileName);
    if (!fin)
        throw std::runtime_error("cannot read file " + fileName);
    return read_oldBA_stream(fin, f);
}


inline std::string write_oldBA(const Graph &G)
{
    Factory f;
    Graph H(copy_other_factory(G, f));
    renumber_consecutive_stable(H, f);
    std::stringstream s;
    s << H.order() << std::endl;
    for (int v = 0; v < H.order(); ++v) {
        std::string separator = "";
        for (auto &i : H[v]) {
            if ((!i.is_loop()) || i.is_primary()) {
                s << separator << i.r2().n().to_int();
                separator = " ";
            }
        }
        s << std::endl;
    }
    return s.str();
}

template <class GraphIterator>
void write_oldBA_file(std::string fileName, GraphIterator begin, GraphIterator end, int graphCount)
{
    std::ofstream fout(fileName);
    fout << graphCount << std::endl << std::endl;
    int k = 0;
    while (begin != end) {
        fout << ++k << std::endl;
        fout << write_oldBA(*begin);
        if (k < graphCount)
            fout << std::endl;
        begin++;
    }
}


} // namespace end

#endif

// TODO file I/O can possibly be made faster by using Boost mapped_file_source
// http://www.cplusplus.com/forum/general/94032/
