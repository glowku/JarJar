#ifndef JARJARVIS_SYMBOLS_H
#define JARJARVIS_SYMBOLS_H

#include <kernel/types.h>

#ifdef __cplusplus
extern "C" {
#endif

// Symbol table entry
typedef struct {
    u64 address;
    char name[256];
    u8 type;  // 0=function, 1=variable, 2=section
    u32 size;
} symbol_t;

// Symbol table
extern symbol_t kernel_symbols[];
extern u32 kernel_symbol_count;

// Functions
const char* symbol_lookup(u64 address, u64* offset);
u64 symbol_find_address(const char* name);
void symbol_dump_all(void);

// Stack trace
void stack_trace(void);
void stack_trace_from(u64 rbp, u64 rip);

#ifdef __cplusplus
}
#endif

#endif // JARJARVIS_SYMBOLS_H
