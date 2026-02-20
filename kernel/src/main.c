#include "limine/limine.h"
#include <stdint.h>
#include <stddef.h>

__attribute__((section(".limine_reqs")))
static volatile struct limine_framebuffer_request framebuffer_request = {
    .id = LIMINE_FRAMEBUFFER_REQUEST,
    .revision = 0
};

// Fonction de survie : écrit un caractère sur le port série
void outb(uint16_t port, uint8_t val) {
    __asm__ volatile ("outb %0, %1" : : "a"(val), "Nd"(port));
}

void kmain(void) {
    // On envoie "OK" sur le port série COM1 (0x3f8)
    outb(0x3f8, 'O');
    outb(0x3f8, 'K');
    outb(0x3f8, '\n');

    if (framebuffer_request.response == NULL || framebuffer_request.response->framebuffer_count < 1) {
        outb(0x3f8, 'E'); // E pour Erreur
        for (;;) { __asm__("hlt"); }
    }

    struct limine_framebuffer *fb = framebuffer_request.response->framebuffers[0];
    uint32_t *fb_ptr = (uint32_t *)fb->address;
    for (size_t i = 0; i < 500 * 500; i++) fb_ptr[i] = 0xFFFFFFFF;

    for (;;) { __asm__("hlt"); }
}