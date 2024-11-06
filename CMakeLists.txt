cmake_minimum_required(VERSION 3.0)  # 指定 CMake 的最低版本要求

project(TinyJsonExample)  # 定义项目名称

# 设置 C++ 标准
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 添加 include 目录到头文件搜索路径
include_directories(include)


# 创建一个可执行文件
add_executable(TinyJsonExample src/main.cpp)