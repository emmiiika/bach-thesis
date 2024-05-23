#include <util/system.hpp>

#include <cassert>

using namespace ba_graph;

int main() {
    auto r = internal::execute_command("echo ahoj");
    assert(r == "ahoj\n");

    return 0;
}
