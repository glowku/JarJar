OUTPUT_FORMAT(elf64-x86-64)
ENTRY(kmain)

SECTIONS
{
    /* On se place à 2Mo au lieu de l'adresse Higher Half complexe */
    . = 2M;

    .text : {
        *(.text*)
    }

    .limine_reqs : {
        KEEP(*(.limine_reqs))
    }

    .rodata : {
        *(.rodata*)
    }

    .data : {
        *(.data*)
    }

    .bss : {
        *(.bss*)
    }
}