# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.29

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
CMAKE_COMMAND = /snap/clion/296/bin/cmake/linux/x64/bin/cmake

# The command to remove a file.
RM = /snap/clion/296/bin/cmake/linux/x64/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/lekhacvuong/Desktop/my_library

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/lekhacvuong/Desktop/my_library/cmake-build-debug

# Include any dependencies generated for this target.
include common/modbus/mb_slave/CMakeFiles/mb_slave.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include common/modbus/mb_slave/CMakeFiles/mb_slave.dir/compiler_depend.make

# Include the progress variables for this target.
include common/modbus/mb_slave/CMakeFiles/mb_slave.dir/progress.make

# Include the compile flags for this target's objects.
include common/modbus/mb_slave/CMakeFiles/mb_slave.dir/flags.make

common/modbus/mb_slave/CMakeFiles/mb_slave.dir/sm_mb_slave_impl.c.o: common/modbus/mb_slave/CMakeFiles/mb_slave.dir/flags.make
common/modbus/mb_slave/CMakeFiles/mb_slave.dir/sm_mb_slave_impl.c.o: /home/lekhacvuong/Desktop/my_library/common/modbus/mb_slave/sm_mb_slave_impl.c
common/modbus/mb_slave/CMakeFiles/mb_slave.dir/sm_mb_slave_impl.c.o: common/modbus/mb_slave/CMakeFiles/mb_slave.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/home/lekhacvuong/Desktop/my_library/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object common/modbus/mb_slave/CMakeFiles/mb_slave.dir/sm_mb_slave_impl.c.o"
	cd /home/lekhacvuong/Desktop/my_library/cmake-build-debug/common/modbus/mb_slave && /usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT common/modbus/mb_slave/CMakeFiles/mb_slave.dir/sm_mb_slave_impl.c.o -MF CMakeFiles/mb_slave.dir/sm_mb_slave_impl.c.o.d -o CMakeFiles/mb_slave.dir/sm_mb_slave_impl.c.o -c /home/lekhacvuong/Desktop/my_library/common/modbus/mb_slave/sm_mb_slave_impl.c

common/modbus/mb_slave/CMakeFiles/mb_slave.dir/sm_mb_slave_impl.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing C source to CMakeFiles/mb_slave.dir/sm_mb_slave_impl.c.i"
	cd /home/lekhacvuong/Desktop/my_library/cmake-build-debug/common/modbus/mb_slave && /usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/lekhacvuong/Desktop/my_library/common/modbus/mb_slave/sm_mb_slave_impl.c > CMakeFiles/mb_slave.dir/sm_mb_slave_impl.c.i

common/modbus/mb_slave/CMakeFiles/mb_slave.dir/sm_mb_slave_impl.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling C source to assembly CMakeFiles/mb_slave.dir/sm_mb_slave_impl.c.s"
	cd /home/lekhacvuong/Desktop/my_library/cmake-build-debug/common/modbus/mb_slave && /usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/lekhacvuong/Desktop/my_library/common/modbus/mb_slave/sm_mb_slave_impl.c -o CMakeFiles/mb_slave.dir/sm_mb_slave_impl.c.s

common/modbus/mb_slave/CMakeFiles/mb_slave.dir/__/__/__/libs/nanoModbus/nanomodbus.c.o: common/modbus/mb_slave/CMakeFiles/mb_slave.dir/flags.make
common/modbus/mb_slave/CMakeFiles/mb_slave.dir/__/__/__/libs/nanoModbus/nanomodbus.c.o: /home/lekhacvuong/Desktop/my_library/libs/nanoModbus/nanomodbus.c
common/modbus/mb_slave/CMakeFiles/mb_slave.dir/__/__/__/libs/nanoModbus/nanomodbus.c.o: common/modbus/mb_slave/CMakeFiles/mb_slave.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/home/lekhacvuong/Desktop/my_library/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building C object common/modbus/mb_slave/CMakeFiles/mb_slave.dir/__/__/__/libs/nanoModbus/nanomodbus.c.o"
	cd /home/lekhacvuong/Desktop/my_library/cmake-build-debug/common/modbus/mb_slave && /usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT common/modbus/mb_slave/CMakeFiles/mb_slave.dir/__/__/__/libs/nanoModbus/nanomodbus.c.o -MF CMakeFiles/mb_slave.dir/__/__/__/libs/nanoModbus/nanomodbus.c.o.d -o CMakeFiles/mb_slave.dir/__/__/__/libs/nanoModbus/nanomodbus.c.o -c /home/lekhacvuong/Desktop/my_library/libs/nanoModbus/nanomodbus.c

