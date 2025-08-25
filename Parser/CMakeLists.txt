# Parser子目录的CMakeLists.txt

# 设置源文件
set(SOURCES
    lexer.cpp
    inter.cpp
    interpreter.cpp
    parser.cpp
    main.cpp
)

# 设置头文件
set(HEADERS
    inter.h
    lexer.h
    parser.h
)

# 创建可执行文件
add_executable(parser ${SOURCES} ${HEADERS})

# 设置包含目录
target_include_directories(parser PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

# 设置输出目录
set_target_properties(parser PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
    CXX_STANDARD 11
    CXX_STANDARD_REQUIRED ON
)

# 安装规则
install(TARGETS parser
    RUNTIME DESTINATION bin
)
