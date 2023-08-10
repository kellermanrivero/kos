cmake_minimum_required(VERSION 3.19)
enable_language(ASM C CXX)
project("VirtuOS")

add_subdirectory(boot)

add_executable(kernel.elf empty.c)
target_link_libraries(kernel.elf boot)
set_target_properties(kernel.elf PROPERTIES  LINK_FLAGS "-nostdlib -nostartfiles -ffreestanding -T ${CMAKE_SOURCE_DIR}/main.ld")

add_custom_command(
        OUTPUT virtuos.bin
        DEPENDS kernel.elf
        COMMENT "objcopy -O binary kernel.elf virtuos.bin"
        COMMAND ${CMAKE_OBJCOPY} ARGS -O binary kernel.elf virtuos.bin
)

add_custom_target(run ALL DEPENDS virtuos.bin)
#add_custom_command(TARGET run POST_BUILD COMMAND
#        qemu-system-aarch64 -M virt -cpu cortex-a57 -kernel virtuos.bin
#        COMMENT "Running QEMU...")