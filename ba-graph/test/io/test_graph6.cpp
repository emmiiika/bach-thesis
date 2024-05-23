#include "implementation.h"
#include <io/graph6.hpp>
#include <invariants.hpp>
#include <io/print_nice.hpp>

#include <cassert>
#include <cstdio>

using namespace ba_graph;

int main()
{
	std::string s10 = "J";
	assert(internal::decode_graph6_order(s10.begin(), s10.end()) == 11);
	std::string s11 = "~B?x";
	assert(internal::decode_graph6_order(s11.begin(), s11.end()) == 12345);
	std::string s12 = "~~?ZZZZZ";
	assert(internal::decode_graph6_order(s12.begin(), s12.end()) == 460175067);

	std::string s1 = "DQc";
	Graph g1(read_graph6(s1.begin(), s1.end()));
	assert(print_nice(g1) == "0: 2 4\n1: 3\n2: 0\n3: 1 4\n4: 0 3\n");

	std::string s2 = "I?h]@eOWG";
	Graph g2(read_graph6(s2.begin(), s2.end()));
	assert(g2.order() == 10);
	assert(min_deg(g2) == 3);
	assert(max_deg(g2) == 3);
	assert(!is_edge_colourable_basic(g2, 3));

	std::string file1 = "in1.g6";
	std::string file1_contents = R"(C~
G}GOW[
G{S_g[
G{O_ww
GsXP_[
GsXPGs
E{Sw
Es\o
)";
	std::ofstream f1out(file1); f1out << file1_contents; f1out.close();

	assert(read_graph6_file_count(file1, 3, 10) == 5);
	auto graphs = read_graph6_file(file1);
	assert(graphs.size() == 8);
	assert(graphs.graphs().size() == 8);
	assert(graphs.strings().size() == 8);
	auto graphsPortion1 = read_graph6_file(file1, static_factory, 5, 7);
	assert(graphsPortion1.size() == 2);
	assert(graphsPortion1.graphs().size() == 2);
	assert(graphsPortion1.strings().size() == 2);
	auto graphsPortion2 = read_graph6_file(file1, static_factory, 2, 7, 3);
	assert(graphsPortion2.size() == 2);
	assert(graphsPortion2.graphs().size() == 2);
	assert(graphsPortion2.strings().size() == 2);
	assert(graphsPortion2.get_string(0) == graphs.get_string(2));
	assert(graphsPortion2.get_string(1) == graphs.strings()[5]);

	std::string file2 = "in2.g6";
	auto gs = graphs.graphs();
	write_graph6_file(file2, gs.begin(), gs.end());
	std::ifstream f1(file1, std::ifstream::ate | std::ifstream::binary);
	std::ifstream f2(file2, std::ifstream::ate | std::ifstream::binary);
	assert(f1.tellg() == f2.tellg());
	remove(file2.c_str());
	int orderSum = 0;
	auto cb = [](std::string &, Graph &G, Factory &, int *orderSum) {
			*orderSum += G.order();
		};
	read_graph6_file<int>(file1, cb, &orderSum);
	assert(orderSum == 56);
	remove(file1.c_str());

	std::string s3 = ":Fa@x^";
	Graph h1(read_graph6_line(s3));
	assert(print_nice(h1) == "0: 1 2\n1: 0 2\n2: 0 1\n3:\n4:\n5: 6\n6: 5\n");
	assert(write_sparse6(h1) == s3 + '\n');

	std::string s4 = R"(:yc?hOAH@bro[icH`riomo??o_[OJEBq@KgVKEr`|?bQHtAlWnWKub\oz]NwCMHFcQxc}`RiTjGgwbECDW@QMQWOQ{SQq`CXhXhCHrAjYKOfdvF)";
	Graph h2(read_graph6_line(s4));
	assert(h2.order() == 58);
	assert(h2.size() == 87);
	assert(max_deg(h2) == 3);
	assert(write_sparse6(h2) == s4 + '\n');

	std::string s5 = ":A_";
	Graph h3(read_graph6_line(s5));
	assert(print_nice(h3) == "0: 1 1 1\n1: 0 0 0\n");
	assert(write_sparse6(h3) == s5 + '\n');
}
