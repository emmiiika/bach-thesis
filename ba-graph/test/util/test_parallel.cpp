#include <cassert>

#include "implementation.h"
#include <graphs/stored_graphs.hpp>
#include <util/parallel.hpp>

using namespace ba_graph;

int main() {
	auto file = "../../resources/graphs/" + internal::stored_snark_path(4, 5, 26);

	int count;
	auto cb = [&](std::string &, Graph &, Factory &) {
		#pragma omp atomic
		count++;
	};

	count = 0;
	ReadGraphsParams params;
	params.chunk_size = 17;
	params.from = 5;
	params.to = 15;
	read_graph6_file_chunks(file, params, cb);
	assert(count == 10);

	count = 0;
	params.chunk_size = 17;
	params.from = 17;
	params.to = 35;
	read_graph6_file_chunks(file, params, cb);
	assert(count == 18);

	count = 0;
	params.chunk_size = 17;
	params.from = 5;
	params.to = 275;
	read_graph6_file_chunks(file, params, cb);
	assert(count == 270);

	auto cb2 = [&](std::string &, Graph &, Factory &, void *) {
		#pragma omp atomic
		count++;		
	};

	count = 0;
	params.chunk_size = 19;
	params.from = 5;
	params.to = 275;
	read_graph6_file_chunks<void>(file, params, cb2, 0);
	assert(count == 270);

    return 0;
}
