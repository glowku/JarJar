#ifndef JARJARVIS_CONSOLE_H
#define JARJARVIS_CONSOLE_H

#include <kernel/types.h>

#ifdef __cplusplus
extern "C" {
#endif

// Couleurs VGA
typedef enum {
    VGA_BLACK = 0,
    VGA_BLUE = 1,
    VGA_GREEN = 2,
    VGA_CYAN = 3,
    VGA_RED = 4,
    VGA_MAGENTA = 5,
    VGA_BROWN = 6,
    VGA_LIGHT_GREY = 7,
    VGA_DARK_GREY = 8,
    VGA_LIGHT_BLUE = 9,
    VGA_LIGHT_GREEN = 10,
    VGA_LIGHT_CYAN = 11,
    VGA_LIGHT_RED = 12,
    VGA_LIGHT_MAGENTA = 13,
    VGA_LIGHT_BROWN = 14,
    VGA_WHITE = 15,
} vga_color_t;

// Structure de la console
typedef struct {
    u16* buffer;
    u32 width;
    u32 height;
    u32 cursor_x;
    u32 cursor_y;
    u8 color;
    bool cursor_enabled;
    bool scroll_enabled;
} console_t;

// Fonctions de base
void console_init(void);
void console_clear(void);
void console_putchar(char c);
void console_puts(const char* str);
void console_printf(const char* fmt, ...);
void console_vprintf(const char* fmt, va_list args);

// Couleurs
void console_set_color(vga_color_t fg, vga_color_t bg);
void console_set_foreground(vga_color_t color);
void console_set_background(vga_color_t color);
void console_reset_color(void);

// Curseur
void console_enable_cursor(void);
void console_disable_cursor(void);
void console_set_cursor_pos(u32 x, u32 y);
void console_get_cursor_pos(u32* x, u32* y);

// Défilement
void console_scroll_up(void);
void console_scroll_down(void);

// Couleurs spéciales pour l'IA
void console_ai_output(void);   // Vert pour sortie IA
void console_ai_thinking(void); // Bleu pour réflexion IA
void console_ai_warning(void);  // Jaune pour avertissements
void console_ai_error(void);    // Rouge pour erreurs

// Buffer de la console
console_t* console_get_active(void);
void console_switch(console_t* console);

#ifdef __cplusplus
}
#endif

#endif // JARJARVIS_CONSOLE_H
