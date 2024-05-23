#ifndef BA_GRAPH_GRAPHS_STORED_GRAPHS_HPP
#define BA_GRAPH_GRAPHS_STORED_GRAPHS_HPP

#include <cassert>
#include <climits>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <map>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

#include "../config/configuration.hpp"
#include "../io/graph6.hpp"


namespace ba_graph {

namespace internal {

inline std::string stored_cubic_dir(int cc, int g)
{
	std::stringstream ss;
	ss << "cubic/";
	ss << std::setfill('0');
	ss << "cubic_cc" << std::setw(2) << cc;
	ss << "_g" << std::setw(2) << g;
    return ss.str();
}

inline std::string stored_cubic_file(int cc, int g, int order)
{
	std::stringstream ss;
	ss << std::setfill('0');
	ss << "cubic_cc" << std::setw(2) << cc;
	ss << "_g" << std::setw(2) << g;
	ss << "." << std::setw(2) << order;
    return ss.str();
}

inline std::string stored_cubic_path(int cc, int g, int order)
{
	return stored_cubic_dir(cc, g) + "/" + stored_cubic_file(cc, g, order);
}

inline std::string stored_snark_dir(int cc, int g)
{
	std::stringstream ss;
	ss << "snarks/";
	ss << std::setfill('0');
	ss << "cubic_cc" << std::setw(2) << cc;
	ss << "_g" << std::setw(2) << g;
	ss << ".snark";
    return ss.str();
}

inline std::string stored_snark_file(int cc, int g, int order)
{
	std::stringstream ss;
	ss << stored_cubic_file(cc, g, order);
	ss << ".snark";
	return ss.str();
}

inline std::string stored_snark_path(int cc, int g, int order)
{
	return stored_snark_dir(cc, g) + "/" + stored_snark_file(cc, g, order);
}

}

class StorageUnavailableError : public std::runtime_error
{
public:
	StorageUnavailableError(std::string msg) : std::runtime_error(msg) {}
};

class GraphStorageData
{
public:
	virtual ~GraphStorageData() {};
	virtual std::string name() = 0;
	virtual const std::map<int, int> &get_graphs_counts() = 0;
	virtual const std::map<int, std::string> &get_filenames() = 0;
};

// supports graph6 and sparse6
class StoredGraphs
{
public:
	std::string storageDir;
	std::unique_ptr<GraphStorageData> storageData;

	int useCache;
	std::map<int, std::vector<std::string>> cache;
	int fileLimit;

	template <typename StorageDataT>
	static std::unique_ptr<StoredGraphs> create(const std::string &storageDir,
			bool useCache = true, int fileLimit = INT_MAX)
	{
		return std::make_unique<StoredGraphs>(storageDir, std::make_unique<StorageDataT>(), useCache, fileLimit);
	}

	template <typename StorageDataT>
	static std::unique_ptr<StoredGraphs> create(const Configuration &cfg,
			bool useCache = true, int fileLimit = INT_MAX)
	{
		return std::make_unique<StoredGraphs>(cfg, std::make_unique<StorageDataT>(), useCache, fileLimit);
	}

	StoredGraphs(const std::string &storageDir, std::unique_ptr<GraphStorageData> storageData,
			bool useCache = true, int fileLimit = INT_MAX) :
			storageDir(storageDir), storageData(std::move(storageData)),
			useCache(useCache), fileLimit(fileLimit) {}

	StoredGraphs(const Configuration &cfg, std::unique_ptr<GraphStorageData> storageData,
			bool useCache = true, int fileLimit = INT_MAX) :
			storageDir(cfg.storage_dir()), storageData(std::move(storageData)),
			useCache(useCache), fileLimit(fileLimit) {}

	inline int maxOrder()
	{
		int maxOrder = -1;
		for (auto &p : storageData->get_graphs_counts())
			maxOrder = std::max(maxOrder, p.first);

		if (maxOrder == -1)
			throw std::logic_error("suspicious graph storage: no orders given");
		return maxOrder;
	}

	inline void checkOrder(int order)
	{
		if (order <= 0 || order > maxOrder())
			throw StorageUnavailableError("order " + std::to_string(order) + " not available at " + storageData->name());
		for (auto &p : storageData->get_graphs_counts()) {
			if (p.first == order)
				return;
		}
		std::string msg = "order " + std::to_string(order) + " not available at " + storageData->name();
#ifdef BA_GRAPH_DEBUG
		std::cerr << msg << std::endl << std::flush;
#else
		throw StorageUnavailableError(msg);
#endif
	}

