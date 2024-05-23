#include <climits>
#include <iomanip>
#include <iostream>
#include <omp.h>

#include <impl/basic/include.hpp>
#include <config/configuration.hpp>
#include <algorithms/cyclic_connectivity.hpp>
#include <algorithms/isomorphism/isomorphism_nauty.hpp>
#include <snarks/colouring.hpp>
#include <snarks/colouring_nagy.hpp>
#include <io/graph6.hpp>
#include <util/cxxopts.hpp>
#include <util/parallel.hpp>

using namespace ba_graph;

const int lineBufferSize = 100000;

cxxopts::Options options("filtergraphs", "\nFilter input graphs (g6/s6) according to order, cyclic connectivity, girth, 3-edge-colourability etc.\nCan canonicalize output if desired.\n");

void writeGraphs(std::vector<std::string> *s6graphs)
{
	for (auto &s : *s6graphs)
		std::cout << s;
	s6graphs->clear();
}

void wrong_usage()
{
	std::cout << options.help() << std::endl;
	exit(1);
}

/*
 * Takes input file with g6/s6 graphs, applies a filter and puts the resulting graphs into the output s6 file (as canonicalized string if desired).
 */
int main(int argc, char **argv) {
	try {
		options.add_options()
			("h, help", "print help")

			("n, order", "order =", cxxopts::value<int>())
			("c, cc", "cyclic connectivity >= [only for cubic]", cxxopts::value<int>())
			("g, girth", "girth >=", cxxopts::value<int>())

			("nocutvertices", "no cut-vertices")
			("cutvertices", "# of cut-vertices >=", cxxopts::value<int>())
			("nocutedges", "no cut-edges")
			("cutedges", "# of cut-edges >=", cxxopts::value<int>())

			("simple", "output only simple graphs")
			("multiple", "# of multiple edges >=", cxxopts::value<int>())
			("noloops", "output only multigraps without loops")
			("loops", "# of loops >=", cxxopts::value<int>())

			("s, snark", "output only uncolourable graphs [only for subcubic]")

			("canonical", "canonicalise output [nauty, only for g >= 3]")

			("chunk", "size of input chunk for each thread (in lines)", cxxopts::value<int>()->default_value(std::to_string(1000)))
			;
		auto o = options.parse(argc, argv);
		
		if (o.count("help")) {
			std::cout << options.help() << std::endl;
			return 0;
		}

		Configuration cfg;
		try {
			cfg.load_from_file("ba_graph.cfg");
			omp_set_num_threads(cfg.num_threads());
		} catch (...) {
			omp_set_num_threads(1);
		}

		auto processGraph = [&](std::string &sG, Graph &G, Factory &, std::vector<std::string> *s6graphs) {
			if (o.count("order")) {
				int order = o["order"].as<int>();
				if (G.order() != order) return;
			}
			if (o.count("cc")) {
				int cc = o["cc"].as<int>();
				if (!is_cyclically_connected(G, cc)) return;
			}
			if (o.count("girth")) {
				int g = o["girth"].as<int>();
				if (g > 1 && has_short_cycle(G, g - 1)) return;
			}

			if (o.count("nocutvertices")) {
				if (has_cut_vertex(G)) return;
			}
			if (o.count("cutvertices")) {
				int cutvertices = o["cutvertices"].as<int>();
				if ((int)cut_vertices(G).size() < cutvertices) return;
			}
			if (o.count("nocutedges")) {
				if (has_cut_edge(G)) return;
			}
			if (o.count("cutedges")) {
				int cutedges = o["cutedges"].as<int>();
				if ((int)cut_edges(G).size() < cutedges) return;
			}

			if (o.count("simple")) {
				if (has_loop(G) || has_parallel_edge(G)) return;
			}
			if (o.count("multiple")) {
				int multiple = o["multiple"].as<int>();
				if ((int)parallel_edges(G).size() < multiple) return;
			}
			if (o.count("noloops")) {
				if (has_loop(G)) return;
			}
			if (o.count("loops")) {
				int loops = o["loops"].as<int>();
				if ((int)G.list(RP::all(), IP::loop() && IP::primary()).size() < loops) return;
			}

			if (o.count("snark")) {
				if (is_colourable<NagyColouriser>(G)) return;
			}

			// G satisfies all the requirements
			std::string sG_out;
			if (o.count("canonical"))
				sG_out = canonical_sparse6(G) + "\n";
			else
				sG_out = sG + "\n";
			// if not canonicalising, keep the original string for G

			#pragma omp critical
			{
				s6graphs->push_back(sG_out);
				if (s6graphs->size() > lineBufferSize)
					writeGraphs(s6graphs);
			}
		};

		std::vector<std::string> s6graphs;
		ReadGraphsParams params;
		params.chunk_size = o["chunk"].as<int>();
		read_graph6_stream_chunks<std::vector<std::string>>(
				std::cin, params, processGraph, &s6graphs);
		writeGraphs(&s6graphs);
	} catch (const cxxopts::OptionException& e) {
		std::cerr << "error parsing options: " << e.what() << std::endl;
		exit(1);
	}

	return 0;
}
