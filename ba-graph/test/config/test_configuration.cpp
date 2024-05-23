#include "implementation.h"

#include <config/configuration.hpp>

using namespace ba_graph;

int main()
{
    Configuration cfg;
    cfg.load_from_file("ba_graph.cfg");

    assert(cfg.storage_dir() == "/home/vyskum_data");
    assert(cfg.tmp_dir() == "/tmp");
    assert(cfg.num_threads() == 4);
}
