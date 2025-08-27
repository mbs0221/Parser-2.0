# 测试子目录配置
add_subdirectory(lexer)
add_subdirectory(parser)
add_subdirectory(interpreter)
add_subdirectory(optimizer)

# 设置测试程序的输出目录
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
