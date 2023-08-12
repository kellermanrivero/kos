add_subdirectory(klibc)
add_subdirectory(dtb)

set(KERNEL_SOURCES
        kmalloc.c
)

add_library(kernel STATIC ${KERNEL_SOURCES})
target_link_libraries(kernel PRIVATE klibc dtb)