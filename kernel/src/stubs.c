#include <kernel/types.h>
#include <stddef.h>

void panic(const char* message) { for(;;); }
void scheduler_init() {}
void scheduler_enable() {}
void process_sleep(int ms) { (void)ms; }
void interrupt_init() {}
void drivers_init() {}
void draw_hex_grid() {}
void draw_tech_border() {}
void jarvis_self_learn_init() {}
void jarvis_shell_main() { for(;;); }

// LibC Stubs
float cosf(float x) { return x; }
float sinf(float x) { return x; }

// Allocation
void* kmalloc(size_t size) { (void)size; return (void*)0; }
void kfree(void* ptr) { (void)ptr; }