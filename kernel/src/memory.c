#include <kernel/types.h>
#include <kernel/console.h>
#include <kernel/memory.h>
#include <limine.h>

// On garde la requête HHDM ici, c'est utile pour le VMM plus tard
static volatile struct limine_hhdm_request hhdm_req = {
    .id = LIMINE_HHDM_REQUEST,
    .revision = 0
};

// CETTE FONCTION SERA APPELÉE PAR TON MAIN.C
void memory_init(void) {
    // On initialise le gestionnaire de pages physiques
    pmm_init();
    
    // Si tu as un tas (heap) ou de la mémoire virtuelle, c'est ici :
    // vmm_init();
    // heap_init();
}