#ifndef JARJARVIS_INTERRUPT_H
#define JARJARVIS_INTERRUPT_H

#include <kernel/types.h>

#ifdef __cplusplus
extern "C" {
#endif

// Vecteurs d'interruption
typedef enum {
    // Exceptions CPU
    INT_DIVIDE_ERROR = 0,
    INT_DEBUG = 1,
    INT_NMI = 2,
    INT_BREAKPOINT = 3,
    INT_OVERFLOW = 4,
    INT_BOUND_RANGE = 5,
    INT_INVALID_OPCODE = 6,
    INT_DEVICE_NOT_AVAIL = 7,
    INT_DOUBLE_FAULT = 8,
    INT_COPROCESSOR = 9,
    INT_INVALID_TSS = 10,
    INT_SEGMENT_NOT_PRESENT = 11,
    INT_STACK_FAULT = 12,
    INT_GENERAL_PROTECTION = 13,
    INT_PAGE_FAULT = 14,
    INT_RESERVED_15 = 15,
    INT_FPU_ERROR = 16,
    INT_ALIGNMENT_CHECK = 17,
    INT_MACHINE_CHECK = 18,
    INT_SIMD_EXCEPTION = 19,
    INT_VIRTUALIZATION = 20,
    INT_SECURITY = 30,
    
    // IRQs (32-47)
    INT_IRQ0 = 32,
    INT_IRQ1 = 33,
    INT_IRQ2 = 34,
    INT_IRQ3 = 35,
    INT_IRQ4 = 36,
    INT_IRQ5 = 37,
    INT_IRQ6 = 38,
    INT_IRQ7 = 39,
    INT_IRQ8 = 40,
    INT_IRQ9 = 41,
    INT_IRQ10 = 42,
    INT_IRQ11 = 43,
    INT_IRQ12 = 44,
    INT_IRQ13 = 45,
    INT_IRQ14 = 46,
    INT_IRQ15 = 47,
    
    // Interruptions système
    INT_SYSCALL = 128,
    INT_IPI = 129,
    INT_TIMER = 130,
    INT_SPURIOUS = 255,
} interrupt_vector_t;

// Structure du contexte CPU
typedef struct {
    u64 rax, rbx, rcx, rdx;
    u64 rsi, rdi, rbp, rsp;
    u64 r8, r9, r10, r11;
    u64 r12, r13, r14, r15;
    u64 rip, rflags;
    u64 cs, ss, ds, es, fs, gs;
    u64 cr3;
} cpu_context_t;

// Structure de la stack frame d'interruption
typedef struct {
    u64 rip;
    u64 cs;
    u64 rflags;
    u64 rsp;
    u64 ss;
} interrupt_frame_t;

// Type de handler d'interruption
typedef void (*interrupt_handler_t)(interrupt_frame_t* frame);
typedef void (*exception_handler_t)(interrupt_frame_t* frame, u64 error_code);

// IDT Entry
typedef struct {
    u16 offset_low;
    u16 selector;
    u8 ist;
    u8 type_attr;
    u16 offset_mid;
    u32 offset_high;
    u32 reserved;
} __packed idt_entry_t;

// IDT Pointer
typedef struct {
    u16 limit;
    u64 base;
} __packed idt_pointer_t;

// Initialisation
void interrupt_init(void);
void idt_init(void);
void pic_init(void);
void apic_init(void);
void ioapic_init(void);

// Gestion des handlers
void interrupt_register_handler(u8 vector, interrupt_handler_t handler);
void interrupt_unregister_handler(u8 vector);
void exception_register_handler(u8 vector, exception_handler_t handler);

// Contrôle des interruptions
void interrupt_enable(void);
void interrupt_disable(void);
bool interrupt_save(void);
void interrupt_restore(bool state);
void interrupt_ack(u8 vector);

// IRQs
void irq_enable(u8 irq);
void irq_disable(u8 irq);
void irq_set_mask(u8 irq);
void irq_clear_mask(u8 irq);
u16 irq_get_mask(void);

// APIC
void apic_send_eoi(void);
u32 apic_get_id(void);
void apic_send_ipi(u32 apic_id, u8 vector);
void apic_timer_init(u32 initial_count);
u32 apic_timer_get_count(void);

// Syscalls
void syscall_init(void);
u64 syscall_handler(u64 num, u64 arg1, u64 arg2, u64 arg3, u64 arg4, u64 arg5);

// Informations
typedef struct {
    u64 total_interrupts;
    u64 spurious_count;
    u64 unhandled_count;
    u64 syscall_count;
} interrupt_stats_t;

void interrupt_get_stats(interrupt_stats_t* stats);

#ifdef __cplusplus
}
#endif

#endif // JARJARVIS_INTERRUPT_H
