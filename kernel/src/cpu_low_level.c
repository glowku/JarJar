#include <kernel/types.h>

// Restaure l'état des interruptions (EFLAGS)
void interrupt_restore(u64 flags) {
    __asm__ volatile("push %0; popf" : : "rm"(flags) : "memory", "cc");
}

// Lit le registre CR3 (Table des pages actuelle)
u64 read_cr3(void) {
    u64 cr3;
    __asm__ volatile("mov %%cr3, %0" : "=r"(cr3));
    return cr3;
}

// Écrit dans le registre CR3 (Change de table des pages)
void write_cr3(u64 val) {
    __asm__ volatile("mov %0, %%cr3" : : "r"(val) : "memory");
}

// Invalide une entrée spécifique dans le cache TLB
void invlpg(u64 addr) {
    __asm__ volatile("invlpg (%0)" : : "r"(addr) : "memory");
}