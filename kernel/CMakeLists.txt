add_subdirectory(klibc)

set(KERNEL_SOURCES
        kmalloc.c
)

add_library(kernel STATIC ${KERNEL_SOURCES})
target_link_libraries(kernel PRIVATE klibc)