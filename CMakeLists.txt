cmake_minimum_required(VERSION 3.19)
enable_language(ASM C CXX)
project("K OS")

# Use C99
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED TRUE)

# Prevents use of floating point registers (since they are not ready)
add_compile_options(-mgeneral-regs-only)

include_directories(include)
add_subdirectory(boot)
add_subdirectory(kernel)
add_executable(kernel.elf empty.c)
target_link_libraries(kernel.elf boot kernel)
set_target_properties(kernel.elf PROPERTIES LINK_FLAGS "-nostdlib -nostartfiles -ffreestanding -T ${CMAKE_SOURCE_DIR}/main.ld")

add_custom_command(
        OUTPUT kos.bin
        DEPENDS kernel.elf
        COMMENT "objcopy -O binary kernel.elf kos.bin"
        COMMAND ${CMAKE_OBJCOPY} ARGS -O binary kernel.elf kos.bin
)

add_custom_target(run ALL DEPENDS kernel.elf)
add_custom_command(TARGET run POST_BUILD COMMAND
        qemu-system-aarch64 -M virt -cpu cortex-a57 -kernel kernel.elf -S -s
        COMMENT "Running QEMU...")