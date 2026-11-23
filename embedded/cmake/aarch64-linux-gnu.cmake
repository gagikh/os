# Toolchain file: build on an x86_64 Linux PC, run on a 64-bit ARM board
# (Jetson Orin/Nano, Raspberry Pi 4/5 with a 64-bit OS, i.MX8 HummingBoard).
#
# Install the cross compiler once (Ubuntu / Debian / WSL2):
#     sudo apt install g++-aarch64-linux-gnu
#
# Use:
#     cmake -S . -B build-arm64 -DCMAKE_TOOLCHAIN_FILE=cmake/aarch64-linux-gnu.cmake
#
# This links against the cross compiler's own glibc. If a program fails on the
# board with "GLIBC_2.xx not found", the PC's toolchain is newer than the board's
# OS: build on the board instead, or point CMAKE_SYSROOT at a copy of the board's
# root filesystem (Day 2 shows how).

set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR aarch64)

set(CMAKE_C_COMPILER   aarch64-linux-gnu-gcc)
set(CMAKE_CXX_COMPILER aarch64-linux-gnu-g++)

# Look for libraries and headers only in the target environment,
# but keep using the PC's own programs (cmake, make, ...).
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)
