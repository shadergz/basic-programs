#include <filesystem>
#include <print>
#include <vector>
#include <fstream>

#include <common.h>
extern void programEntry(std::fstream& io, const std::filesystem::path& path);

i32 main(const i32 argc, char** argv) {
    std::vector<char*> args;
    for (i32 arg = 1; arg < argc; arg++) {
        args.push_back(argv[arg]);
    }
    for (const auto argument : args) {
        std::filesystem::path path(argument);
        if (!std::filesystem::exists(argument)) {
            std::print("The file '{}' does not exist or is not a regular file\n", argument);
            std::terminate();
        }
        // ReSharper disable once CppTooWideScopeInitStatement
        std::fstream io;
        io.open(path, std::ios::binary | std::ios::in);
        if (io.is_open()) {
            programEntry(io, path);
            io.close();
        }
    }
}
