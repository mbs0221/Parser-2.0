# Parser子目录的CMakeLists.txt

# 设置源文件
set(SOURCES
    lexer.cpp
    inter.cpp
    interpreter.cpp
    parser.cpp
    main.cpp
    logger.cpp
    ast_optimizer.cpp
)

# 设置头文件
set(HEADERS
    inter.h
    lexer.h
    parser.h
    logger.h
    ast_visitor.h
    ast_optimizer.h
)

# 创建可执行文件
add_executable(parser ${SOURCES} ${HEADERS})

# 查找log4cpp
find_package(PkgConfig REQUIRED)
pkg_check_modules(LOG4CPP REQUIRED log4cpp)

# 设置包含目录
target_include_directories(parser PRIVATE ${CMAKE_CURRENT_SOURCE_DIR} ${LOG4CPP_INCLUDE_DIRS})

# 设置输出目录
set_target_properties(parser PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
    CXX_STANDARD 11
    CXX_STANDARD_REQUIRED ON
)

# 链接log4cpp库
target_link_libraries(parser ${LOG4CPP_LIBRARIES})
target_compile_options(parser PRIVATE ${LOG4CPP_CFLAGS_OTHER})

# 安装规则
install(TARGETS parser
    RUNTIME DESTINATION bin
)
