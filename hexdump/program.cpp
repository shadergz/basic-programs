#include <print>
#include <filesystem>
#include <fstream>
#include <boost/program_options.hpp>

#include <common.h>

template <typename... Args>
void quit(const std::format_string<Args...> format, Args&&... args) {
    std::print(format, std::forward<Args>(args)...);
    std::print("\n");
    std::terminate();
}

u32 seek{};
u32 col{16};
u32 count{static_cast<u32>(-1)};
u32 style{};
u32 format{};

i32 main(const i32 argc, char** argv) {
    namespace po = boost::program_options;
    po::variables_map vm;
    po::options_description bin2("Bin2Array options");
    bin2.add_options()
        ("usage", "Display the utility help message")
        ("octal", "Show the values in octal")
        ("seek", po::value<std::string>(), "Skip N bytes from the beginning of the file")
        ("col", po::value<std::string>(), "Change the column size")
        ("count", po::value<std::string>(), "Display only N bytes")
        ("input", po::value<std::string>(), "Indicate the name of the target file")
        ("format", po::value<u32>(), "Change the display format of the interpreted bytes");

    store(parse_command_line(argc, argv, bin2), vm);
    vm.notify();
    if (vm.contains("usage")) {
        std::cout << bin2 << '\n';
        return {};
    }
    bool isHex{true};
    if (vm.contains("octal")) {
        isHex = {};
    }
    const auto getUnsignedArg = [&](u32& value, const std::string& name) {
        if (!vm.contains(name))
            return;

        std::stringstream ss;
        const auto user{vm[name].as<std::string>()};
        if (user.contains("0x")) {
            ss << std::hex << user;
        } else {
            ss << user;
        }
        ss >> value;
    };
    getUnsignedArg(seek, "seek");
    getUnsignedArg(col, "col");
    getUnsignedArg(count, "count");
    if (vm.contains("format")) {
        format = vm["format"].as<u32>();
    }

    std::filesystem::path input{};
    std::fstream stream{};

    if (vm.contains("input")) {
        input = vm["input"].as<std::string>();
    }
    if (input.empty())
        input = std::string(argv[argc - 1]);
    if (!exists(input) || input.empty())
        input = std::string(argv[0]);
    if (!input.empty()) {

        if (!exists(input))
            quit("Specified file '{}' does not exist", input.string());

        stream.open(input, std::ios_base::in | std::ios_base::binary);

        if (!stream.is_open())
            quit("Could not open the specified file");
    }

    if (col < 10 || col > 24 || (col % 2))
        quit("Invalid column size {}", col);
    if (seek)
        stream.seekg(seek);
    auto tableSize{static_cast<std::streamsize>(col * std::log10(col * 2))};

    std::vector<char> bytesTable(tableSize);
    std::vector<char> asciiTable(tableSize);

    std::streamsize read;
    std::streamsize offset{seek};
    do {
        auto block{count != -1 ? (count > col ? (col % count) ? col : count : count) : col};
        std::ranges::fill(bytesTable, '-');
        std::ranges::fill(asciiTable, '-');

        stream.read(&bytesTable[0], block);
        read = stream.gcount();
        if (!read)
            break;

        offset += read;
        const auto formatFunction = [&](const char ch) -> i32 {
            switch (format) {
                case 0:
                    return std::isprint(ch);
                case 1:
                    return std::isalnum(ch);
                case 2:
                    return std::isalpha(ch);
                case 3:
                    return std::isupper(ch);
                case 4:
                    return std::isdigit(ch);
                case 5:
                    return std::isxdigit(ch);
                default: {

                }
            }
            return ch;
        };
        decltype(read) index{};
        for (; index < col; index++) {
            if (!(index % 4))
                std::print(" ");

            if (!index)
                std::print("{:#08x}: ", offset);
            if (read > index) {
                if (isHex)
                    std::print("{:02x}", bytesTable[index]);
                else
                    std::print("{:03o}", bytesTable[index]);
                asciiTable[index] = formatFunction(bytesTable[index]) ? bytesTable[index] : '.';
            } else {
                std::print("{}{}", bytesTable[index], bytesTable[index]);
            }
        }
        for (index = {}; index < col; index++) {
            if (!index)
                std::print(" : ");
            std::print("{}", asciiTable[index]);
        }

        std::print("\n");

        if (count != -1) {
            count -= read;
            if (count <= 0)
                break;
        }

    } while (read >= 0);
}