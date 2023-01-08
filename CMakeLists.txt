cmake_minimum_required (VERSION 3.20)

project(ParallelSum)

add_executable(ParallelSum "ParallelSum.cpp")

set_target_properties(ParallelSum PROPERTIES CXX_STANDARD 20)
