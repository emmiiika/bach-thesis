#include <sstream>
#include <iostream>
#include <climits>

#include <impl/basic/include.hpp>
#include <io.hpp>
#include <util/cxxopts.hpp>

using namespace ba_graph;

cxxopts::Options options("showgraph",
		"\nTransforms g6/oldBA graph representation into a pdf picture using graphviz/neato.\n");

void wrong_usage()
{
	std::cout << options.help() << std::endl;
	exit(1);
}

void missingError()
{
	fprintf(stderr, "There is no such graph in the input file\n");
	exit(1);
}

Graph getGraphStream(std::istream &in, int graphIndex)
{
	auto graphs = read_graph6_stream(in, static_factory, graphIndex, graphIndex + 1);
	if (graphs.size() == 0)
		missingError();
	return std::move(graphs[0]);
}

Graph getGraphFile(std::string fileFormat, std::string inputFileName, int graphIndex)
{
	Graph G = createG();
	if (fileFormat == "g6" || fileFormat == "s6") {
		auto graphs = read_graph6_file(inputFileName, static_factory, graphIndex, graphIndex + 1).graphs();
		if (graphs.size() == 0)
			missingError();
		G = std::move(graphs[0]);
	} else if (fileFormat == "ba") {
		auto graphs = read_oldBA_file(inputFileName);
		if (unsigned(graphIndex) >= graphs.size())
			missingError();
		G = std::move(graphs[graphIndex]);
	} else {
		wrong_usage();
	}
	return G;
}

int main(int argc, char **argv) {
	try {
		options.add_options()
			("h, help", "print help")
			("f, format", "format (g6/s6/ba)", cxxopts::value<std::string>()->default_value("g6"))
			("o, output", "output file", cxxopts::value<std::string>()->default_value("showgraph.pdf"))
			("i, input", "input file", cxxopts::value<std::string>())
			("n, index", "graph index", cxxopts::value<int>()->default_value(std::to_string(0)))
			("firstRed", "number of first red vertex", cxxopts::value<int>()->default_value(std::to_string(INT_MAX)))
			;
		auto o = options.parse(argc, argv);

		if (o.count("help")) {
			std::cout << options.help() << std::endl;
			return 0;
		}

		std::string fileFormat = o["format"].as<std::string>();
		std::string outputFileName = o["output"].as<std::string>();
		int graphIndex = o["index"].as<int>();

		Graph G = createG();
		if (o.count("input")) {
			std::string inputFileName = o["input"].as<std::string>();
			G = std::move(getGraphFile(fileFormat, inputFileName, graphIndex));
		} else {
			G = std::move(getGraphStream(std::cin, graphIndex));
		}

		int firstRedVertex = o["firstRed"].as<int>();
		std::vector<Number> redVertices;
		for (auto &rv : G) {
			if (rv.n() >= firstRedVertex)
				redVertices.push_back(rv.n());
		}

		std::stringstream command;
		auto r = internal::neato_representation(G, redVertices);
		command << "echo \"" << r << "\" | neato -Tpdf > " << outputFileName;

		if (system(command.str().c_str()) == -1)
			throw std::runtime_error("Call of system() failed");
	} catch (const cxxopts::OptionException& e) {
		std::cerr << "error parsing options: " << e.what() << std::endl;
		exit(1);
	}

	return 0;
}
