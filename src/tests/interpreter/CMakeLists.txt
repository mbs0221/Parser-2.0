# Interpreter测试配置
add_executable(test_interpreter_comprehensive test_interpreter_comprehensive.cpp)
target_include_directories(test_interpreter_comprehensive PRIVATE 
    ${CMAKE_SOURCE_DIR}/include
)
target_link_libraries(test_interpreter_comprehensive 
    interpreter
    parser
    lexer
    gtest
    gtest_main
    pthread
)
add_test(NAME test_interpreter_comprehensive COMMAND test_interpreter_comprehensive)

# 其他interpreter相关测试
add_executable(test_interpreter_basic test_interpreter.cpp)
target_include_directories(test_interpreter_basic PRIVATE 
    ${CMAKE_SOURCE_DIR}/include
)
target_link_libraries(test_interpreter_basic 
    interpreter
    parser
    lexer
    gtest
    gtest_main
    pthread
)
add_test(NAME test_interpreter_basic COMMAND test_interpreter_basic)

add_executable(test_interpreter_execution test_interpreter_execution.cpp)
target_include_directories(test_interpreter_execution PRIVATE 
    ${CMAKE_SOURCE_DIR}/include
)
target_link_libraries(test_interpreter_execution 
    interpreter
    parser
    lexer
    gtest
    gtest_main
    pthread
)
add_test(NAME test_interpreter_execution COMMAND test_interpreter_execution)

add_executable(test_interpreter_simple test_interpreter_simple.cpp)
target_include_directories(test_interpreter_simple PRIVATE 
    ${CMAKE_SOURCE_DIR}/include
)
target_link_libraries(test_interpreter_simple 
    interpreter
    parser
    lexer
    gtest
    gtest_main
    pthread
)
add_test(NAME test_interpreter_simple COMMAND test_interpreter_simple)

add_executable(test_simple_interpreter test_simple_interpreter.cpp)
target_include_directories(test_simple_interpreter PRIVATE 
    ${CMAKE_SOURCE_DIR}/include
)
target_link_libraries(test_simple_interpreter 
    interpreter
    parser
    lexer
    gtest
    gtest_main
    pthread
)
add_test(NAME test_simple_interpreter COMMAND test_simple_interpreter)

add_executable(test_builtin_function test_builtin_function.cpp)
target_include_directories(test_builtin_function PRIVATE 
    ${CMAKE_SOURCE_DIR}/include
)
target_link_libraries(test_builtin_function 
    interpreter
    parser
    lexer
    gtest
    gtest_main
    pthread
)
add_test(NAME test_builtin_function COMMAND test_builtin_function)

add_executable(test_builtin_functions_as_identifiers test_builtin_functions_as_identifiers.cpp)
target_include_directories(test_builtin_functions_as_identifiers PRIVATE 
    ${CMAKE_SOURCE_DIR}/include
)
target_link_libraries(test_builtin_functions_as_identifiers 
    interpreter
    parser
    lexer
    gtest
    gtest_main
    pthread
)
add_test(NAME test_builtin_functions_as_identifiers COMMAND test_builtin_functions_as_identifiers)

# Cast操作测试
add_executable(test_cast_operations test_cast_operations.cpp)
target_include_directories(test_cast_operations PRIVATE 
    ${CMAKE_SOURCE_DIR}/include
)
target_link_libraries(test_cast_operations 
    interpreter
    parser
    lexer
    gtest
    gtest_main
    pthread
)
add_test(NAME test_cast_operations COMMAND test_cast_operations)

add_executable(test_builtin_refactor test_builtin_refactor.cpp)
target_include_directories(test_builtin_refactor PRIVATE 
    ${CMAKE_SOURCE_DIR}/include
)
target_link_libraries(test_builtin_refactor 
    interpreter
    parser
    lexer
    gtest
    gtest_main
    pthread
)
add_test(NAME test_builtin_refactor COMMAND test_builtin_refactor)

add_executable(test_visitor test_visitor.cpp)
target_include_directories(test_visitor PRIVATE 
    ${CMAKE_SOURCE_DIR}/include
)
target_link_libraries(test_visitor 
    interpreter
    parser
    lexer
    gtest
    gtest_main
    pthread
)
add_test(NAME test_visitor COMMAND test_visitor)

add_executable(test_value_visitor test_value_visitor.cpp)
target_include_directories(test_value_visitor PRIVATE 
    ${CMAKE_SOURCE_DIR}/include
)
target_link_libraries(test_value_visitor 
    interpreter
    parser
    lexer
    gtest
    gtest_main
    pthread
)
add_test(NAME test_value_visitor COMMAND test_value_visitor)
