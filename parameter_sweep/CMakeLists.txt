cmake_minimum_required(VERSION 3.14)
project(parameter_sweep VERSION 1.0)

# Set C++ standard and architecture
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -arch arm64 -fopenmp")
set(CMAKE_OSX_ARCHITECTURES "arm64")
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_LINKER "/usr/bin/ld")
set(CMAKE_CXX_STANDARD_REQUIRED ON)


# Find SFML
find_package(SFML 3.0 COMPONENTS Graphics Window System REQUIRED)
find_package(OpenMP REQUIRED)

# Add include directories
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${SFML_INCLUDE_DIR}
)

# Download and include nlohmann/json
include(FetchContent)
FetchContent_Declare(
    nlohmann_json
    URL https://github.com/nlohmann/json/releases/download/v3.11.2/json.tar.xz
)
FetchContent_MakeAvailable(nlohmann_json)

# Add source files
set(SOURCES
    src/parameter_sweep.cpp
    src/lhs_sampler.cpp
)

# Add header files
set(HEADERS
    include/parameter_sweep.hpp
    include/lhs_sampler.hpp
) 

