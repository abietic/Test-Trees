cmake_minimum_required(VERSION 3.10)

# set the project name
project(Trees VERSION 0.01)

# add the executable
add_executable(Trees main.cxx)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)