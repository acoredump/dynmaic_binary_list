cmake_minimum_required(VERSION 3.21)
project(phaeton)
set(CMAKE_CXX_FLAGS_DEBUG "-fPIC -O0 -g -Wall")
set(CMAKE_CXX_STANDARD 14)

add_executable(phaeton main.cpp base_struct/skip_list.h base.h base_struct/skip_list.cpp
        base_struct/simple_queue.h
        base_struct/simple_queue.cpp
        base_struct/binary_list.h
        base_struct/binary_list.cpp
        base_struct/rw_spin_lock.h
        base_struct/rw_spin_lock.cpp)
find_package(Threads REQUIRED)
target_link_libraries(phaeton PRIVATE Threads::Threads)