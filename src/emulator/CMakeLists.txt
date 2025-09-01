# Emulator子项目CMakeLists.txt

# 设置源文件
set(EMULATOR_SOURCES
    emulator.cpp
    mmio.cpp
    vm_machine.cpp
    vm_loader.cpp
    vm_executor.cpp
)

# C++14
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 创建emulator库
add_library(emulator STATIC ${EMULATOR_SOURCES})

# 链接依赖库
target_link_libraries(emulator
    ${LOG4CPP_LIBRARIES}
)

# 设置包含目录
target_include_directories(emulator PUBLIC
    ${CMAKE_SOURCE_DIR}/include
    ${LOG4CPP_INCLUDE_DIRS}
)

# 创建emulator可执行文件
add_executable(emulator_main main.cpp)

# 链接emulator库
target_link_libraries(emulator_main
    emulator
    ${LOG4CPP_LIBRARIES}
)

# 设置包含目录
target_include_directories(emulator_main PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${LOG4CPP_INCLUDE_DIRS}
)
