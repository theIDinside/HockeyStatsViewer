cmake_minimum_required(VERSION 3.15)
project(instrumentation)

# We need to specifically turn this off for our simple instrumentation library, as it really has nothing to do with Qt. I don't know how to set this variable on a per-target specific level.
set(CMAKE_AUTOMOC OFF)
set(CMAKE_AUTORCC OFF)
set(CMAKE_AUTOUIC OFF)

set(CMAKE_CXX_STANDARD 17)
set(SOURCES instrumentation.cpp instrumentation.h)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
add_library(instrumentation STATIC ${SOURCES})
target_link_libraries(instrumentation pthread)
target_include_directories(instrumentation PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})