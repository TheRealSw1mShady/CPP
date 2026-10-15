set(CMAKE_CXX_COMPILER g++)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

cmake_minimum_required(VERSION 4.4)

# Set some basic project attributes
project (simplePool
	VERSION 1.0
	LANGUAGES CXX)

# This project will output an executable file
add_executable(${PROJECT_NAME} main.cpp)