#include <kernel/memory.h>
#include <kernel/console.h>
#include <kernel/panic.h>
#include <limine.h>

// Prototypes internes
static u64 read_msr(u32 msr);
static void write_msr(u32 msr, u64 value);
void paging_enable_nx(void); // Déclaration pour supprimer le warning

// Indices
#define PML4_INDEX(addr) (((addr) >> 39) & 0x1FF)
#define PDPT_INDEX(addr) (((addr) >> 30) & 0x1FF)
#define PD_INDEX(addr) (((addr) >> 21) & 0x1FF)
#define PT_INDEX(addr) (((addr) >> 12) & 0x1FF)

static vmm_t kernel_vmm;

// Importation du HHDM offset via Limine pour le VMM
static volatile struct limine_hhdm_request vmm_hhdm_request = {
    .id = LIMINE_HHDM_REQUEST, .revision = 0
};

extern void invlpg(virt_addr_t addr);
extern phys_addr_t read_cr3(void);
extern void write_cr3(phys_addr_t addr);

void vmm_init(void) {
    console_printf("[VMM] Setup...\n");
    
    kernel_vmm.pml4_phys = read_cr3();
    kernel_vmm.pml4 = (u64*)PHYS_TO_VIRT(kernel_vmm.pml4_phys);
    
    paging_enable_nx();
}

vmm_t* vmm_create(void) {
    vmm_t* vmm = kmalloc(sizeof(vmm_t));
    if (!vmm) return NULL;
    
    vmm->pml4_phys = pmm_alloc_page();
    if (!vmm->pml4_phys) {
        kfree(vmm);
        return NULL;
    }
    
    vmm->pml4 = (u64*)PHYS_TO_VIRT(vmm->pml4_phys);
    
    // Copie du Higher Half du noyau
    for (int i = 256; i < 512; i++) vmm->pml4[i] = kernel_vmm.pml4[i];
    for (int i = 0; i < 256; i++) vmm->pml4[i] = 0;
    
    return vmm;
}

static u64* get_or_create_table(u64* parent, int index, u64 flags) {
    if (parent[index] & PAGE_PRESENT) {
        return (u64*)PHYS_TO_VIRT(parent[index] & PAGE_MASK);
    }
    
    phys_addr_t page = pmm_alloc_page();
    if (!page) return NULL;
    
    parent[index] = page | flags | PAGE_PRESENT;
    u64* table = (u64*)PHYS_TO_VIRT(page);
    for (int i = 0; i < 512; i++) table[i] = 0;
    
    return table;
}

jar_result_t vmm_map(vmm_t* vmm, virt_addr_t virt, phys_addr_t phys, u64 flags) {
    if (!vmm) return JAR_INVAL;
    
    u64* pdpt = get_or_create_table(vmm->pml4, PML4_INDEX(virt), flags);
    u64* pd   = get_or_create_table(pdpt, PDPT_INDEX(virt), flags);
    u64* pt   = get_or_create_table(pd, PD_INDEX(virt), flags);
    
    if (!pt) return JAR_NOMEM;
    
    pt[PT_INDEX(virt)] = phys | flags | PAGE_PRESENT;
    invlpg(virt);
    
    return JAR_OK;
}

void vmm_switch(vmm_t* vmm) {
    if (vmm) write_cr3(vmm->pml4_phys);
}

void paging_enable_nx(void) {
    u64 efer = read_msr(0xC0000080);
    efer |= (1 << 11);
    write_msr(0xC0000080, efer);
}

static u64 read_msr(u32 msr) {
    u32 low, high;
    __asm__ volatile ("rdmsr" : "=a"(low), "=d"(high) : "c"(msr));
    return ((u64)high << 32) | low;
}

static void write_msr(u32 msr, u64 value) {
    u32 low = value & 0xFFFFFFFF;
    u32 high = value >> 32;
    __asm__ volatile ("wrmsr" : : "c"(msr), "a"(low), "d"(high));
}