	inline void checkFile(int order)
	{
#ifdef BA_GRAPH_DEBUG
		checkOrder(order);
#endif
		std::ifstream in(storage_filename(order));
		if (!in)
			throw StorageUnavailableError("stored graphs file missing: '" + storage_filename(order) + "'");
	}

	inline std::string storage_filename(int order)
	{
#ifdef BA_GRAPH_DEBUG
		checkOrder(order);
#endif
		std::stringstream ss;
		ss << storageDir;
		if (storageDir.back() != std::filesystem::path::preferred_separator)
			ss << std::filesystem::path::preferred_separator;
		auto filenames = storageData->get_filenames();
		if (filenames.count(order) == 0)
			throw StorageUnavailableError("file not available for order " + std::to_string(order) + " at " + storageData->name());
		ss << storageData->get_filenames().at(order);
		return ss.str();
	}

	int get_graphs_count(int order)
	{
#ifdef BA_GRAPH_DEBUG
		checkOrder(order);
#endif
		auto counts = storageData->get_graphs_counts();
		return (counts.count(order) > 0) ? counts.at(order) : 0;
	}

	inline MinimizedGraphs get_graphs(int order, Factory &f = static_factory,
			int from = 0, int to = INT_MAX, int step = 1)
	{
#ifdef BA_GRAPH_DEBUG
		checkFile(order);
#endif
		try {
			return read_graph6_file(storage_filename(order), f, from, to, step);
		} catch (const StorageUnavailableError &) {
			return MinimizedGraphs(f);
		}
	}

	inline Graph get_graph(int order, int index, Factory &f = static_factory)
	{
#ifdef BA_GRAPH_DEBUG
		checkFile(order);
		assert(index >= 0);
		assert(index < storageData->get_graphs_counts().at(order));
#endif
		if (useCache) {
			if (cache.count(order) == 0)
				loadCache(order);
#ifdef BA_GRAPH_DEBUG
			assert((int)cache[order].size() > index);
#endif
			return read_graph6_line(cache[order][index], f);
		} else {
			auto graphs = read_graph6_file(storage_filename(order), f, index, index + 1).graphs();
#ifdef BA_GRAPH_DEBUG
			assert(graphs.size() > 0);
#endif
			return std::move(graphs[0]);
		}
	}

	template <typename P>
	void enumerate_graphs(int order, std::function<void(std::string &, Graph &, Factory &, P *param)> callback, P *callbackParam,
			Factory *f = &static_factory, int from = 0, int to = INT_MAX, int step = 1)
	{
		try {
			auto s = storage_filename(order);
			read_graph6_file<P>(s, callback, callbackParam, f, from, to, step);
		} catch (const StorageUnavailableError &) {
			return;
		}
	}

	Graph getRandomGraph(int order, Factory &f = static_factory)
	{
#ifdef BA_GRAPH_DEBUG
		checkFile(order);
#endif
		int index = rand() % std::min(storageData->get_graphs_counts().at(order), fileLimit);
		return get_graph(order, index, f);
	}

private:
	void loadCache(int order)
	{
		std::vector<std::string> lines;
		std::ifstream fin(storage_filename(order));
		if (!fin) {
			std::stringstream msg; msg << "cannot read from file " << storage_filename(order);
			throw std::runtime_error(msg.str());
		}
		for (int i = 0; i < fileLimit; ++i) {
			std::string line;
			if (!std::getline(fin, line))
				break;
			lines.push_back(line);
		}
		cache[order] = lines;
	}
};

/* ============================================
 *       cubic graphs, cc = 1
 * ============================================
 */

class CubicStorageDataC1G1 : public GraphStorageData
{
private:
	const std::map<int, int> graphs_counts = { {2, 2}, {4, 5}, {6, 17}, {8, 71}, {10, 388}, {12, 2592},
			{14, 21096}, {16, 204638}, {18, 2317172}, {20, 30024276}, {22, 437469859} };

