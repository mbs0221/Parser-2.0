# Optimizer库的CMakeLists.txt

# 设置源文件
set(OPTIMIZER_SOURCES
    ${CMAKE_SOURCE_DIR}/src/optimizer/ast_optimizer.cpp
)

# 设置头文件
set(OPTIMIZER_HEADERS
    ${CMAKE_SOURCE_DIR}/include/optimizer/ast_optimizer.h
    ${CMAKE_SOURCE_DIR}/include/parser/ast_visitor.h
)

# 创建optimizer静态库
add_library(optimizer STATIC ${OPTIMIZER_SOURCES} ${OPTIMIZER_HEADERS})

# 设置包含目录
target_include_directories(optimizer PUBLIC 
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/Parser
    ${LOG4CPP_INCLUDE_DIRS}
)

# 设置编译选项
target_compile_options(optimizer PRIVATE ${LOG4CPP_CFLAGS_OTHER})

# 链接依赖库
target_link_libraries(optimizer parser)

# 设置输出目录
set_target_properties(optimizer PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
    CXX_STANDARD 11
    CXX_STANDARD_REQUIRED ON
)
