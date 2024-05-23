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

cxxopts::Options options("filtermultipoles", "\nFilter input multipoles (g6/s6) according to order, cyclic connectivity, girth, 3-edge-colourability etc.\nCan canonicalize output if desired.\n");

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

			("k, dangling", "# of dangling edges =", cxxopts::value<int>())
			("n, order", "order =", cxxopts::value<int>())
			("c, cc", "contains no obstacles to cyclic connectivity = ", cxxopts::value<int>())
			("g, girth", "girth >=", cxxopts::value<int>())

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
			auto terminals = G.list(RP::degree(1), RT::n());
			if (o.count("dangling")) {
				int k = o["dangling"].as<int>();
				if ((int)terminals.size() != k) return;
			}
			if (o.count("order")) {
				int order = o["order"].as<int>();
				if (G.order() - (int)terminals.size() != order) return;
			}
			if (o.count("cc")) {
				int cc = o["cc"].as<int>();
				if (contains_cc_obstacle(G, Multipole({Connector(terminals)}), cc)) return;
			}
			if (o.count("girth")) {
				int g = o["girth"].as<int>();
				if (g > 1 && has_short_cycle(G, g - 1)) return;
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