	const std::map<int, std::string> fileNames = {
			{ 2, internal::stored_cubic_path(1, 1, 2)},
			{ 4, internal::stored_cubic_path(1, 1, 4)},
			{ 6, internal::stored_cubic_path(1, 1, 6)},
			{ 8, internal::stored_cubic_path(1, 1, 8)},
			{10, internal::stored_cubic_path(1, 1, 10)},
			{12, internal::stored_cubic_path(1, 1, 12)},
			{14, internal::stored_cubic_path(1, 1, 14)},
			{16, internal::stored_cubic_path(1, 1, 16)},
			{18, internal::stored_cubic_path(1, 1, 18)},
			{20, internal::stored_cubic_path(1, 1, 20)},
			{22, internal::stored_cubic_path(1, 1, 22)}
	};

public:
	std::string name() { return "CubicStorageDataC1G1"; }
	const std::map<int, int> & get_graphs_counts() { return graphs_counts; }
	const std::map<int, std::string> & get_filenames() { return fileNames; }
};

class CubicStorageDataC1G2 : public GraphStorageData
{
private:
	const std::map<int, int> graphs_counts = { {2, 1}, {4, 2}, {6, 6}, {8, 20},
			{10, 91}, {12, 509}, {14, 3608}, {16, 31856}, {18, 340416},
			{20, 4269971}, {22, 61133757}, {24, 978098997} };

	const std::map<int, std::string> fileNames = {
			{ 2, internal::stored_cubic_path(1, 2, 2)},
			{ 4, internal::stored_cubic_path(1, 2, 4)},
			{ 6, internal::stored_cubic_path(1, 2, 6)},
			{ 8, internal::stored_cubic_path(1, 2, 8)},
			{10, internal::stored_cubic_path(1, 2, 10)},
			{12, internal::stored_cubic_path(1, 2, 12)},
			{14, internal::stored_cubic_path(1, 2, 14)},
			{16, internal::stored_cubic_path(1, 2, 16)},
			{18, internal::stored_cubic_path(1, 2, 18)},
			{20, internal::stored_cubic_path(1, 2, 20)},
			{22, internal::stored_cubic_path(1, 2, 22)},
			{24, internal::stored_cubic_path(1, 2, 24)}
	};

public:
	std::string name() { return "CubicStorageDataC1G2"; }
	const std::map<int, int> & get_graphs_counts() { return graphs_counts; }
	const std::map<int, std::string> & get_filenames() { return fileNames; }
};

class CubicStorageDataC1G3 : public GraphStorageData
{
private:
	const std::map<int, int> graphs_counts = { {4, 1}, {6, 2}, {8, 5}, {10, 19},
			{12, 85}, {14, 509}, {16, 4060}, {18, 41301}, {20, 510489},
			{22, 7319447}, {24, 117940535}};

	const std::map<int, std::string> fileNames = {
			{ 4, internal::stored_cubic_path(1, 3, 4)},
			{ 6, internal::stored_cubic_path(1, 3, 6)},
			{ 8, internal::stored_cubic_path(1, 3, 8)},
			{10, internal::stored_cubic_path(1, 3, 10)},
			{12, internal::stored_cubic_path(1, 3, 12)},
			{14, internal::stored_cubic_path(1, 3, 14)},
			{16, internal::stored_cubic_path(1, 3, 16)},
			{18, internal::stored_cubic_path(1, 3, 18)},
			{20, internal::stored_cubic_path(1, 3, 20)},
			{22, internal::stored_cubic_path(1, 3, 22)},
			{24, internal::stored_cubic_path(1, 3, 24)}
	};

public:
	std::string name() { return "CubicStorageDataC1G3"; }
	const std::map<int, int> & get_graphs_counts() { return graphs_counts; }
	const std::map<int, std::string> & get_filenames() { return fileNames; }
};

class CubicStorageDataC1G4 : public GraphStorageData
{
private:
	const std::map<int, int> graphs_counts = { {6, 1}, {8, 2}, {10, 6},
			{12, 22}, {14, 110}, {16, 792}, {18, 7805}, {20, 97546},
			{22, 1435720}, {24, 23780814}};

