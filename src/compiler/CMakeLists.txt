# Compiler库的CMakeLists.txt

# 设置源文件
set(COMPILER_SOURCES
    ${CMAKE_SOURCE_DIR}/src/compiler/expression_converter.cpp
    ${CMAKE_SOURCE_DIR}/src/compiler/statement_converter.cpp
    ${CMAKE_SOURCE_DIR}/src/compiler/ssa_converter.cpp
)

# 设置头文件
set(COMPILER_HEADERS
    ${CMAKE_SOURCE_DIR}/include/compiler/code_generator.h
    ${CMAKE_SOURCE_DIR}/include/compiler/ssa_converter.h
)

# 创建compiler静态库
add_library(compiler STATIC ${COMPILER_SOURCES} ${COMPILER_HEADERS})

# 设置位置无关代码
set_property(TARGET compiler PROPERTY POSITION_INDEPENDENT_CODE ON)

# 设置包含目录
target_include_directories(compiler PUBLIC 
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/Compiler
    ${LOG4CPP_INCLUDE_DIRS}
)

# 设置编译选项
target_compile_options(compiler PRIVATE ${LOG4CPP_CFLAGS_OTHER})

# 链接依赖库
target_link_libraries(compiler 
    parser 
    interpreter 
    lexer 
    common 
    ${LOG4CPP_LIBRARIES}
)

# 设置输出目录
set_target_properties(compiler PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
    CXX_STANDARD 11
    CXX_STANDARD_REQUIRED ON
)