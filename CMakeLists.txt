cmake_minimum_required(VERSION 3.14)
project(predator_prey VERSION 1.0)

# Set C++ standard and architecture
set(DCMAKE_CXX_COMPILER "/opt/homebrew/opt/llvm/bin/clang++")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -arch arm64")
set(CMAKE_OSX_ARCHITECTURES "arm64")
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_LINKER "/usr/bin/ld")
set(CMAKE_CXX_STANDARD_REQUIRED ON)
# Target macOS and Apple Silicon M3 specifically
set(CMAKE_OSX_ARCHITECTURES "arm64")
set(CMAKE_OSX_DEPLOYMENT_TARGET "13.0" CACHE STRING "Minimum macOS deployment version")

# Enable compiler optimization flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=armv8.5-a+crypto+sha3 -mcpu=apple-m1")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -ffast-math -ffp-contract=fast")

# Enable vectorization and Apple-specific optimizations
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fvectorize -fslp-vectorize")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-stack-check")

# Enable link-time optimization (both at compile and link time)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -flto=thin")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -flto=thin")
set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)

# Optimize for performance over code size
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fomit-frame-pointer")

# M3-specific optimizations
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -falign-functions=64 -falign-loops=64")

# Enable Clang optimizations if using Clang (default on macOS)
if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fcolor-diagnostics -Rpass=loop,vectorize")
    # Use libc++ (LLVM's C++ standard library)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
endif()

# Release-specific flags
if(CMAKE_BUILD_TYPE MATCHES "Release")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -DNDEBUG -g0")
endif()
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
    ${CMAKE_CURRENT_SOURCE_DIR}/c_api  # Add C API directory
    ${SFML_INCLUDE_DIR}
    ${Python3_INCLUDE_DIRS}
)

# Download and include nlohmann/json
include(FetchContent)
FetchContent_Declare(
    nlohmann_json
    URL https://github.com/nlohmann/json/releases/download/v3.11.2/json.tar.xz
)

FetchContent_MakeAvailable(nlohmann_json)

# Add source files for main simulation
set(SIMULATION_SOURCES
    src/agent.cpp
    src/grid.cpp
    src/simulation_controller.cpp
    src/simulation_report.cpp
    src/main.cpp
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
    include/simulation_controller.hpp
    include/simulation_report.hpp
    parameter_sweep/include/parameter_sweep.hpp
    parameter_sweep/include/sample_manager.hpp
    parameter_sweep/include/lhs_sampler.hpp
)

# C API sources
set(C_API_SOURCES
    c_api/simulation_c_api.cpp
    c_api/simulation_c_api.h
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

# Create C API shared library for Python
add_library(simulation_c_api SHARED ${C_API_SOURCES})
target_link_libraries(simulation_c_api PRIVATE predator_prey_lib)
target_include_directories(simulation_c_api PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Add compile definition to enable the export macro
target_compile_definitions(simulation_c_api PRIVATE SIMULATION_C_API_EXPORTS=1)

# Don't hide symbols for C API
set_target_properties(simulation_c_api PROPERTIES
    CXX_VISIBILITY_PRESET default
    C_VISIBILITY_PRESET default
    VISIBILITY_INLINES_HIDDEN OFF
    POSITION_INDEPENDENT_CODE ON
    MACOSX_RPATH ON
    # Set output name to match what the Python code expects
    OUTPUT_NAME "simulation_c_api"
    PREFIX "lib"  # Standard prefix for Unix-like systems
)

# For macOS, explicitly disable symbol hiding
if(APPLE)
    target_compile_options(simulation_c_api PRIVATE -fvisibility=default)
    set_target_properties(simulation_c_api PROPERTIES
        LINK_FLAGS "-Wl,-exported_symbols_list,${CMAKE_CURRENT_BINARY_DIR}/exports.list"
    )
    
    # Generate a list of exported symbols
    file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/exports.list
    "_simulation_create\n_simulation_initialize\n_simulation_step\n_simulation_run\n_simulation_pause\n_simulation_resume\n_simulation_end\n_simulation_get_status\n_simulation_get_results\n_simulation_free_results\n_simulation_destroy\n_simulation_reset_global_state\n"
)
endif()

# Disable LTO for the C API to avoid symbol visibility issues
if(CMAKE_INTERPROCEDURAL_OPTIMIZATION)
    set_target_properties(simulation_c_api PROPERTIES INTERPROCEDURAL_OPTIMIZATION OFF)
endif()

# Add compiler warnings
if(MSVC)
    target_compile_options(predator_prey PRIVATE /W4)
    target_compile_options(parameter_sweep PRIVATE /W4)
    target_compile_options(simulation_c_api PRIVATE /W4)
else()
    target_compile_options(predator_prey PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(parameter_sweep PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(simulation_c_api PRIVATE -Wall -Wextra -Wpedantic)
endif()

# Installation targets
install(TARGETS predator_prey predator_prey_lib parameter_sweep simulation_c_api
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)

# Install headers
install(FILES ${HEADERS} c_api/simulation_c_api.h
    DESTINATION include/predator_prey
)

# Install Python API files
install(FILES 
    python_api/predator_prey_sim.py
    python_api/example_simulation.py
    python_api/parameter_sweep.py
    python_api/README.md
    DESTINATION python_api
)

# Create a directory structure for the Python API
add_custom_target(setup_python_api
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/python_api
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${CMAKE_CURRENT_SOURCE_DIR}/python_api/predator_prey_sim.py
        ${CMAKE_CURRENT_SOURCE_DIR}/python_api/example_simulation.py
        ${CMAKE_CURRENT_SOURCE_DIR}/python_api/parameter_sweep.py
        ${CMAKE_BINARY_DIR}/python_api/
    COMMAND ${CMAKE_COMMAND} -E create_symlink
        ${CMAKE_BINARY_DIR}/lib/libsimulation_c_api${CMAKE_SHARED_LIBRARY_SUFFIX}
        ${CMAKE_BINARY_DIR}/python_api/libsimulation_c_api${CMAKE_SHARED_LIBRARY_SUFFIX}
    COMMENT "Setting up Python API directory"
)

# Make this run after building the C API
add_dependencies(setup_python_api simulation_c_api)