	const std::map<int, std::string> fileNames = {
			{ 6, internal::stored_cubic_path(1, 4, 6)},
			{ 8, internal::stored_cubic_path(1, 4, 8)},
			{10, internal::stored_cubic_path(1, 4, 10)},
			{12, internal::stored_cubic_path(1, 4, 12)},
			{14, internal::stored_cubic_path(1, 4, 14)},
			{16, internal::stored_cubic_path(1, 4, 16)},
			{18, internal::stored_cubic_path(1, 4, 18)},
			{20, internal::stored_cubic_path(1, 4, 20)},
			{22, internal::stored_cubic_path(1, 4, 22)},
			{24, internal::stored_cubic_path(1, 4, 24)}
	};

public:
	std::string name() { return "CubicStorageDataC1G4"; }
	const std::map<int, int> & get_graphs_counts() { return graphs_counts; }
	const std::map<int, std::string> & get_filenames() { return fileNames; }
};

class CubicStorageDataC1G5 : public GraphStorageData
{
private:
	const std::map<int, int> graphs_counts = { {10, 1},
			{12, 2}, {14, 9}, {16, 49}, {18, 455}, {20, 5783},
			{22, 90938}, {24, 1620479}, {26, 31478584}};

	const std::map<int, std::string> fileNames = {
			{10, internal::stored_cubic_path(1, 5, 10)},
			{12, internal::stored_cubic_path(1, 5, 12)},
			{14, internal::stored_cubic_path(1, 5, 14)},
			{16, internal::stored_cubic_path(1, 5, 16)},
			{18, internal::stored_cubic_path(1, 5, 18)},
			{20, internal::stored_cubic_path(1, 5, 20)},
			{22, internal::stored_cubic_path(1, 5, 22)},
			{24, internal::stored_cubic_path(1, 5, 24)},
			{26, internal::stored_cubic_path(1, 5, 26)}
	};

public:
	std::string name() { return "CubicStorageDataC1G5"; }
	const std::map<int, int> & get_graphs_counts() { return graphs_counts; }
	const std::map<int, std::string> & get_filenames() { return fileNames; }
};

class CubicStorageDataC1G6 : public GraphStorageData
{
private:
	const std::map<int, int> graphs_counts = {
			{14, 1}, {16, 1}, {18, 5}, {20, 32},
			{22, 385}, {24, 7574}, {26, 181227}, {28, 4624501}};

	const std::map<int, std::string> fileNames = {
			{14, internal::stored_cubic_path(1, 6, 14)},
			{16, internal::stored_cubic_path(1, 6, 16)},
			{18, internal::stored_cubic_path(1, 6, 18)},
			{20, internal::stored_cubic_path(1, 6, 20)},
			{22, internal::stored_cubic_path(1, 6, 22)},
			{24, internal::stored_cubic_path(1, 6, 24)},
			{26, internal::stored_cubic_path(1, 6, 26)},
			{28, internal::stored_cubic_path(1, 6, 28)}
	};

public:
	std::string name() { return "CubicStorageDataC1G6"; }
	const std::map<int, int> & get_graphs_counts() { return graphs_counts; }
	const std::map<int, std::string> & get_filenames() { return fileNames; }
};

class CubicStorageDataC1G7 : public GraphStorageData
{
private:
	const std::map<int, int> graphs_counts = {
			{24, 1}, {26, 3}, {28, 21}, {30, 546},
			{32, 30368}, {34, 1782840}, {36, 95079083} };

	const std::map<int, std::string> fileNames = {
			{24, internal::stored_cubic_path(1, 7, 24)},
			{26, internal::stored_cubic_path(1, 7, 26)},
			{28, internal::stored_cubic_path(1, 7, 28)},
			{30, internal::stored_cubic_path(1, 7, 30)},
			{32, internal::stored_cubic_path(1, 7, 32)},
			{34, internal::stored_cubic_path(1, 7, 34)},
			{36, internal::stored_cubic_path(1, 7, 36)}
	};

public:
	std::string name() { return "CubicStorageDataC1G7"; }
	const std::map<int, int> & get_graphs_counts() { return graphs_counts; }
	const std::map<int, std::string> & get_filenames() { return fileNames; }
};

class CubicStorageDataC1G8 : public GraphStorageData
{
private:
	const std::map<int, int> graphs_counts = {
			{30, 1}, {34, 1}, {36, 3}, {38, 13}, {40, 155},
			{42, 4337}, {44, 266362}, {46, 20807688	} };

