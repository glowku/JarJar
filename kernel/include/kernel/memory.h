#ifndef JARJARVIS_MEMORY_H
#define JARJARVIS_MEMORY_H

#include <kernel/types.h>
#include <stdbool.h> // Indispensable pour le type 'bool'

#ifdef __cplusplus
extern "C" {
#endif

// Constantes de mémoire
#define PAGE_SIZE 4096
#define PAGE_SHIFT 12
#define PAGE_MASK (~(PAGE_SIZE - 1))

// Flags de pages
typedef enum {
    PAGE_PRESENT = 1 << 0,
    PAGE_WRITABLE = 1 << 1,
    PAGE_USER = 1 << 2,
    PAGE_WRITETHROUGH = 1 << 3,
    PAGE_NOCACHE = 1 << 4,
    PAGE_ACCESSED = 1 << 5,
    PAGE_DIRTY = 1 << 6,
    PAGE_HUGE = 1 << 7,
    PAGE_GLOBAL = 1 << 8,
    PAGE_NX = 1ULL << 63,
} page_flags_t;

// Structure de région mémoire (On la garde pour la compatibilité)
typedef struct mem_region {
    phys_addr_t base;
    u64 size;
    u32 type;
    struct mem_region* next;
} mem_region_t;

// Structure du gestionnaire de mémoire physique
typedef struct {
    u64 total_pages;
    u64 free_pages;
    u64 used_pages;
    u8* bitmap;           // Changé u64* en u8* pour simplifier l'accès
    phys_addr_t bitmap_phys;
    u64 bitmap_size;
} pmm_t;

// Structure du gestionnaire de mémoire virtuelle (Manquait pour le scheduler)
typedef struct vmm {
    u64* pml4;          // Table de niveau 4
    phys_addr_t pml4_phys;
} vmm_t;


// --- INITIALISATION ---
void memory_init(void);
void pmm_init(void);      // <--- LA MODIFICATION EST ICI (plus de 'regions')
void vmm_init(void);
void heap_init(void);

// Gestionnaire de mémoire physique (PMM)
phys_addr_t pmm_alloc_page(void);
phys_addr_t pmm_alloc_pages(u32 count);
void pmm_free_page(phys_addr_t addr);
void pmm_free_pages(phys_addr_t addr, u32 count);
u64 pmm_get_free_pages(void);
u64 pmm_get_total_pages(void);

// Heap
void* kmalloc(size_t size);
void kfree(void* ptr);

#ifdef __cplusplus
}
#endif

#endif