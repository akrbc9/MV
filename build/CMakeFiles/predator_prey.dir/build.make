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
CMAKE_COMMAND = /opt/homebrew/bin/cmake

# The command to remove a file.
RM = /opt/homebrew/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /Users/UM/Documents/UM25/Learning/MV

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /Users/UM/Documents/UM25/Learning/MV/build

# Include any dependencies generated for this target.
include CMakeFiles/predator_prey.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include CMakeFiles/predator_prey.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/predator_prey.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/predator_prey.dir/flags.make

CMakeFiles/predator_prey.dir/codegen:
.PHONY : CMakeFiles/predator_prey.dir/codegen

CMakeFiles/predator_prey.dir/src/main.cpp.o: CMakeFiles/predator_prey.dir/flags.make
CMakeFiles/predator_prey.dir/src/main.cpp.o: /Users/UM/Documents/UM25/Learning/MV/src/main.cpp
CMakeFiles/predator_prey.dir/src/main.cpp.o: CMakeFiles/predator_prey.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/Users/UM/Documents/UM25/Learning/MV/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/predator_prey.dir/src/main.cpp.o"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/predator_prey.dir/src/main.cpp.o -MF CMakeFiles/predator_prey.dir/src/main.cpp.o.d -o CMakeFiles/predator_prey.dir/src/main.cpp.o -c /Users/UM/Documents/UM25/Learning/MV/src/main.cpp

CMakeFiles/predator_prey.dir/src/main.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/predator_prey.dir/src/main.cpp.i"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/UM/Documents/UM25/Learning/MV/src/main.cpp > CMakeFiles/predator_prey.dir/src/main.cpp.i

CMakeFiles/predator_prey.dir/src/main.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/predator_prey.dir/src/main.cpp.s"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/UM/Documents/UM25/Learning/MV/src/main.cpp -o CMakeFiles/predator_prey.dir/src/main.cpp.s

# Object files for target predator_prey
predator_prey_OBJECTS = \
"CMakeFiles/predator_prey.dir/src/main.cpp.o"

# External object files for target predator_prey
predator_prey_EXTERNAL_OBJECTS =

bin/predator_prey: CMakeFiles/predator_prey.dir/src/main.cpp.o
bin/predator_prey: CMakeFiles/predator_prey.dir/build.make
bin/predator_prey: lib/libpredator_prey_lib.a
bin/predator_prey: /opt/homebrew/lib/libsfml-graphics.3.0.0.dylib
bin/predator_prey: /opt/homebrew/lib/libsfml-window.3.0.0.dylib
bin/predator_prey: /opt/homebrew/lib/libsfml-system.3.0.0.dylib
bin/predator_prey: CMakeFiles/predator_prey.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --bold --progress-dir=/Users/UM/Documents/UM25/Learning/MV/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable bin/predator_prey"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/predator_prey.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/predator_prey.dir/build: bin/predator_prey
.PHONY : CMakeFiles/predator_prey.dir/build

CMakeFiles/predator_prey.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/predator_prey.dir/cmake_clean.cmake
.PHONY : CMakeFiles/predator_prey.dir/clean

CMakeFiles/predator_prey.dir/depend:
	cd /Users/UM/Documents/UM25/Learning/MV/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /Users/UM/Documents/UM25/Learning/MV /Users/UM/Documents/UM25/Learning/MV /Users/UM/Documents/UM25/Learning/MV/build /Users/UM/Documents/UM25/Learning/MV/build /Users/UM/Documents/UM25/Learning/MV/build/CMakeFiles/predator_prey.dir/DependInfo.cmake "--color=$(COLOR)"
.PHONY : CMakeFiles/predator_prey.dir/depend