	const std::map<int, std::string> fileNames = {
			{30, internal::stored_cubic_path(1, 8, 30)},
			{34, internal::stored_cubic_path(1, 8, 34)},
			{36, internal::stored_cubic_path(1, 8, 36)},
			{38, internal::stored_cubic_path(1, 8, 38)},
			{40, internal::stored_cubic_path(1, 8, 40)},
			{42, internal::stored_cubic_path(1, 8, 42)},
			{44, internal::stored_cubic_path(1, 8, 44)},
			{46, internal::stored_cubic_path(1, 8, 46)}
	};

public:
	std::string name() { return "CubicStorageDataC1G8"; }
	const std::map<int, int> & get_graphs_counts() { return graphs_counts; }
	const std::map<int, std::string> & get_filenames() { return fileNames; }
};

class CubicStorageDataC1G9 : public GraphStorageData
{
private:
	const std::map<int, int> graphs_counts = {
			{58, 18}, {60, 474}, {62, 27169}, {64, 1408813} };

	const std::map<int, std::string> fileNames = {
			{58, internal::stored_cubic_path(1, 9, 58)},
			{60, internal::stored_cubic_path(1, 9, 60)},
			{62, internal::stored_cubic_path(1, 9, 62)},
			{64, internal::stored_cubic_path(1, 9, 64)}
	};

public:
	std::string name() { return "CubicStorageDataC1G9"; }
	const std::map<int, int> & get_graphs_counts() { return graphs_counts; }
	const std::map<int, std::string> & get_filenames() { return fileNames; }
};

/* ============================================
 *       cubic graphs, cc = 2
 * ============================================
 */

class CubicStorageDataC2G2 : public GraphStorageData
{
private:
	const std::map<int, int> graphs_counts = { {2, 1}, {4, 2}, {6, 5}, {8, 16},
			{10, 66}, {12, 365}, {14, 2602}, {16, 23811}, {18, 264993},
			{20, 3449683} };

	const std::map<int, std::string> fileNames = {
			{ 2, internal::stored_cubic_path(2, 2, 2)},
			{ 4, internal::stored_cubic_path(2, 2, 4)},
			{ 6, internal::stored_cubic_path(2, 2, 6)},
			{ 8, internal::stored_cubic_path(2, 2, 8)},
			{10, internal::stored_cubic_path(2, 2, 10)},
			{12, internal::stored_cubic_path(2, 2, 12)},
			{14, internal::stored_cubic_path(2, 2, 14)},
			{16, internal::stored_cubic_path(2, 2, 16)},
			{18, internal::stored_cubic_path(2, 2, 18)},
			{20, internal::stored_cubic_path(2, 2, 20)}
	};

public:
	std::string name() { return "CubicStorageDataC2G2"; }
	const std::map<int, int> & get_graphs_counts() { return graphs_counts; }
	const std::map<int, std::string> & get_filenames() { return fileNames; }
};

/* ============================================
 *       cubic graphs, cc = 3
 * ============================================
 */

class CubicStorageDataC3G3 : public GraphStorageData
{
private:
	const std::map<int, int> graphs_counts = { {4, 1}, {6, 2}, {8, 4},
			{10, 14}, {12, 57}, {14, 341}, {16, 2828}, {18, 30468}, {20, 396150},
			{22, 5909292} };

	const std::map<int, std::string> fileNames = {
			{ 4, internal::stored_cubic_path(3, 3, 4)},
			{ 6, internal::stored_cubic_path(3, 3, 6)},
			{ 8, internal::stored_cubic_path(3, 3, 8)},
			{10, internal::stored_cubic_path(3, 3, 10)},
			{12, internal::stored_cubic_path(3, 3, 12)},
			{14, internal::stored_cubic_path(3, 3, 14)},
			{16, internal::stored_cubic_path(3, 3, 16)},
			{18, internal::stored_cubic_path(3, 3, 18)},
			{20, internal::stored_cubic_path(3, 3, 20)},
			{22, internal::stored_cubic_path(3, 3, 22)}
	};

public:
	std::string name() { return "CubicStorageDataC3G3"; }
	const std::map<int, int> & get_graphs_counts() { return graphs_counts; }
	const std::map<int, std::string> & get_filenames() { return fileNames; }
};


/* ============================================
 *       snarks, cc = 1
 * ============================================
 */

class SnarkStorageDataC1G1 : public GraphStorageData
{
private:
	const std::map<int, int> graphs_counts = {
			{2, 1}, {4, 3}, {6, 12}, {8, 55}, {10, 323}, {12, 2229},
			{14, 18508}, {16, 180936}, {18, 2053220},
			{20, 26585634}, {22, 386706357} };

