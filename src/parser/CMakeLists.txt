# Parser库的CMakeLists.txt

# 设置源文件
set(PARSER_SOURCES
    ${CMAKE_SOURCE_DIR}/src/parser/parser.cpp
    ${CMAKE_SOURCE_DIR}/src/parser/expression.cpp
    ${CMAKE_SOURCE_DIR}/src/parser/statement.cpp
    ${CMAKE_SOURCE_DIR}/src/parser/scope.cpp
    ${CMAKE_SOURCE_DIR}/src/parser/inter.cpp
    ${CMAKE_SOURCE_DIR}/src/parser/function.cpp
)

# 设置头文件
set(PARSER_HEADERS
    ${CMAKE_SOURCE_DIR}/include/parser/parser.h
    ${CMAKE_SOURCE_DIR}/include/parser/expression.h
    ${CMAKE_SOURCE_DIR}/include/parser/statement.h
    ${CMAKE_SOURCE_DIR}/include/parser/scope.h
    ${CMAKE_SOURCE_DIR}/include/parser/function.h
    ${CMAKE_SOURCE_DIR}/include/parser/ast_visitor.h
    ${CMAKE_SOURCE_DIR}/include/parser/inter.h
)

# 创建parser静态库
add_library(parser STATIC ${PARSER_SOURCES} ${PARSER_HEADERS})

# 设置包含目录
target_include_directories(parser PUBLIC 
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/Parser
    ${LOG4CPP_INCLUDE_DIRS}
)

# 设置编译选项
target_compile_options(parser PRIVATE ${LOG4CPP_CFLAGS_OTHER})

# 链接依赖库
target_link_libraries(parser lexer)

# 设置输出目录
set_target_properties(parser PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
    CXX_STANDARD 11
    CXX_STANDARD_REQUIRED ON
)
