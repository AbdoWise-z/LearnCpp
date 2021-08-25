cmake_minimum_required(VERSION 3.0.0)
project(Proj VERSION 0.1.0)

include_directories("include")

# src/looper_thread.cpp is should be added to sources .. but I changed the compiler 
# so it doesnot work any more :|

set(PRJ_SRC src/main.cpp src/utility.cpp src/console.cpp src/MathUtils.cpp)

add_executable(Proj ${PRJ_SRC})
add_executable(Playground src/playground.cpp)
