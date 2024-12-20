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
CMAKE_SOURCE_DIR = /home/nyereres/embedded/week5-project/pico-temp-fan

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/nyereres/embedded/week5-project/pico-temp-fan/build

# Utility rule file for dht_dht_pio_h.

# Include any custom commands dependencies for this target.
include pico_dht_build/CMakeFiles/dht_dht_pio_h.dir/compiler_depend.make

# Include the progress variables for this target.
include pico_dht_build/CMakeFiles/dht_dht_pio_h.dir/progress.make

pico_dht_build/CMakeFiles/dht_dht_pio_h: pico_dht_build/dht.pio.h

pico_dht_build/dht.pio.h: ../pico_dht/dht/dht.pio
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --blue --bold --progress-dir=/home/nyereres/embedded/week5-project/pico-temp-fan/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Generating dht.pio.h"
	cd /home/nyereres/embedded/week5-project/pico-temp-fan/build/pico_dht_build && ../pioasm-install/pioasm/pioasm -o c-sdk -v 0 /home/nyereres/embedded/week5-project/pico-temp-fan/pico_dht/dht/dht.pio /home/nyereres/embedded/week5-project/pico-temp-fan/build/pico_dht_build/dht.pio.h

dht_dht_pio_h: pico_dht_build/CMakeFiles/dht_dht_pio_h
dht_dht_pio_h: pico_dht_build/dht.pio.h
dht_dht_pio_h: pico_dht_build/CMakeFiles/dht_dht_pio_h.dir/build.make
.PHONY : dht_dht_pio_h

# Rule to build all files generated by this target.
pico_dht_build/CMakeFiles/dht_dht_pio_h.dir/build: dht_dht_pio_h
.PHONY : pico_dht_build/CMakeFiles/dht_dht_pio_h.dir/build

pico_dht_build/CMakeFiles/dht_dht_pio_h.dir/clean:
	cd /home/nyereres/embedded/week5-project/pico-temp-fan/build/pico_dht_build && $(CMAKE_COMMAND) -P CMakeFiles/dht_dht_pio_h.dir/cmake_clean.cmake
.PHONY : pico_dht_build/CMakeFiles/dht_dht_pio_h.dir/clean

pico_dht_build/CMakeFiles/dht_dht_pio_h.dir/depend:
	cd /home/nyereres/embedded/week5-project/pico-temp-fan/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/nyereres/embedded/week5-project/pico-temp-fan /home/nyereres/embedded/week5-project/pico-temp-fan/pico_dht/dht /home/nyereres/embedded/week5-project/pico-temp-fan/build /home/nyereres/embedded/week5-project/pico-temp-fan/build/pico_dht_build /home/nyereres/embedded/week5-project/pico-temp-fan/build/pico_dht_build/CMakeFiles/dht_dht_pio_h.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : pico_dht_build/CMakeFiles/dht_dht_pio_h.dir/depend

