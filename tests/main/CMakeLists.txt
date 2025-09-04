# Main可执行文件的CMakeLists.txt

# 1. 词法分析器主程序 (lexer_main)
add_executable(lexer_main lexer_main.cpp)

target_include_directories(lexer_main PRIVATE 
    ${CMAKE_SOURCE_DIR}/include
    ${LOG4CPP_INCLUDE_DIRS}
)

target_compile_options(lexer_main PRIVATE ${LOG4CPP_CFLAGS_OTHER})

target_link_libraries(lexer_main 
    lexer
    ${LOG4CPP_LIBRARIES}
)

set_target_properties(lexer_main PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)

# 2. 语法分析器主程序 (parser_main)
add_executable(parser_main parser_main.cpp)

target_include_directories(parser_main PRIVATE 
    ${CMAKE_SOURCE_DIR}/include
    ${LOG4CPP_INCLUDE_DIRS}
)

target_compile_options(parser_main PRIVATE ${LOG4CPP_CFLAGS_OTHER})

target_link_libraries(parser_main 
    lexer
    parser
    interpreter
    ${LOG4CPP_LIBRARIES}
)

set_target_properties(parser_main PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)

# 3. 解释器主程序 (interpreter_main)
add_executable(interpreter_main interpreter_main.cpp)

target_include_directories(interpreter_main PRIVATE 
    ${CMAKE_SOURCE_DIR}/include
    ${LOG4CPP_INCLUDE_DIRS}
)

target_compile_options(interpreter_main PRIVATE ${LOG4CPP_CFLAGS_OTHER})

target_link_libraries(interpreter_main 
    lexer
    parser
    interpreter
    ${LOG4CPP_LIBRARIES}
)

set_target_properties(interpreter_main PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)

# 安装规则
install(TARGETS lexer_main parser_main interpreter_main
    RUNTIME DESTINATION bin
)
