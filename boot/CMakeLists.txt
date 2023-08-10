enable_language(ASM C)

set(BOOT_SOURCES
        boot.s
        start.c
)

add_library(boot STATIC ${BOOT_SOURCES})