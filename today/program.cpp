#include <vector>
#include <chrono>
#include <filesystem>
#include <format>
#include <print>
#include <functional>

#include <common.h>
i32 main(const i32 argc, char** argv) {
    std::vector<char*> args;
    for (u32 arg{1}; arg < argc; arg++)
        args.push_back(argv[arg]);

    for (const auto pathDir : args) {
        std::filesystem::path path(pathDir);
        if (!is_directory(path))
            std::terminate();

        const std::chrono::time_point epoch{std::chrono::system_clock::now()};
        std::string today{std::format("{}", epoch)};
        today = today.substr(0, today.find_last_of(' '));

        const std::function<void(const std::filesystem::path& path)> walkPath = [&](const auto& directory) {
            const std::filesystem::recursive_directory_iterator walker(directory);
            const std::filesystem::recursive_directory_iterator end;

            if (walker != end) {
                for (const auto& inPath : walker) {
                    if (is_directory(inPath)) {
                        walkPath(inPath);
                        continue;
                    }
                    if (!is_regular_file(inPath)) {
                        std::terminate();
                    }
                    const std::string create{std::format("{}", last_write_time(inPath))};

                    if (create.contains(today.substr(0, 11))) {
                        std::print("{}: {}\n", create, inPath.path().string());
                    }
                }
            }
        };
        walkPath(path);
    }
}
