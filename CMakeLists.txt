cmake_minimum_required(VERSION 3.10)

# 项目名称和版本 - 支持 C 和 C++ 混合编译
project(AccountManagement VERSION 1.0.0 LANGUAGES C CXX)

# 设置 C++ 标准
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 设置 C 标准
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

# 包含目录
include_directories(
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/src/sqlite3
)

# 分别查找 C 和 C++ 源文件
file(GLOB_RECURSE CPP_SOURCES
    "src/*.cpp"
    "src/services/*.cpp"
)

file(GLOB_RECURSE C_SOURCES
    "src/sqlite3/*.c"
)

# 创建可执行文件
add_executable(${PROJECT_NAME} ${CPP_SOURCES} ${C_SOURCES})

# 设置输出目录 - 可执行文件输出到项目根目录，命名为 app.exe
set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}
    OUTPUT_NAME "app"
)
set_target_properties(${PROJECT_NAME} PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
)

get_target_property(OUT_DIR ${PROJECT_NAME} RUNTIME_OUTPUT_DIRECTORY)
message(STATUS "Runtime output directory for ${PROJECT_NAME}: ${OUT_DIR}")

# 设置目标属性
target_compile_definitions(${PROJECT_NAME} PRIVATE
    _CRT_SECURE_NO_WARNINGS
)

# 为 C 文件设置编译选项 - 禁用 C++ 严格类型检查（仅 GCC/Clang）
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    set_source_files_properties(${C_SOURCES} PROPERTIES
        COMPILE_FLAGS "-fpermissive"
    )
endif()

# 链接库
if(WIN32)
    target_link_libraries(${PROJECT_NAME} PRIVATE ws2_32)
endif()

# 安装目标（可选）
install(TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION bin
)

# 安装头文件（可选）
install(DIRECTORY include/
    DESTINATION include
    FILES_MATCHING PATTERN "*.h"
)

# 编译选项
if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /W4)
else()
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra)
endif()

# 可选的构建类型配置
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

string(TOUPPER ${CMAKE_BUILD_TYPE} BUILD_TYPE_UPPER)

if(BUILD_TYPE_UPPER STREQUAL "DEBUG")
    target_compile_definitions(${PROJECT_NAME} PRIVATE DEBUG)
    target_compile_options(${PROJECT_NAME} PRIVATE -g -O0)
else()
    target_compile_definitions(${PROJECT_NAME} PRIVATE NDEBUG)
    target_compile_options(${PROJECT_NAME} PRIVATE -O2)
endif()

# 显示构建信息
message(STATUS "项目名称：${PROJECT_NAME}")
message(STATUS "构建类型：${CMAKE_BUILD_TYPE}")
message(STATUS "C++ 标准：${CMAKE_CXX_STANDARD}")
message(STATUS "可执行文件输出目录：${CMAKE_SOURCE_DIR}")
