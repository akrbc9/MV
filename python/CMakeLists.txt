cmake_minimum_required(VERSION 3.14)
project(predator_prey_python VERSION 1.0)

# Set C++ standard and architecture
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -arch arm64")
set(CMAKE_OSX_ARCHITECTURES "arm64")
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find Python and pybind11
find_package(Python3 COMPONENTS Interpreter Development REQUIRED)
find_package(pybind11 REQUIRED)

# Find matplotlib-cpp
find_package(Python3 COMPONENTS Interpreter Development NumPy REQUIRED)
find_package(matplotlib-cpp REQUIRED)

# Add source files
set(SOURCES
    src/python_bindings.cpp
)

# Add header files
set(HEADERS
    include/python_bindings.hpp
)

# Create Python module
pybind11_add_module(predator_prey MODULE ${SOURCES} ${HEADERS})

# Add include directories
target_include_directories(predator_prey PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/parameter_sweep/include
)

# Link libraries
target_link_libraries(predator_prey PRIVATE
    predator_prey_lib
    matplotlib-cpp
)

# Set output directory
set_target_properties(predator_prey PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/python
    PREFIX ""
    SUFFIX ".so"
)

# Add compiler warnings
if(MSVC)
    target_compile_options(predator_prey PRIVATE /W4)
else()
    target_compile_options(predator_prey PRIVATE -Wall -Wextra -Wpedantic)
endif() 