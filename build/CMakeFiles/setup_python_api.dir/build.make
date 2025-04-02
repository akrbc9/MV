# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.31

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:

# Disable VCS-based implicit rules.
% : %,v

# Disable VCS-based implicit rules.
% : RCS/%

# Disable VCS-based implicit rules.
% : RCS/%,v

# Disable VCS-based implicit rules.
% : SCCS/s.%

# Disable VCS-based implicit rules.
% : s.%

.SUFFIXES: .hpux_make_needs_suffix_list

# Command-line flag to silence nested $(MAKE).
$(VERBOSE)MAKESILENT = -s

#Suppress display of executed commands.
$(VERBOSE).SILENT:

# A target that is always out of date.
cmake_force:
.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /opt/anaconda3/envs/CMPLX/bin/cmake

# The command to remove a file.
RM = /opt/anaconda3/envs/CMPLX/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /Users/UM/Documents/UM25/Learning/MV

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /Users/UM/Documents/UM25/Learning/MV/build

# Utility rule file for setup_python_api.

# Include any custom commands dependencies for this target.
include CMakeFiles/setup_python_api.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/setup_python_api.dir/progress.make

CMakeFiles/setup_python_api:
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --blue --bold --progress-dir=/Users/UM/Documents/UM25/Learning/MV/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Setting up Python API directory"
	/opt/anaconda3/envs/CMPLX/bin/cmake -E make_directory /Users/UM/Documents/UM25/Learning/MV/build/python_api
	/opt/anaconda3/envs/CMPLX/bin/cmake -E copy_if_different /Users/UM/Documents/UM25/Learning/MV/python_api/predator_prey_sim.py /Users/UM/Documents/UM25/Learning/MV/python_api/example_simulation.py /Users/UM/Documents/UM25/Learning/MV/python_api/parameter_sweep.py /Users/UM/Documents/UM25/Learning/MV/build/python_api/
	/opt/anaconda3/envs/CMPLX/bin/cmake -E create_symlink /Users/UM/Documents/UM25/Learning/MV/build/lib/libsimulation_c_api.dylib /Users/UM/Documents/UM25/Learning/MV/build/python_api/libsimulation_c_api.dylib

CMakeFiles/setup_python_api.dir/codegen:
.PHONY : CMakeFiles/setup_python_api.dir/codegen

setup_python_api: CMakeFiles/setup_python_api
setup_python_api: CMakeFiles/setup_python_api.dir/build.make
.PHONY : setup_python_api

# Rule to build all files generated by this target.
CMakeFiles/setup_python_api.dir/build: setup_python_api
.PHONY : CMakeFiles/setup_python_api.dir/build

CMakeFiles/setup_python_api.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/setup_python_api.dir/cmake_clean.cmake
.PHONY : CMakeFiles/setup_python_api.dir/clean

CMakeFiles/setup_python_api.dir/depend:
	cd /Users/UM/Documents/UM25/Learning/MV/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /Users/UM/Documents/UM25/Learning/MV /Users/UM/Documents/UM25/Learning/MV /Users/UM/Documents/UM25/Learning/MV/build /Users/UM/Documents/UM25/Learning/MV/build /Users/UM/Documents/UM25/Learning/MV/build/CMakeFiles/setup_python_api.dir/DependInfo.cmake "--color=$(COLOR)"
.PHONY : CMakeFiles/setup_python_api.dir/depend

