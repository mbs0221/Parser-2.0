# Fuzzing CMakeLists.txt
# 用于构建LibFuzzer测试

cmake_minimum_required(VERSION 3.16)
project(ParserFuzzing)

# 设置C++标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 设置编译选项 - 不使用全局LibFuzzer标志
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address,undefined")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -O1")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O2")

# 链接选项
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fsanitize=address,undefined")

# 包含目录
include_directories(${CMAKE_SOURCE_DIR}/../include)
include_directories(${CMAKE_SOURCE_DIR}/../src)

# 查找源文件
file(GLOB_RECURSE LEXER_SOURCES 
    "${CMAKE_SOURCE_DIR}/../src/lexer/*.cpp"
    "${CMAKE_SOURCE_DIR}/../src/common/*.cpp"
)

file(GLOB_RECURSE PARSER_SOURCES 
    "${CMAKE_SOURCE_DIR}/../src/parser/*.cpp"
    "${CMAKE_SOURCE_DIR}/../src/lexer/*.cpp"
    "${CMAKE_SOURCE_DIR}/../src/common/*.cpp"
)

file(GLOB_RECURSE INTERPRETER_SOURCES 
    "${CMAKE_SOURCE_DIR}/../src/interpreter/*.cpp"
    "${CMAKE_SOURCE_DIR}/../src/parser/*.cpp"
    "${CMAKE_SOURCE_DIR}/../src/lexer/*.cpp"
    "${CMAKE_SOURCE_DIR}/../src/common/*.cpp"
)

# 创建最简单的fuzzer (用于基本测试)

# 创建最简单的fuzzer
add_executable(minimal_fuzzer
    interpreter/minimal_fuzzer.cpp
)
target_compile_options(minimal_fuzzer PRIVATE -fsanitize=fuzzer,address,undefined)
target_link_options(minimal_fuzzer PRIVATE -fsanitize=fuzzer,address,undefined)

# 创建针对性的lexer fuzzer
add_executable(lexer_fuzzer
    interpreter/lexer_fuzzer.cpp
    ${LEXER_SOURCES}
)
target_compile_options(lexer_fuzzer PRIVATE -fsanitize=fuzzer,address,undefined)
target_link_options(lexer_fuzzer PRIVATE -fsanitize=fuzzer,address,undefined)


# 创建针对性的parser fuzzer
add_executable(parser_fuzzer
    interpreter/parser_fuzzer.cpp
    ${PARSER_SOURCES}
)
target_compile_options(parser_fuzzer PRIVATE -fsanitize=fuzzer,address,undefined)
target_link_options(parser_fuzzer PRIVATE -fsanitize=fuzzer,address,undefined)

# 创建针对性的interpreter fuzzer
add_executable(interpreter_fuzzer
    interpreter/interpreter_fuzzer.cpp
    ${INTERPRETER_SOURCES}
)
target_compile_options(interpreter_fuzzer PRIVATE -fsanitize=fuzzer,address,undefined)
target_link_options(interpreter_fuzzer PRIVATE -fsanitize=fuzzer,address,undefined)

# 设置输出目录
set_target_properties(lexer_fuzzer PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/fuzz
)

set_target_properties(parser_fuzzer PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/fuzz
)

set_target_properties(interpreter_fuzzer PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/fuzz
)

set_target_properties(minimal_fuzzer PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/fuzz
)

# 链接库
target_link_libraries(lexer_fuzzer)
target_link_libraries(parser_fuzzer)
target_link_libraries(interpreter_fuzzer)
target_link_libraries(minimal_fuzzer)

# 添加自定义目标
add_custom_target(fuzz_all
    DEPENDS lexer_fuzzer parser_fuzzer interpreter_fuzzer minimal_fuzzer
    COMMENT "Building all fuzzing targets"
)

# 添加测试目标
add_custom_target(fuzz_test
    COMMAND ${CMAKE_BINARY_DIR}/fuzz/lexer_fuzzer -runs=1000 ${CMAKE_SOURCE_DIR}/fuzz/corpus/lexer
    COMMAND ${CMAKE_BINARY_DIR}/fuzz/parser_fuzzer -runs=1000 ${CMAKE_SOURCE_DIR}/fuzz/corpus/parser
    COMMAND ${CMAKE_BINARY_DIR}/fuzz/interpreter_fuzzer -runs=1000 ${CMAKE_SOURCE_DIR}/fuzz/corpus/interpreter
    DEPENDS fuzz_all
    COMMENT "Running fuzzing tests"
)

# 安装目标
install(TARGETS lexer_fuzzer parser_fuzzer interpreter_fuzzer
    RUNTIME DESTINATION bin/fuzz
)
