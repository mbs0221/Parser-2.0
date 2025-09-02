# Main可执行文件的CMakeLists.txt

# 设置源文件
set(MAIN_SOURCES
    main.cpp
)

# 创建可执行文件
add_executable(parser_main ${MAIN_SOURCES})

# 设置包含目录
target_include_directories(parser_main PRIVATE 
    ${CMAKE_SOURCE_DIR}/include
    ${LOG4CPP_INCLUDE_DIRS}
)

# 设置编译选项
target_compile_options(parser_main PRIVATE ${LOG4CPP_CFLAGS_OTHER})

# 链接所有库
target_link_libraries(parser_main 
    lexer
    parser
    interpreter
    ${LOG4CPP_LIBRARIES}
)

# 设置输出目录
set_target_properties(parser_main PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
    CXX_STANDARD 11
    CXX_STANDARD_REQUIRED ON
)

# 安装规则
install(TARGETS parser_main
    RUNTIME DESTINATION bin
)
