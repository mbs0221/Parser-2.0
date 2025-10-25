# Parser库的CMakeLists.txt

# 设置源文件
set(PARSER_SOURCES
    ${CMAKE_SOURCE_DIR}/src/parser/parser.cpp
    ${CMAKE_SOURCE_DIR}/src/parser/expression.cpp
    ${CMAKE_SOURCE_DIR}/src/parser/statement.cpp
    ${CMAKE_SOURCE_DIR}/src/parser/inter.cpp
    ${CMAKE_SOURCE_DIR}/src/parser/definition.cpp
    ${CMAKE_SOURCE_DIR}/src/parser/flex_bison_parser.cpp
    ${CMAKE_SOURCE_DIR}/src/parser/unified_parser.cpp
)

# 设置头文件
set(PARSER_HEADERS
    ${CMAKE_SOURCE_DIR}/include/parser/parser.h
    ${CMAKE_SOURCE_DIR}/include/parser/expression.h
    ${CMAKE_SOURCE_DIR}/include/parser/statement.h
    ${CMAKE_SOURCE_DIR}/include/parser/definition.h
    ${CMAKE_SOURCE_DIR}/include/parser/ast_visitor.h
    ${CMAKE_SOURCE_DIR}/include/parser/inter.h
    ${CMAKE_SOURCE_DIR}/include/parser/flex_bison_parser.h
    ${CMAKE_SOURCE_DIR}/include/parser/unified_parser.h
)

# 查找flex和bison
find_program(FLEX_EXECUTABLE flex)
find_program(BISON_EXECUTABLE bison)

if(FLEX_EXECUTABLE AND BISON_EXECUTABLE)
    message(STATUS "Found flex: ${FLEX_EXECUTABLE}")
    message(STATUS "Found bison: ${BISON_EXECUTABLE}")
    
    # 生成flex词法分析器
    add_custom_command(
        OUTPUT ${CMAKE_BINARY_DIR}/mbs_lexer.c
        COMMAND ${FLEX_EXECUTABLE} -o ${CMAKE_BINARY_DIR}/mbs_lexer.c ${CMAKE_SOURCE_DIR}/src/parser/flex_bison/mbs_lexer.l
        DEPENDS ${CMAKE_SOURCE_DIR}/src/parser/flex_bison/mbs_lexer.l
        COMMENT "Generating lexer with flex"
    )
    
    # 生成bison语法分析器
    add_custom_command(
        OUTPUT ${CMAKE_BINARY_DIR}/mbs_parser.tab.cc ${CMAKE_BINARY_DIR}/mbs_parser.tab.hh
        COMMAND ${BISON_EXECUTABLE} -d -o ${CMAKE_BINARY_DIR}/mbs_parser.tab.cc ${CMAKE_SOURCE_DIR}/src/parser/flex_bison/mbs_parser.y
        DEPENDS ${CMAKE_SOURCE_DIR}/src/parser/flex_bison/mbs_parser.y
        COMMENT "Generating parser with bison"
    )
    
    # 添加生成的源文件
    list(APPEND PARSER_SOURCES 
        ${CMAKE_BINARY_DIR}/mbs_lexer.c
        ${CMAKE_BINARY_DIR}/mbs_parser.tab.cc
    )
    
    # 设置包含目录以包含生成的头文件
    target_include_directories(parser PUBLIC ${CMAKE_BINARY_DIR})
    
else()
    message(WARNING "flex or bison not found. Flex/Bison parser will not be available.")
    message(WARNING "Please install flex and bison to enable Flex/Bison parser support.")
endif()

# 创建parser静态库
add_library(parser STATIC ${PARSER_SOURCES} ${PARSER_HEADERS})

# 设置位置无关代码
set_property(TARGET parser PROPERTY POSITION_INDEPENDENT_CODE ON)

# 设置包含目录
target_include_directories(parser PUBLIC 
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/Parser
    ${LOG4CPP_INCLUDE_DIRS}
)

# 设置编译选项
target_compile_options(parser PRIVATE ${LOG4CPP_CFLAGS_OTHER})

# 链接依赖库
target_link_libraries(parser lexer common ${LOG4CPP_LIBRARIES})

# 设置输出目录
set_target_properties(parser PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
    CXX_STANDARD 11
    CXX_STANDARD_REQUIRED ON
)
