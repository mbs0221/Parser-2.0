# Lexer库的CMakeLists.txt

# 设置源文件
set(LEXER_SOURCES
    ${CMAKE_SOURCE_DIR}/src/lexer/lexer.cpp
    ${CMAKE_SOURCE_DIR}/src/lexer/value.cpp
    ${CMAKE_SOURCE_DIR}/src/lexer/token.cpp
)

# 设置头文件
set(LEXER_HEADERS
    ${CMAKE_SOURCE_DIR}/include/lexer/lexer.h
    ${CMAKE_SOURCE_DIR}/include/lexer/value.h
    ${CMAKE_SOURCE_DIR}/include/lexer/token.h
)

# 创建lexer动态库
add_library(lexer SHARED ${LEXER_SOURCES} ${LEXER_HEADERS})

# 设置位置无关代码
set_property(TARGET lexer PROPERTY POSITION_INDEPENDENT_CODE ON)

# 设置包含目录
target_include_directories(lexer PUBLIC 
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/Parser
    ${LOG4CPP_INCLUDE_DIRS}
)

# 设置编译选项
target_compile_options(lexer PRIVATE ${LOG4CPP_CFLAGS_OTHER})

# 链接依赖库
target_link_libraries(lexer common ${LOG4CPP_LIBRARIES})

# 添加debug宏定义（在Debug构建类型下启用）
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_definitions(lexer PRIVATE LEXER_DEBUG)
endif()

# 设置输出目录
set_target_properties(lexer PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)


