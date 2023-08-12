enable_language(ASM C)

set(DTB_SOURCES
        dtb.c
)

add_library(dtb STATIC ${DTB_SOURCES})