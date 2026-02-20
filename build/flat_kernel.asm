; flat_kernel.asm
[BITS 64]
org 0x100000

; On inclut l'en-tête et le boot
incbin "CMakeFiles/jar_elf.dir/kernel/arch/x86_64/boot.asm.obj"

; On inclut les bibliothèques (Vérifie bien les noms dans tes dossiers !)
incbin "kernel/libkernel.a"
incbin "ai_core/libai_core.a"
incbin "drivers/libdrivers.a"
incbin "filesystem/libfilesystem.a"
incbin "libc/liblibc.a" ; <--- C'était ici l'erreur (liblibc.a au lieu de libliblibc.a)