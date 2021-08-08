cmake_minimum_required(VERSION 3.0.0)
project(Proj VERSION 0.1.0)

include_directories("include")

set(PRJ_SRC src/main.cpp src/utility.cpp src/looper_thread.cpp src/console.cpp src/MathUtils.cpp)

add_executable(Proj ${PRJ_SRC})
