# Lexer测试配置
add_executable(test_lexer_comprehensive test_lexer_comprehensive.cpp)
target_include_directories(test_lexer_comprehensive PRIVATE 
    ${CMAKE_SOURCE_DIR}/include
)
target_link_libraries(test_lexer_comprehensive 
    lexer
    gtest
    gtest_main
    pthread
)
set_target_properties(test_lexer_comprehensive PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)
add_test(NAME test_lexer_comprehensive COMMAND test_lexer_comprehensive)

# 其他lexer相关测试
add_executable(test_lexer_basic test_lexer.cpp)
target_include_directories(test_lexer_basic PRIVATE 
    ${CMAKE_SOURCE_DIR}/include
)
target_link_libraries(test_lexer_basic 
    lexer
    gtest
    gtest_main
    pthread
)
set_target_properties(test_lexer_basic PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)
add_test(NAME test_lexer_basic COMMAND test_lexer_basic)

add_executable(test_value_variables test_value_variables.cpp)
target_include_directories(test_value_variables PRIVATE 
    ${CMAKE_SOURCE_DIR}/include
)
target_link_libraries(test_value_variables 
    lexer
    gtest
    gtest_main
    pthread
)
set_target_properties(test_value_variables PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)
add_test(NAME test_value_variables COMMAND test_value_variables)

add_executable(test_string_operations test_string_operations.cpp)
target_include_directories(test_string_operations PRIVATE 
    ${CMAKE_SOURCE_DIR}/include
)
target_link_libraries(test_string_operations 
    lexer
    gtest
    gtest_main
    pthread
)
set_target_properties(test_string_operations PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)
add_test(NAME test_string_operations COMMAND test_string_operations)

add_executable(test_operator_design test_operator_design.cpp)
target_include_directories(test_operator_design PRIVATE 
    ${CMAKE_SOURCE_DIR}/include
)
target_link_libraries(test_operator_design 
    lexer
    gtest
    gtest_main
    pthread
)
set_target_properties(test_operator_design PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)
add_test(NAME test_operator_design COMMAND test_operator_design)

add_executable(test_operator_type test_operator_type.cpp)
target_include_directories(test_operator_type PRIVATE 
    ${CMAKE_SOURCE_DIR}/include
)
target_link_libraries(test_operator_type 
    lexer
    gtest
    gtest_main
    pthread
)
set_target_properties(test_operator_type PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)
add_test(NAME test_operator_type COMMAND test_operator_type)

add_executable(test_merged_target_type test_merged_target_type.cpp)
target_include_directories(test_merged_target_type PRIVATE 
    ${CMAKE_SOURCE_DIR}/include
)
target_link_libraries(test_merged_target_type 
    lexer
    gtest
    gtest_main
    pthread
)
set_target_properties(test_merged_target_type PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)
add_test(NAME test_merged_target_type COMMAND test_merged_target_type)
