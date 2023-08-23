.global __kos_start
__kos_start:
    // Setup the stack
    ldr x30, =stack_top // Load into X30 the address of the stack
    mov sp, x30         // Mov X30 into SP (Stack Pointer)
    mov x18, 0x2905     // Magic number for debug
    bl __kos_main
    b .
