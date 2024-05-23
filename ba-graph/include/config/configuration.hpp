#ifndef BA_GRAPH_CONFIG_CONFIGURATION_HPP
#define BA_GRAPH_CONFIG_CONFIGURATION_HPP

#include "../util/json.hpp"

#include <fstream>

using namespace nlohmann;


namespace ba_graph {


class Configuration {
public:
	json cfg;

	std::runtime_error configurationError = std::runtime_error("configuration error");

	void load_from_file(const std::string &fileName) {
		std::ifstream in(fileName);
		if (!in)
			throw std::runtime_error("cannot read configuration file '" + fileName + "'");
		in >> cfg;
	}

	void load_from_string(const std::string &jsonString) {
		cfg = json::parse(jsonString);
	}

	void load_from_json(json &newCfg) {
		cfg = newCfg;
	}

	const std::string STORAGE = "storage";
	const std::string DIR = "dir";

	std::string storage_dir() const {
		try {
			return cfg[STORAGE][DIR];
		} catch(...) {
			throw configurationError;
		}
	}

	const std::string RUNTIME_SETTINGS = "runtime_settings";
	const std::string TMP_DIR = "tmp_dir";

	std::string tmp_dir() const {
		try {
			return cfg[RUNTIME_SETTINGS][TMP_DIR];
		} catch(...) {
			throw configurationError;
		}
	}

	const std::string NUM_THREADS = "num_threads";

	int num_threads() const {
		try {
			std::string s = cfg[RUNTIME_SETTINGS][NUM_THREADS];
			return std::stoi(s);
		} catch(...) {
			throw configurationError;
		}
	}

	const std::string SAT_SOLVERS = "sat_solvers";
	const std::string EXECUTE_COMMAND = "execute_command";
	const std::string OUTPUT_TYPE = "output_type";

	std::string sat_solver_command(int i) const {
#ifdef BA_GRAPH_DEBUG
		assert(i >= 0);
		assert(i < (int)cfg[SAT_SOLVERS].size());
		assert(cfg[SAT_SOLVERS][i].find(EXECUTE_COMMAND) != cfg[SAT_SOLVERS][i].end());
		assert(cfg[SAT_SOLVERS][i][EXECUTE_COMMAND].is_string());
#endif
		return cfg[SAT_SOLVERS][i][EXECUTE_COMMAND];
	}

	std::string sat_solver_output_type(int i) const {
#ifdef BA_GRAPH_DEBUG
		assert(i >= 0);
		assert(i < (int)cfg[SAT_SOLVERS].size());
		assert(cfg[SAT_SOLVERS][i].find(OUTPUT_TYPE) != cfg[SAT_SOLVERS][i].end());
		assert(cfg[SAT_SOLVERS][i][OUTPUT_TYPE].is_string());
#endif
		return cfg[SAT_SOLVERS][i][OUTPUT_TYPE];
	}

	const std::string ALLSAT_SOLVERS = "allsat_solvers";

	std::string allsat_solver_command(int i) const {
#ifdef BA_GRAPH_DEBUG
		assert(i >= 0);
		assert(i < (int)cfg[ALLSAT_SOLVERS].size());
		assert(cfg[ALLSAT_SOLVERS][i].find(EXECUTE_COMMAND) != cfg[ALLSAT_SOLVERS][i].end());
		assert(cfg[ALLSAT_SOLVERS][i][EXECUTE_COMMAND].is_string());
#endif
		return cfg[ALLSAT_SOLVERS][i][EXECUTE_COMMAND];
	}

	std::string allsat_solver_output_type(int i) const {
#ifdef BA_GRAPH_DEBUG
		assert(i >= 0);
		assert(i < (int)cfg[ALLSAT_SOLVERS].size());
		assert(cfg[ALLSAT_SOLVERS][i].find(OUTPUT_TYPE) != cfg[ALLSAT_SOLVERS][i].end());
		assert(cfg[ALLSAT_SOLVERS][i][OUTPUT_TYPE].is_string());
#endif
		return cfg[ALLSAT_SOLVERS][i][OUTPUT_TYPE];
	}
};

} // namespace end

#endif
