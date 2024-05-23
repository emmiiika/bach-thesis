#ifndef BA_GRAPH_UTIL_SYSTEM_HPP
#define BA_GRAPH_UTIL_SYSTEM_HPP

#include <filesystem>
#include <memory>
#include <stdexcept>

namespace ba_graph
{

namespace internal
{

inline std::string execute_command(const char *cmd)
{
    std::array<char, 128> buffer;
    std::string result;
    std::shared_ptr<FILE> pipe(popen(cmd, "r"), pclose);
    if (!pipe) throw std::runtime_error("popen() failed!");
    while (!feof(pipe.get())) {
        if (fgets(buffer.data(), 128, pipe.get()) != nullptr)
            result += buffer.data();
    }
    return result;
}

} // namespace internal

inline std::string random_string(int len) {
    static const char alphanum[] =
            "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";

    std::stringstream ss;
    for (int i = 0; i < len; ++i)
        ss << alphanum[rand() % (sizeof(alphanum) - 1)];
    return ss.str();
}

inline std::filesystem::path tmp_file(std::filesystem::path tmp_dir)
{
    tmp_dir /= random_string(30);
    return tmp_dir;
}

inline std::filesystem::path tmp_file(std::string tmp_dir)
{
    std::filesystem::path tmpDirPath(tmp_dir);
    return tmp_file(tmpDirPath);
}

} // namespace ba_graph

#endif
