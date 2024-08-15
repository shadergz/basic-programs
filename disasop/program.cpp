#include <algorithm>
#include <array>
#include <iostream>
#include <vector>
#include <cstring>
#include <sstream>

#include <llvm-c/Disassembler.h>
#include <llvm-c/Target.h>

#include <print>
#include <common.h>
#include <iomanip>

void printUsage(auto self) {
    std::print("Usage: {} <PC> <thumb/a32/a64> <Instructions (hexadecimal)>\n", self);
    std::print("\n");
    std::exit(EXIT_FAILURE);
}
template <typename... Args>
void clash(auto self, const std::format_string<Args...> format, Args... args) {
    std::print(format, std::forward<Args>(args)...);
    printUsage(self);
}

auto collectAllInstructions(const i32 argc, char** argv) {
    std::vector<u32> collection;
    for (i32 index{3}; index < argc; ++index) {
        std::string_view hex{argv[index]};

        std::stringstream ss{};
        if (!hex.starts_with("0x")) {
            ss << hex;
        } else {
            ss << std::hex << hex;
        }
        u32 instruction;
        ss >> instruction;
        collection.emplace_back(instruction);
    }

    return collection;
}

std::array<char, 0xff> buffer{};

std::string disassembleA32(const bool isThumb, const u32 pc, const u32 instruction, u32 size) {
    const auto ctx{LLVMCreateDisasm(isThumb ? "thumbv7" : "armv8-arm", nullptr, 0, nullptr, nullptr)};
    LLVMSetDisasmOptions(ctx, LLVMDisassembler_Option_UseMarkup);

    std::string result;
    while (size) {
        std::array<u8, 0x4> macro{};
        std::memcpy(macro.data(), &instruction, size);

        auto sizeUsed{LLVMDisasmInstruction(ctx, macro.data(), size, pc, buffer.data(), buffer.size())};
        result = sizeUsed > 0 ? buffer.data() : "<invalid>";
        if (!sizeUsed) {
            if (isThumb)
                sizeUsed = 2;
            else
                sizeUsed = 4;
        }
        result += '\n';
        size -= sizeUsed;
    }

    LLVMDisasmDispose(ctx);
    return result;
}

std::string disassembleArch(const std::string& arch, const u32 pc, const u32 instruction) {
    std::string result;
    const auto ctx{LLVMCreateDisasm(arch.data(), nullptr, 0, nullptr, nullptr)};
    LLVMSetDisasmOptions(ctx, LLVMDisassembler_Option_AsmPrinterVariant);

    std::array<u8, 0x4> macro{};
    std::memcpy(macro.data(), &instruction, macro.size());
    if (!LLVMDisasmInstruction(ctx, macro.data(), pc, instruction, buffer.data(), buffer.size())) {
        result = "<invalid>\n";
    } else {
        result = buffer.data();
        result += '\n';
    }
    LLVMDisasmDispose(ctx);
    return result;
}

int main(const i32 argc, char **argv) {
    if (argc < 4)
        printUsage(argv[0]);

    i32 mode{};
    bool selected{false};
    for (const auto match : {"thumb", "a32", "a64", "mips", "mipsel", "mips64", "mips64el", ""}) {
        if (!selected) {
            if (std::string_view(argv[2]) == std::string_view(match))
                selected = true;
            mode++;
        }
    }
    if (mode == 8)
        mode = {};
    if (!mode)
        printUsage(argv[0]);

    auto pc{std::strtol(argv[1], nullptr, 16)};
    // ReSharper disable once CppTooWideScope
    const bool is32{mode <= 2};
    if (is32) {
        LLVMInitializeARMTargetInfo();
        LLVMInitializeARMTargetMC();

        LLVMInitializeARMDisassembler();
    } else {
        if (mode >= 4) {
            LLVMInitializeMipsTargetInfo();
            LLVMInitializeMipsTargetMC();
            LLVMInitializeMipsDisassembler();
        } else {
            // aarch64
            LLVMInitializeAArch64TargetInfo();
            LLVMInitializeAArch64TargetMC();
            LLVMInitializeAArch64Disassembler();
        }
    }

    // ReSharper disable once CppTooWideScopeInitStatement
    const auto instructions{collectAllInstructions(argc, argv)};
    for (const auto risc : instructions) {
        u32 size{4};

        switch (mode) {
            case 1:
                size = risc >> 16 == 0 ? 2 : 4;
                if (size == 4)
                    std::print("{:08x}: {:08x} -> {}", pc, risc, disassembleA32(true, pc, risc, size));
                else
                    std::print("{:08x}: {:04x} -> {}", pc, risc, disassembleA32(true, pc, risc, size));
                break;
            case 2:
                std::print("{:08x}: {:08x} -> {}", pc, risc, disassembleA32(false, pc, risc, size));
                break;
            default:
                case 3:
                std::print("{:08x}: {:08x} -> {}", pc, risc, disassembleArch(argv[2], pc, risc));
                break;
        }
        pc += size;
    }

    return EXIT_SUCCESS;
}
