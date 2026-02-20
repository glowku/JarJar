#ifndef JARJARVIS_PANIC_H
#define JARJARVIS_PANIC_H

#include <kernel/types.h>

#ifdef __cplusplus
extern "C" {
#endif

// Panic reason codes
typedef enum {
    PANIC_GENERIC = 0,
    PANIC_DIVISION_BY_ZERO,
    PANIC_PAGE_FAULT,
    PANIC_GENERAL_PROTECTION,
    PANIC_STACK_OVERFLOW,
    PANIC_OUT_OF_MEMORY,
    PANIC_ASSERT_FAILED,
    PANIC_UNHANDLED_INTERRUPT,
    PANIC_DOUBLE_FAULT,
    PANIC_MACHINE_CHECK,
    PANIC_NMI,
    PANIC_INVALID_OPCODE,
    PANIC_KERNEL_STACK_CORRUPTED,
    PANIC_HEAP_CORRUPTED,
    PANIC_DEADLOCK_DETECTED,
    PANIC_AI_ERROR,
} panic_reason_t;

// Panic handler
__noreturn void panic(const char* message);
__noreturn void panic_reason(panic_reason_t reason, const char* message);
__noreturn void panic_assert(const char* file, int line, const char* expression);

// Debug information
void panic_dump_registers(void);
void panic_dump_stack(void);
void panic_dump_memory_map(void);

// Stack trace
void panic_print_stack_trace(void);

// Assertion macros
#ifdef DEBUG
    #define assert(expr) \
        do { \
            if (!(expr)) { \
                panic_assert(__FILE__, __LINE__, #expr); \
            } \
        } while (0)
#else
    #define assert(expr) ((void)0)
#endif

#define ASSERT_NOT_NULL(ptr) assert((ptr) != NULL)
#define ASSERT_VALID_POINTER(ptr) assert((ptr) != NULL && (uintptr_t)(ptr) >= 0x1000)
#define ASSERT_ALIGNED(ptr, alignment) assert(((uintptr_t)(ptr) & ((alignment) - 1)) == 0)

#ifdef __cplusplus
}
#endif

#endif // JARJARVIS_PANIC_H
