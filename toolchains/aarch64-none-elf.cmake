# the name of the target operating system
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR arm64)

set(KOS_TOOLCHAIN_ROOT $ENV{HOME}/.toolchains/aarch64-none-elf)
set(KOS_TOOLCHAIN_BIN ${KOS_TOOLCHAIN_ROOT}/bin)

# which compilers to use for C and C++
set(CMAKE_C_COMPILER   ${KOS_TOOLCHAIN_BIN}/aarch64-none-elf-gcc)
set(CMAKE_CXX_COMPILER ${KOS_TOOLCHAIN_BIN}/aarch64-none-elf-g++)
set(CMAKE_EXE_LINKER_FLAGS "--specs=nosys.specs")

set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)