common/modbus/mb_slave/CMakeFiles/mb_slave.dir/__/__/__/libs/nanoModbus/nanomodbus.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing C source to CMakeFiles/mb_slave.dir/__/__/__/libs/nanoModbus/nanomodbus.c.i"
	cd /home/lekhacvuong/Desktop/my_library/cmake-build-debug/common/modbus/mb_slave && /usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/lekhacvuong/Desktop/my_library/libs/nanoModbus/nanomodbus.c > CMakeFiles/mb_slave.dir/__/__/__/libs/nanoModbus/nanomodbus.c.i

common/modbus/mb_slave/CMakeFiles/mb_slave.dir/__/__/__/libs/nanoModbus/nanomodbus.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling C source to assembly CMakeFiles/mb_slave.dir/__/__/__/libs/nanoModbus/nanomodbus.c.s"
	cd /home/lekhacvuong/Desktop/my_library/cmake-build-debug/common/modbus/mb_slave && /usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/lekhacvuong/Desktop/my_library/libs/nanoModbus/nanomodbus.c -o CMakeFiles/mb_slave.dir/__/__/__/libs/nanoModbus/nanomodbus.c.s

# Object files for target mb_slave
mb_slave_OBJECTS = \
"CMakeFiles/mb_slave.dir/sm_mb_slave_impl.c.o" \
"CMakeFiles/mb_slave.dir/__/__/__/libs/nanoModbus/nanomodbus.c.o"

# External object files for target mb_slave
mb_slave_EXTERNAL_OBJECTS =

common/modbus/mb_slave/libmb_slave.a: common/modbus/mb_slave/CMakeFiles/mb_slave.dir/sm_mb_slave_impl.c.o
common/modbus/mb_slave/libmb_slave.a: common/modbus/mb_slave/CMakeFiles/mb_slave.dir/__/__/__/libs/nanoModbus/nanomodbus.c.o
common/modbus/mb_slave/libmb_slave.a: common/modbus/mb_slave/CMakeFiles/mb_slave.dir/build.make
common/modbus/mb_slave/libmb_slave.a: common/modbus/mb_slave/CMakeFiles/mb_slave.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --bold --progress-dir=/home/lekhacvuong/Desktop/my_library/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Linking C static library libmb_slave.a"
	cd /home/lekhacvuong/Desktop/my_library/cmake-build-debug/common/modbus/mb_slave && $(CMAKE_COMMAND) -P CMakeFiles/mb_slave.dir/cmake_clean_target.cmake
	cd /home/lekhacvuong/Desktop/my_library/cmake-build-debug/common/modbus/mb_slave && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/mb_slave.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
common/modbus/mb_slave/CMakeFiles/mb_slave.dir/build: common/modbus/mb_slave/libmb_slave.a
.PHONY : common/modbus/mb_slave/CMakeFiles/mb_slave.dir/build

common/modbus/mb_slave/CMakeFiles/mb_slave.dir/clean:
	cd /home/lekhacvuong/Desktop/my_library/cmake-build-debug/common/modbus/mb_slave && $(CMAKE_COMMAND) -P CMakeFiles/mb_slave.dir/cmake_clean.cmake
.PHONY : common/modbus/mb_slave/CMakeFiles/mb_slave.dir/clean

common/modbus/mb_slave/CMakeFiles/mb_slave.dir/depend:
	cd /home/lekhacvuong/Desktop/my_library/cmake-build-debug && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/lekhacvuong/Desktop/my_library /home/lekhacvuong/Desktop/my_library/common/modbus/mb_slave /home/lekhacvuong/Desktop/my_library/cmake-build-debug /home/lekhacvuong/Desktop/my_library/cmake-build-debug/common/modbus/mb_slave /home/lekhacvuong/Desktop/my_library/cmake-build-debug/common/modbus/mb_slave/CMakeFiles/mb_slave.dir/DependInfo.cmake "--color=$(COLOR)"
.PHONY : common/modbus/mb_slave/CMakeFiles/mb_slave.dir/depend

