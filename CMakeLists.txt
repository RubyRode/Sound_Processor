cmake_minimum_required(VERSION 3.24)
project(Sound_processor)

set(CMAKE_CXX_STANDARD 20)

add_executable(Sound_processor main.cpp Utility/Input_parser.cpp Utility/Input_parser.h Proc/Proccessor.cpp Proc/Proccessor.h)