	const std::map<int, std::string> fileNames = {
			{ 2, internal::stored_snark_path(1, 1,  2)},
			{ 4, internal::stored_snark_path(1, 1,  4)},
			{ 6, internal::stored_snark_path(1, 1,  6)},
			{ 8, internal::stored_snark_path(1, 1,  8)},
			{10, internal::stored_snark_path(1, 1, 10)},
			{12, internal::stored_snark_path(1, 1, 12)},
			{14, internal::stored_snark_path(1, 1, 14)},
			{16, internal::stored_snark_path(1, 1, 16)},
			{18, internal::stored_snark_path(1, 1, 18)},
			{20, internal::stored_snark_path(1, 1, 20)},
			{22, internal::stored_snark_path(1, 1, 22)}
	};

public:
	std::string name() { return "SnarkStorageDataC1G1"; }
	const std::map<int, int> & get_graphs_counts() { return graphs_counts; }
	const std::map<int, std::string> & get_filenames() { return fileNames; }
};

class SnarkStorageDataC1G2 : public GraphStorageData
{
private:
	const std::map<int, int> graphs_counts = {
			{6, 1}, {8, 4}, {10, 26}, {12, 146},
			{14, 1020}, {16, 8154}, {18, 76464},
			{20, 831329}, {22, 10370255}, {24, 146200420} };

	const std::map<int, std::string> fileNames = {
			{ 6, internal::stored_snark_path(1, 2,  6)},
			{ 8, internal::stored_snark_path(1, 2,  8)},
			{10, internal::stored_snark_path(1, 2, 10)},
			{12, internal::stored_snark_path(1, 2, 12)},
			{14, internal::stored_snark_path(1, 2, 14)},
			{16, internal::stored_snark_path(1, 2, 16)},
			{18, internal::stored_snark_path(1, 2, 18)},
			{20, internal::stored_snark_path(1, 2, 20)},
			{22, internal::stored_snark_path(1, 2, 22)},
			{24, internal::stored_snark_path(1, 2, 24)}
	};

public:
	std::string name() { return "SnarkStorageDataC1G2"; }
	const std::map<int, int> & get_graphs_counts() { return graphs_counts; }
	const std::map<int, std::string> & get_filenames() { return fileNames; }
};

class SnarkStorageDataC1G3 : public GraphStorageData
{
private:
	const std::map<int, int> graphs_counts = {
			{10, 2}, {12, 5}, {14, 34}, {16, 212}, {18, 1614}, {20, 14059},
			{22, 144712}, {24, 1726497} };

	const std::map<int, std::string> fileNames = {
			{10, internal::stored_snark_path(1, 3, 10)},
			{12, internal::stored_snark_path(1, 3, 12)},
			{14, internal::stored_snark_path(1, 3, 14)},
			{16, internal::stored_snark_path(1, 3, 16)},
			{18, internal::stored_snark_path(1, 3, 18)},
			{20, internal::stored_snark_path(1, 3, 20)},
			{22, internal::stored_snark_path(1, 3, 22)},
			{24, internal::stored_snark_path(1, 3, 24)}
	};

public:
	std::string name() { return "SnarkStorageDataC1G3"; }
	const std::map<int, int> & get_graphs_counts() { return graphs_counts; }
	const std::map<int, std::string> & get_filenames() { return fileNames; }
};

/* ============================================
 *       snarks, cc = 2
 * ============================================
 */

class SnarkStorageDataC2G2 : public GraphStorageData
{
private:
	const std::map<int, int> graphs_counts = {
			{10, 1}, {12, 2}, {14, 14}, {16, 109}, {18, 1041},
			{20, 11041}, {22, 128113}, {24, 1579663} };

