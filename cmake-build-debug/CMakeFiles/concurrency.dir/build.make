# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.22

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
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/michael/code/c/concurrency

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/michael/code/c/concurrency/cmake-build-debug

# Include any dependencies generated for this target.
include CMakeFiles/concurrency.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include CMakeFiles/concurrency.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/concurrency.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/concurrency.dir/flags.make

CMakeFiles/concurrency.dir/main.c.o: CMakeFiles/concurrency.dir/flags.make
CMakeFiles/concurrency.dir/main.c.o: ../main.c
CMakeFiles/concurrency.dir/main.c.o: CMakeFiles/concurrency.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/michael/code/c/concurrency/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object CMakeFiles/concurrency.dir/main.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT CMakeFiles/concurrency.dir/main.c.o -MF CMakeFiles/concurrency.dir/main.c.o.d -o CMakeFiles/concurrency.dir/main.c.o -c /home/michael/code/c/concurrency/main.c

CMakeFiles/concurrency.dir/main.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/concurrency.dir/main.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/michael/code/c/concurrency/main.c > CMakeFiles/concurrency.dir/main.c.i

CMakeFiles/concurrency.dir/main.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/concurrency.dir/main.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/michael/code/c/concurrency/main.c -o CMakeFiles/concurrency.dir/main.c.s

CMakeFiles/concurrency.dir/occ.c.o: CMakeFiles/concurrency.dir/flags.make
CMakeFiles/concurrency.dir/occ.c.o: ../occ.c
CMakeFiles/concurrency.dir/occ.c.o: CMakeFiles/concurrency.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/michael/code/c/concurrency/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building C object CMakeFiles/concurrency.dir/occ.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT CMakeFiles/concurrency.dir/occ.c.o -MF CMakeFiles/concurrency.dir/occ.c.o.d -o CMakeFiles/concurrency.dir/occ.c.o -c /home/michael/code/c/concurrency/occ.c

CMakeFiles/concurrency.dir/occ.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/concurrency.dir/occ.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/michael/code/c/concurrency/occ.c > CMakeFiles/concurrency.dir/occ.c.i

CMakeFiles/concurrency.dir/occ.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/concurrency.dir/occ.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/michael/code/c/concurrency/occ.c -o CMakeFiles/concurrency.dir/occ.c.s

CMakeFiles/concurrency.dir/operation.c.o: CMakeFiles/concurrency.dir/flags.make
CMakeFiles/concurrency.dir/operation.c.o: ../operation.c
CMakeFiles/concurrency.dir/operation.c.o: CMakeFiles/concurrency.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/michael/code/c/concurrency/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Building C object CMakeFiles/concurrency.dir/operation.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT CMakeFiles/concurrency.dir/operation.c.o -MF CMakeFiles/concurrency.dir/operation.c.o.d -o CMakeFiles/concurrency.dir/operation.c.o -c /home/michael/code/c/concurrency/operation.c

CMakeFiles/concurrency.dir/operation.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/concurrency.dir/operation.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/michael/code/c/concurrency/operation.c > CMakeFiles/concurrency.dir/operation.c.i

CMakeFiles/concurrency.dir/operation.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/concurrency.dir/operation.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/michael/code/c/concurrency/operation.c -o CMakeFiles/concurrency.dir/operation.c.s

# Object files for target concurrency
concurrency_OBJECTS = \
"CMakeFiles/concurrency.dir/main.c.o" \
"CMakeFiles/concurrency.dir/occ.c.o" \
"CMakeFiles/concurrency.dir/operation.c.o"

# External object files for target concurrency
concurrency_EXTERNAL_OBJECTS =

concurrency: CMakeFiles/concurrency.dir/main.c.o
concurrency: CMakeFiles/concurrency.dir/occ.c.o
concurrency: CMakeFiles/concurrency.dir/operation.c.o
concurrency: CMakeFiles/concurrency.dir/build.make
concurrency: CMakeFiles/concurrency.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/michael/code/c/concurrency/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "Linking C executable concurrency"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/concurrency.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/concurrency.dir/build: concurrency
.PHONY : CMakeFiles/concurrency.dir/build

CMakeFiles/concurrency.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/concurrency.dir/cmake_clean.cmake
.PHONY : CMakeFiles/concurrency.dir/clean

CMakeFiles/concurrency.dir/depend:
	cd /home/michael/code/c/concurrency/cmake-build-debug && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/michael/code/c/concurrency /home/michael/code/c/concurrency /home/michael/code/c/concurrency/cmake-build-debug /home/michael/code/c/concurrency/cmake-build-debug /home/michael/code/c/concurrency/cmake-build-debug/CMakeFiles/concurrency.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/concurrency.dir/depend

