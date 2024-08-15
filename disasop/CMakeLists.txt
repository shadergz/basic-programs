
find_package(LLVM 15.0 REQUIRED
    armdesc armdisassembler aarch64desc aarch64disassembler
)

add_executable(disasop program.cpp)
target_include_directories(disasop PRIVATE ${LLVM_INCLUDE_DIRS})
target_link_directories(disasop PRIVATE ${LLVM_LIBRARY_DIRS})
target_link_libraries(disasop PRIVATE ${LLVM_LIBRARIES})