	const std::map<int, std::string> fileNames = {
			{10, internal::stored_snark_path(2, 2, 10)},
			{12, internal::stored_snark_path(2, 2, 12)},
			{14, internal::stored_snark_path(2, 2, 14)},
			{16, internal::stored_snark_path(2, 2, 16)},
			{18, internal::stored_snark_path(2, 2, 18)},
			{20, internal::stored_snark_path(2, 2, 20)},
			{22, internal::stored_snark_path(2, 2, 22)},
			{24, internal::stored_snark_path(2, 2, 24)}
	};

public:
	std::string name() { return "SnarkStorageDataC2G2"; }
	const std::map<int, int> & get_graphs_counts() { return graphs_counts; }
	const std::map<int, std::string> & get_filenames() { return fileNames; }
};

class SnarkStorageDataC2G3 : public GraphStorageData
{
private:
	const std::map<int, int> graphs_counts = {
			{10, 1}, {12, 1}, {14, 5}, {16, 26}, {18, 179}, {20, 1388},
			{22, 12892}, {24, 135597}, {26, 1678744} };

	const std::map<int, std::string> fileNames = {
			{10, internal::stored_snark_path(2, 3, 10)},
			{12, internal::stored_snark_path(2, 3, 12)},
			{14, internal::stored_snark_path(2, 3, 14)},
			{16, internal::stored_snark_path(2, 3, 16)},
			{18, internal::stored_snark_path(2, 3, 18)},
			{20, internal::stored_snark_path(2, 3, 20)},
			{22, internal::stored_snark_path(2, 3, 22)},
			{24, internal::stored_snark_path(2, 3, 24)},
			{26, internal::stored_snark_path(2, 3, 26)}
	};

public:
	std::string name() { return "SnarkStorageDataC2G3"; }
	const std::map<int, int> & get_graphs_counts() { return graphs_counts; }
	const std::map<int, std::string> & get_filenames() { return fileNames; }
};

/* ============================================
 *       snarks, cc = 3
 * ============================================
 */

class SnarkStorageDataC3G3 : public GraphStorageData
{
private:
	const std::map<int, int> graphs_counts = {
			{10, 1}, {12, 1}, {14, 4}, {16, 18}, {18, 114}, {20, 847},
			{22, 7868}, {24, 84682}, {26, 1044256}, {28, 15391409 },
			{30, 236559454} };

	const std::map<int, std::string> fileNames = {
			{10, internal::stored_snark_path(3, 3, 10)},
			{12, internal::stored_snark_path(3, 3, 12)},
			{14, internal::stored_snark_path(3, 3, 14)},
			{16, internal::stored_snark_path(3, 3, 16)},
			{18, internal::stored_snark_path(3, 3, 18)},
			{20, internal::stored_snark_path(3, 3, 20)},
			{22, internal::stored_snark_path(3, 3, 22)},
			{24, internal::stored_snark_path(3, 3, 24)},
			{26, internal::stored_snark_path(3, 3, 26)},
			{28, internal::stored_snark_path(3, 3, 28)},
			{30, internal::stored_snark_path(3, 3, 30)}
	};

public:
	std::string name() { return "SnarkStorageDataC3G3"; }
	const std::map<int, int> & get_graphs_counts() { return graphs_counts; }
	const std::map<int, std::string> & get_filenames() { return fileNames; }
};

/* ============================================
 *       snarks, cc = 4
 * ============================================
 */

class SnarkStorageDataC4G4 : public GraphStorageData
{
private:
	const std::map<int, int> graphs_counts = { {10, 1}, {18, 2}, {20, 6},
			{22, 31}, {24, 155}, {26, 1297}, {28, 12517}, {30, 139854},
			{32, 1764950}, {34, 25286953}, {36, 404899916} };

	const std::map<int, std::string> fileNames = {
			{10, internal::stored_snark_path(4, 4, 10)},
			{18, internal::stored_snark_path(4, 4, 18)},
			{20, internal::stored_snark_path(4, 4, 20)},
			{22, internal::stored_snark_path(4, 4, 22)},
			{24, internal::stored_snark_path(4, 4, 24)},
			{26, internal::stored_snark_path(4, 4, 26)},
			{28, internal::stored_snark_path(4, 4, 28)},
			{30, internal::stored_snark_path(4, 4, 30)},
			{32, internal::stored_snark_path(4, 4, 32)},
			{34, internal::stored_snark_path(4, 4, 34)},
			{36, internal::stored_snark_path(4, 4, 36)}
	};

public:
	std::string name() { return "SnarkStorageDataC4G4"; }
	const std::map<int, int> & get_graphs_counts() { return graphs_counts; }
	const std::map<int, std::string> & get_filenames() { return fileNames; }
};

class SnarkStorageDataC4G5 : public GraphStorageData
{
private:
	const std::map<int, int> graphs_counts = { {10, 1}, {18, 2}, {20, 6},
			{22, 20}, {24, 38}, {26, 280}, {28, 2900}, {30, 28399},
			{32, 293059}, {34, 3833587}, {36, 60167732} };

