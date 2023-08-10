enable_language(ASM C)

set(KLIBC_SOURCES
        stdlib.c
)

add_library(klibc STATIC ${KLIBC_SOURCES})