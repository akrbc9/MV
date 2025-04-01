cmake_minimum_required(VERSION 3.14)
project(predator_prey VERSION 1.0)

# Set C++ standard and architecture
set(DCMAKE_CXX_COMPILER "/opt/homebrew/opt/llvm/bin/clang++")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -arch arm64")
set(CMAKE_OSX_ARCHITECTURES "arm64")
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_LINKER "/usr/bin/ld")
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Find required packages
find_package(SFML 3.0 COMPONENTS Graphics Window System REQUIRED)
find_package(Python3 COMPONENTS Interpreter Development REQUIRED)

# Add include directories
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/parameter_sweep/include
    ${SFML_INCLUDE_DIR}
    ${Python3_INCLUDE_DIRS}
)

# Download and include nlohmann/json
include(FetchContent)
FetchContent_Declare(
    nlohmann_json
    URL https://github.com/nlohmann/json/releases/download/v3.11.2/json.tar.xz
)

# Download and include pybind11
FetchContent_MakeAvailable(nlohmann_json)

# Add source files for main simulation
set(SIMULATION_SOURCES
    src/agent.cpp
    src/grid.cpp
    src/spatial_hash.cpp
    src/simulation_controller.cpp
    src/simulation_report.cpp
)

# Add parameter sweep sources
set(PARAMETER_SWEEP_SOURCES
    parameter_sweep/src/parameter_sweep.cpp
    parameter_sweep/src/sample_manager.cpp
    parameter_sweep/src/lhs_sampler.cpp
)

# Add header files
set(HEADERS
    include/position.hpp
    include/simulation_config.hpp
    include/simulation_context.hpp
    include/agent.hpp
    include/grid.hpp
    include/spatial_hash.hpp
    include/simulation_controller.hpp
    include/simulation_report.hpp
    parameter_sweep/include/parameter_sweep.hpp
    parameter_sweep/include/sample_manager.hpp
    parameter_sweep/include/lhs_sampler.hpp
)

# Create simulation library
add_library(predator_prey_lib STATIC ${SIMULATION_SOURCES} ${HEADERS})
target_include_directories(predator_prey_lib PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/parameter_sweep/include
    ${SFML_INCLUDE_DIR}
)
target_link_libraries(predator_prey_lib PUBLIC 
    nlohmann_json::nlohmann_json
    SFML::Graphics
    SFML::Window
    SFML::System
)

# Create main simulation executable
add_executable(predator_prey src/main.cpp)
target_link_libraries(predator_prey PRIVATE predator_prey_lib)

# Create parameter sweep executable
add_executable(parameter_sweep ${PARAMETER_SWEEP_SOURCES})
target_link_libraries(parameter_sweep PRIVATE predator_prey_lib)

# Python module
# Rename the output library to match Python's module naming convention
# set_target_properties(predator_prey_python PROPERTIES 
#     OUTPUT_NAME "predator_prey"
#     PREFIX "${PYTHON_MODULE_PREFIX}"
#     SUFFIX "${PYTHON_MODULE_EXTENSION}"
# )

# Add compiler warnings
if(MSVC)
    target_compile_options(predator_prey PRIVATE /W4)
    target_compile_options(parameter_sweep PRIVATE /W4)
    # target_compile_options(predator_prey_python PRIVATE /W4)
else()
    target_compile_options(predator_prey PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(parameter_sweep PRIVATE -Wall -Wextra -Wpedantic)
    # target_compile_options(predator_prey_python PRIVATE -Wall -Wextra -Wpedantic)
endif()