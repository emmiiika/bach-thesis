#ifndef BA_GRAPH_IO_PREGRAPH_HPP
#define BA_GRAPH_IO_PREGRAPH_HPP

#include <fstream>

// Functions to read and write graphs in pregraph format. Pregraphs are cubic graphs with semi-edges
// (although you can write any graph in this format). Pregraph is a binary format. More here: http://caagt.ugent.be/pregraphs/

namespace ba_graph {

namespace internal {

Graph semi_edge_dump_to_many_dumps(Graph &g, Factory *f = &static_factory) {
    Graph h(createG(*f));
    for (int i = 0; i < g.order() - 1; i++) {
        addV(h, i);
    }
    int j = h.order();
    for (auto &rotation : g) {
        for (auto &incidence : rotation) {
            if (incidence.n1() < incidence.n2()) {
                if (incidence.n1() == g.order() - 1 || incidence.n2() == g.order() - 1) {
                    addV(h, j);
                    addE(h, Location(incidence.n1(), j));
                    j++;
                } else {
                    addE(h, Location(incidence.n1(), incidence.n2()));
                }
            }
        }
    }
    return h;
}

int read_header(std::istream &in) {
    int endian = LITTLE_ENDIAN;
    // First we read the header and determine the file format
    char buffer[16];
    in.read(buffer, 15);
    buffer[15] = '\0';
    std::string header = ">>pregraph_code";
    if (buffer != header) {
        throw std::runtime_error("Header is in wrong format");
    }
    // next we read the endian of the file
    char en[3];
    in.read(buffer, 1);
    if (buffer[0] != ' ') {
        in.putback(1);
    }
    in.read(en, 2);
    en[2] = 0;
    
    std::string be = "be";
    std::string le = "le";
    std::string arrows = "<<";
    if (en == be) {
        endian = BIG_ENDIAN;
        in.read(buffer, 2);
    } else if (en == le) {
        in.read(buffer, 2);
    } else if (en == arrows) {
    } else {
        throw std::runtime_error("Endian is in wrong format");
    }
    return endian;
}

unsigned short read_2byte_number(std::istream &in, int endian) {
    char c[2];
    unsigned short res;
    if (in.read(c, 2).eof()) {
        throw std::runtime_error("File is in wrong format");
    }
    if (endian == BIG_ENDIAN) {
        res = c[0] * 256 + c[1];
    } else {
        res = c[1] * 256 + c[0];
    }
    return res;
}

unsigned short read_next_number(std::istream &in, bool bignum, int endian) {
    if (bignum) {
        return read_2byte_number(in, endian);
    }
    char k;
    in.get(k);
    return (unsigned short)k;
}

Graph read_one_pregraph(std::istream &in, int endian, Factory *f = &static_factory) {
    int signum, number;
    signum = read_next_number(in, false, endian);

    // if the code starts with a zero, all the entries are two bytes
    // else the number we just read was the order of the graph
    if (signum == 0) {
        number = read_next_number(in, true, endian);
    } else {
        number = signum;
    }
    int n = number;
    if (number == 0) {
        throw std::runtime_error("graph of unsupported order");
    }
    Graph g(createG(*f));
    for (int i = 0; i < n + 1; i++) { // +1 to store semiedges
        addV(g, i);
    }
    int i = 0;
    while (i <= n - 1) {
        number = read_next_number(in, signum == 0, endian);
        number--;
        if (number != -1) {
            addE(g, Location(i, number));
        } else {
            i++; // next vertex
        }
    }
    return std::move(semi_edge_dump_to_many_dumps(g));
}

Graph renumber_graph(Graph &g) {
    Graph newG(createG());
    addMultipleV(newG, g.order());
    std::unordered_map<int, int> map;
    int curOrder3 = 0;
    int curOrder1 = g.order() - 1;
    for (auto &rotation : g) {
        for (auto &incidence : rotation) {
            if (incidence.n1() < incidence.n2()) {
                auto n1 = map.find(incidence.n1().to_int());
                if (n1 == map.end()) {
                    n1 = map.emplace(incidence.n1().to_int(), g[incidence.n1()].degree() == 1 ? curOrder1-- : curOrder3++).first;
                }
                auto n2 = map.find(incidence.n2().to_int());
                if (n2 == map.end()) {
                    n2 = map.emplace(incidence.n2().to_int(), g[incidence.n2()].degree() == 1 ? curOrder1-- : curOrder3++).first;
                }
                n1 = map.find(incidence.n1().to_int());

                addE(newG, Location(n1->second, n2->second));
            }
        }
    }
    return newG;
}

} // namespace internal

std::vector<Graph> read_pregraphs_from_file(const std::string &fileName, Factory *f = &static_factory) {
    std::ifstream in(fileName);
    if (!in)
        throw std::runtime_error("cannot read file " + fileName);
    auto endian = internal::read_header(in);
    std::vector<Graph> graphs;
    while (!in.eof()) {
        if ((short)in.peek() == -1)
            break;
        graphs.push_back(std::move(internal::read_one_pregraph(in, endian, f)));
    }
    return graphs;
}

template <typename P>
void read_pregraph_stream(std::istream &in,
                          std::function<void(Graph &, Factory &, P *)> callback,
                          P *callbackParam, Factory *f = &static_factory) {
    auto endian = internal::read_header(in);
    while (!in.eof()) {
        if ((short)in.peek() == -1)
            break;
        Graph g = std::move(internal::read_one_pregraph(in, endian, f));
        callback(g, *f, callbackParam);
    }
}

template <typename P>
void read_pregraph_file(const std::string &fileName,
                        std::function<void(Graph &, Factory &, P *)> callback,
                        P *callbackParam, Factory *f = &static_factory) {
    std::ifstream fin(fileName);
    if (!fin)
        throw std::runtime_error("cannot read file " + fileName);
    read_pregraph_stream<P>(fin, callback, callbackParam, f);
}

// assumes g has single semiedge dump
Graph copy_without_semiedges(Graph &g) {
    Graph h(createG());

    int order = g.order();
    for (int i = 1; i < order; i++) {
        addV(h, i);
    }
    for (auto &rotation : g) {
        for (auto &incidence : rotation) {
        if (incidence.n1() < incidence.n2()) {
            if (incidence.n2() < order) {
            addE(h, Location(incidence.n1(), incidence.n2()));
            }
        }
        }
    }

    return h;
}

void write_pregraph(FILE* f, Graph &G) {
    if (G.order() >= 255) throw std::runtime_error("Unsupported order: can not write graph with order 255 and more");

    Graph g = internal::renumber_graph(G);

    int newOrder = g.order();
    for (auto &rotation : g) {
        if (rotation.degree() == 1) {
            newOrder--;
        }
    }
    // note: it is possible, that write operation could be done faster using write() method
    fprintf(f, "%c", (unsigned char)newOrder);
    for (auto &rotation : g) {
        if (rotation.degree() != 1) {
            for (auto &incidence : rotation) {
                if (g[incidence.n2()].degree() == 1) {
                    fprintf(f, "%c", (unsigned char) newOrder + 1);
                } else if (incidence.n1() < incidence.n2()) {
                    fprintf(f, "%c", (unsigned char) incidence.n2().to_int() + 1);
                }
            }
            fprintf(f, "%c", (unsigned char) 0);
        }
    }
}

void write_pregraph_code(const std::string fileName, std::vector<Graph> &graphs) {
    // write header
    FILE* f;
    f = fopen(fileName.c_str(), "w");
    fprintf(f, ">>pregraph_code %s<<", "le");

    for (auto &g : graphs) {
        write_pregraph(f, g);
    }
    fclose(f);
}

} // namespace ba_graph

#endif
