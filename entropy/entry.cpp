#include <iomanip>
#include <vector>
#include <fstream>
#include <print>
#include <filesystem>
#include <complex>

#include <common.h>
double genEntropy(const std::streamsize total, const std::vector<u32>& frequency) {
    if (frequency.size() != 256) {
    }
    double result{};
    for (const auto bytes : frequency) {
        // ReSharper disable once CppTooWideScopeInitStatement
        if (bytes) {
            const auto temp{static_cast<double>(bytes) / static_cast<double>(total)};
            if (temp > 0.)
                result += temp * std::fabs(std::log2(temp));
        }
    }
    return result;
}

void programEntry(std::fstream& io, const std::filesystem::path& path) {
    std::cout << std::fixed << std::setprecision(2);

    std::vector<char> buffer(16 * 1024);
    // Store the frequency of bytes from the files
    std::vector<u32> bytesFrequency(256);

    std::ranges::fill(bytesFrequency, 0);
    // ReSharper disable once CppDFAUnusedValue
    std::streamsize br{},
        totalBytes{};
    while (!io.eof()) {
        io.read(&buffer[0], static_cast<std::streamsize>(buffer.size()));
        br = io.gcount();
        totalBytes += br;

        for (const auto byte : std::span(&buffer[0], br)) {
            bytesFrequency[static_cast<u8>(byte)]++;
        }
    }

    const auto result{genEntropy(totalBytes, bytesFrequency)};
    std::print("Entropy of the file '{}': {}\n", path.string(), result);
}
