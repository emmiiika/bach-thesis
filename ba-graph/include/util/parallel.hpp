#ifndef BA_GRAPH_UTIL_PARALLEL_HPP
#define BA_GRAPH_UTIL_PARALLEL_HPP

#include <climits>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <omp.h>

#include "../io/graph6.hpp"

namespace ba_graph
{

typedef struct ReadGraphsParams {
	Factory *factory; // if NULL, a local one should be created
	int chunk_size;
	int graph_count;
	int from;
	int to;
	bool show_initial_msg;
	bool report_progress;
	int progress_interval;

	ReadGraphsParams()
	{
		factory = NULL;
		chunk_size = 1;
		graph_count = INT_MAX;
		from = 0;
		to = INT_MAX;
		show_initial_msg = false;
		report_progress = false;
		progress_interval = 1;
	}
} ReadGraphsParams;

// chunks go to separate threads
template <typename P>
void read_graph6_stream_chunks(std::istream &in, ReadGraphsParams params,
		std::function<void(std::string &sG, Graph &G, Factory &f, P *param)> callback, P *callback_param)
{
	int64_t totalGraphs = params.graph_count;
	int64_t chunkCount = (totalGraphs + params.chunk_size - 1) / params.chunk_size; // ceiling

	if (params.show_initial_msg) {
		std::cerr << "============ there are " << totalGraphs << " graphs"
				<< ", will be read in " << chunkCount << " chunks of " << params.chunk_size  << std::endl;
	}

	int count = 0;
	std::string line;
	while ((count < params.from) && std::getline(in, line))
		count++;

	#pragma omp parallel for schedule(dynamic, 1)
	for (int i = 0; i < chunkCount; ++i) {
		if (in.eof()) continue;
		std::vector<std::string> graphLines;
		#pragma omp critical
		{
			int upperLimit = std::min(params.chunk_size, params.to - params.from - i * params.chunk_size);
			std::string line;
			int count2 = 0;
			while (std::getline(in, line)) {
				if (line.back() == '\n') line.pop_back();
				graphLines.push_back(line);
				++count2;
				if (count2 >= upperLimit) break;
			}
		}

		for (auto &line : graphLines) {
			#pragma omp atomic
			count++;

			if (params.report_progress && ((count - params.from) % params.progress_interval == 0)) {
				std::cout << "====== processing graph "
						<< (count - params.from) << "/" << totalGraphs << " (estimated)" << std::endl;
			}

			Factory localF;
			Factory &f = (params.factory == NULL) ? localF : *params.factory;
			Graph G = read_graph6_line(line, f);
			callback(line, G, f, callback_param);
		}
	}
}

// chunks go to separate threads
void read_graph6_stream_chunks(std::istream &in, ReadGraphsParams params,
		std::function<void(std::string &sG, Graph &G, Factory &f)> callback)
{
	auto cb = [&](std::string &sG, Graph &G, Factory &f, void *) { callback(sG, G, f); };
	read_graph6_stream_chunks<void>(in, params, cb, 0);
}

template <typename P>
void read_graph6_file_chunks(std::string filename, ReadGraphsParams params,
		std::function<void(std::string &sG, Graph &G, Factory &f, P *param)> callback, P *callback_param)
{
	params.graph_count = read_graph6_file_count(filename, params.from, params.to);
	std::ifstream fin(filename);
	if (!fin)
		throw std::runtime_error("cannot read file " + filename);
	read_graph6_stream_chunks(fin, params, callback, callback_param);
}

void read_graph6_file_chunks(std::string filename, ReadGraphsParams params,
		std::function<void(std::string &sG, Graph &G, Factory &f)> callback)
{
	params.graph_count = read_graph6_file_count(filename, params.from, params.to);
	std::ifstream fin(filename);
	if (!fin)
		throw std::runtime_error("cannot read file " + filename);
	read_graph6_stream_chunks(fin, params, callback);
}
} // namespace ba_graph

#endif
