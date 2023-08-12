ENTRY(__vos_start)
SECTIONS
{
    . = 0x40000000;
    dtb = .;
	. = . + 0x100000;
    .text : { *(.text) }
    . = ALIGN(16);
    . = . + 0x1000;
    stack_top = .;
}