	const std::map<int, std::string> fileNames = {
			{10, internal::stored_snark_path(4, 5, 10)},
			{18, internal::stored_snark_path(4, 5, 18)},
			{20, internal::stored_snark_path(4, 5, 20)},
			{22, internal::stored_snark_path(4, 5, 22)},
			{24, internal::stored_snark_path(4, 5, 24)},
			{26, internal::stored_snark_path(4, 5, 26)},
			{28, internal::stored_snark_path(4, 5, 28)},
			{30, internal::stored_snark_path(4, 5, 30)},
			{32, internal::stored_snark_path(4, 5, 32)},
			{34, internal::stored_snark_path(4, 5, 34)},
			{36, internal::stored_snark_path(4, 5, 36)}
	};

public:
	std::string name() { return "SnarkStorageDataC4G5"; }
	const std::map<int, int> & get_graphs_counts() { return graphs_counts; }
	const std::map<int, std::string> & get_filenames() { return fileNames; }
};

class SnarkStorageDataC4G6 : public GraphStorageData
{
private:
	const std::map<int, int> graphs_counts = { {28, 1}, {30, 1}, {36, 1},
			{38, 39}, {40, 25} };

	const std::map<int, std::string> fileNames = {
			{28, internal::stored_snark_path(4, 6, 28)},
			{30, internal::stored_snark_path(4, 6, 30)},
			{36, internal::stored_snark_path(4, 6, 36)},
			{38, internal::stored_snark_path(4, 6, 38)},
			{40, internal::stored_snark_path(4, 6, 40) + ".some"}
	};

public:
	std::string name() { return "SnarkStorageDataC4G6"; }
	const std::map<int, int> & get_graphs_counts() { return graphs_counts; }
	const std::map<int, std::string> & get_filenames() { return fileNames; }
};


/* ============================================
 *       snarks, cc = 5
 * ============================================
 */

class SnarkStorageDataC5G5 : public GraphStorageData
{
private:
	const std::map<int, int> graphs_counts = { {10, 1}, {20, 1}, {22, 2},
			{24, 2}, {26, 10}, {28, 75}, {30, 509},
			{32, 2953}, {34, 19935}, {36, 180612}, {38, 35429} };

	const std::map<int, std::string> fileNames = {
			{10, internal::stored_snark_path(5, 5, 10)},
			{20, internal::stored_snark_path(5, 5, 20)},
			{22, internal::stored_snark_path(5, 5, 22)},
			{24, internal::stored_snark_path(5, 5, 24)},
			{26, internal::stored_snark_path(5, 5, 26)},
			{28, internal::stored_snark_path(5, 5, 28)},
			{30, internal::stored_snark_path(5, 5, 30)},
			{32, internal::stored_snark_path(5, 5, 32)},
			{34, internal::stored_snark_path(5, 5, 34)},
			{36, internal::stored_snark_path(5, 5, 36)},
			{38, internal::stored_snark_path(5, 5, 36) + ".some"}
	};

public:
	std::string name() { return "SnarkStorageDataC5G5"; }
	const std::map<int, int> & get_graphs_counts() { return graphs_counts; }
	const std::map<int, std::string> & get_filenames() { return fileNames; }
};

class SnarkStorageDataC5G6 : public GraphStorageData
{
private:
	const std::map<int, int> graphs_counts = { {28, 1}, {30, 1}, {36, 1} };

	const std::map<int, std::string> fileNames = {
			{28, internal::stored_snark_path(5, 6, 28)},
			{30, internal::stored_snark_path(5, 6, 30)},
			{36, internal::stored_snark_path(5, 6, 36)}
	};
public:
	std::string name() { return "SnarkStorageDataC5G6"; }
	const std::map<int, int> & get_graphs_counts() { return graphs_counts; }
	const std::map<int, std::string> & get_filenames() { return fileNames; }
};

} // namespace ba_graph

#endif
