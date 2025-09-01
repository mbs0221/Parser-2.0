# SSA转换器模块构建配置

# SSA转换器库
add_library(ssa_converter STATIC
    ssa_converter.cpp
    expression_converter.cpp
    statement_converter.cpp
)

target_include_directories(ssa_converter PRIVATE
    ${CMAKE_SOURCE_DIR}/include
)

target_link_libraries(ssa_converter 
    parser 
    interpreter 
    lexer
)

# 设置编译标准
set_target_properties(ssa_converter PROPERTIES
    CXX_STANDARD 11
    CXX_STANDARD_REQUIRED ON
)

# 编译器测试程序
add_executable(simple_test
    simple_test.cpp
)

add_executable(template_test
    template_test.cpp
)

add_executable(ssa_simple_test
    ssa_simple_test.cpp
)

set_target_properties(simple_test template_test ssa_simple_test PROPERTIES
    CXX_STANDARD 11
    CXX_STANDARD_REQUIRED ON
)
