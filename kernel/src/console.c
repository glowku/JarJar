#include <kernel/console.h>
#include <kernel/memory.h>

// Adresse du buffer VGA
#define VGA_BUFFER 0xB8000
#define VGA_WIDTH 80
#define VGA_HEIGHT 25

// Couleur par défaut
#define DEFAULT_COLOR ((VGA_LIGHT_GREY << 4) | VGA_BLACK)
// Prototypes pour éviter les déclarations implicites
static void itoa(int value, char* str, int base);
static void uitoa(unsigned int value, char* str, int base);
static void ltoa(long value, char* str, int base);
static void ultoa(unsigned long value, char* str, int base);
static inline void outb(u16 port, u8 value);
static inline u8 inb(u16 port);
// Console principale
static console_t main_console;
static console_t* active_console = &main_console;

// Table de conversion couleur VGA
static inline u8 vga_entry_color(vga_color_t fg, vga_color_t bg) {
    return fg | (bg << 4);
}

static inline u16 vga_entry(unsigned char c, u8 color) {
    return (u16)c | ((u16)color << 8);
}

void console_init(void) {
    main_console.buffer = (u16*)PHYS_TO_VIRT(VGA_BUFFER);
    main_console.width = VGA_WIDTH;
    main_console.height = VGA_HEIGHT;
    main_console.cursor_x = 0;
    main_console.cursor_y = 0;
    main_console.color = vga_entry_color(VGA_LIGHT_GREY, VGA_BLACK);
    main_console.cursor_enabled = true;
    main_console.scroll_enabled = true;
    
    console_clear();
    console_enable_cursor();
}

void console_clear(void) {
    console_t* console = active_console;
    
    for (u32 y = 0; y < console->height; y++) {
        for (u32 x = 0; x < console->width; x++) {
            const u32 index = y * console->width + x;
            console->buffer[index] = vga_entry(' ', console->color);
        }
    }
    
    console->cursor_x = 0;
    console->cursor_y = 0;
    console_set_cursor_pos(0, 0);
}

void console_putchar(char c) {
    console_t* console = active_console;
    
    if (c == '\n') {
        console->cursor_x = 0;
        console->cursor_y++;
    } else if (c == '\r') {
        console->cursor_x = 0;
    } else if (c == '\t') {
        console->cursor_x = (console->cursor_x + 8) & ~7;
    } else if (c == '\b') {
        if (console->cursor_x > 0) {
            console->cursor_x--;
            const u32 index = console->cursor_y * console->width + console->cursor_x;
            console->buffer[index] = vga_entry(' ', console->color);
        }
    } else if (c >= ' ') {
        const u32 index = console->cursor_y * console->width + console->cursor_x;
        console->buffer[index] = vga_entry(c, console->color);
        console->cursor_x++;
    }
    
    // Gestion du dépassement horizontal
    if (console->cursor_x >= console->width) {
        console->cursor_x = 0;
        console->cursor_y++;
    }
    
    // Gestion du dépassement vertical
    if (console->cursor_y >= console->height) {
        if (console->scroll_enabled) {
            console_scroll_up();
            console->cursor_y = console->height - 1;
        } else {
            console->cursor_y = 0;
        }
    }
    
    // Mise à jour du curseur
    if (console->cursor_enabled) {
        console_set_cursor_pos(console->cursor_x, console->cursor_y);
    }
}

void console_puts(const char* str) {
    while (*str) {
        console_putchar(*str++);
    }
}

// Fonction printf simplifiée
void console_vprintf(const char* fmt, va_list args) {
    char buffer[32];
    
    while (*fmt) {
        if (*fmt == '%' && *(fmt + 1)) {
            fmt++;
            switch (*fmt) {
                case 'c': {
                    char c = (char)va_arg(args, int);
                    console_putchar(c);
                    break;
                }
                case 's': {
                    const char* s = va_arg(args, const char*);
                    console_puts(s ? s : "(null)");
                    break;
                }
                case 'd':
                case 'i': {
                    int i = va_arg(args, int);
                    if (i < 0) {
                        console_putchar('-');
                        i = -i;
                    }
                    itoa(i, buffer, 10);
                    console_puts(buffer);
                    break;
                }
                case 'u': {
                    unsigned int u = va_arg(args, unsigned int);
                    uitoa(u, buffer, 10);
                    console_puts(buffer);
                    break;
                }
                case 'x': {
                    unsigned int x = va_arg(args, unsigned int);
                    uitoa(x, buffer, 16);
                    console_puts(buffer);
                    break;
                }
                case 'X': {
                    unsigned int X = va_arg(args, unsigned int);
                    uitoa(X, buffer, 16);
                    // Convertir en majuscules
                    for (char* p = buffer; *p; p++) {
                        if (*p >= 'a' && *p <= 'f') {
                            *p -= 32;
                        }
                    }
                    console_puts(buffer);
                    break;
                }
                case 'p': {
                    void* p = va_arg(args, void*);
                    console_puts("0x");
                    uitoa((u64)p, buffer, 16);
                    console_puts(buffer);
                    break;
                }
                case 'l': {
                    fmt++;
                    if (*fmt == 'u') {
                        unsigned long ul = va_arg(args, unsigned long);
                        ultoa(ul, buffer, 10);
                        console_puts(buffer);
                    } else if (*fmt == 'd') {
                        long l = va_arg(args, long);
                        if (l < 0) {
                            console_putchar('-');
                            l = -l;
                        }
                        ltoa(l, buffer, 10);
                        console_puts(buffer);
                    } else if (*fmt == 'x') {
                        unsigned long ul = va_arg(args, unsigned long);
                        ultoa(ul, buffer, 16);
                        console_puts(buffer);
                    }
                    break;
                }
                case '%': {
                    console_putchar('%');
                    break;
                }
                default: {
                    console_putchar('%');
                    console_putchar(*fmt);
                    break;
                }
            }
        } else {
            console_putchar(*fmt);
        }
        fmt++;
    }
}

void console_printf(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    console_vprintf(fmt, args);
    va_end(args);
}

// Couleurs
void console_set_color(vga_color_t fg, vga_color_t bg) {
    active_console->color = vga_entry_color(fg, bg);
}

void console_set_foreground(vga_color_t color) {
    active_console->color = (active_console->color & 0xF0) | color;
}

void console_set_background(vga_color_t color) {
    active_console->color = (active_console->color & 0x0F) | (color << 4);
}

void console_reset_color(void) {
    active_console->color = vga_entry_color(VGA_LIGHT_GREY, VGA_BLACK);
}

// Curseur
void console_enable_cursor(void) {
    active_console->cursor_enabled = true;
    
    // Configuration du curseur VGA
    outb(0x3D4, 0x0A);
    outb(0x3D5, (inb(0x3D5) & 0xC0) | 0);
    
    outb(0x3D4, 0x0B);
    outb(0x3D5, (inb(0x3D5) & 0xE0) | 15);
}

void console_disable_cursor(void) {
    active_console->cursor_enabled = false;
    
    outb(0x3D4, 0x0A);
    outb(0x3D5, 0x20);
}

void console_set_cursor_pos(u32 x, u32 y) {
    if (x >= active_console->width) x = active_console->width - 1;
    if (y >= active_console->height) y = active_console->height - 1;
    
    active_console->cursor_x = x;
    active_console->cursor_y = y;
    
    u16 pos = y * active_console->width + x;
    
    outb(0x3D4, 0x0F);
    outb(0x3D5, (u8)(pos & 0xFF));
    outb(0x3D4, 0x0E);
    outb(0x3D5, (u8)((pos >> 8) & 0xFF));
}

void console_get_cursor_pos(u32* x, u32* y) {
    if (x) *x = active_console->cursor_x;
    if (y) *y = active_console->cursor_y;
}

// Défilement
void console_scroll_up(void) {
    console_t* console = active_console;
    
    // Déplacer tout vers le haut
    for (u32 y = 0; y < console->height - 1; y++) {
        for (u32 x = 0; x < console->width; x++) {
            const u32 dst = y * console->width + x;
            const u32 src = (y + 1) * console->width + x;
            console->buffer[dst] = console->buffer[src];
        }
    }
    
    // Effacer la dernière ligne
    for (u32 x = 0; x < console->width; x++) {
        const u32 index = (console->height - 1) * console->width + x;
        console->buffer[index] = vga_entry(' ', console->color);
    }
}

void console_scroll_down(void) {
    console_t* console = active_console;
    
    // Déplacer tout vers le bas
    for (u32 y = console->height - 1; y > 0; y--) {
        for (u32 x = 0; x < console->width; x++) {
            const u32 dst = y * console->width + x;
            const u32 src = (y - 1) * console->width + x;
            console->buffer[dst] = console->buffer[src];
        }
    }
    
    // Effacer la première ligne
    for (u32 x = 0; x < console->width; x++) {
        console->buffer[x] = vga_entry(' ', console->color);
    }
}

// Couleurs spéciales pour l'IA
void console_ai_output(void) {
    console_set_color(VGA_LIGHT_GREEN, VGA_BLACK);
}

void console_ai_thinking(void) {
    console_set_color(VGA_LIGHT_BLUE, VGA_BLACK);
}

void console_ai_warning(void) {
    console_set_color(VGA_LIGHT_BROWN, VGA_BLACK);
}

void console_ai_error(void) {
    console_set_color(VGA_LIGHT_RED, VGA_BLACK);
}

// Gestion des consoles
console_t* console_get_active(void) {
    return active_console;
}

void console_switch(console_t* console) {
    if (console) {
        active_console = console;
    }
}

// Fonctions utilitaires d'IO
static inline void outb(u16 port, u8 value) {
    __asm__ volatile ("outb %0, %1" : : "a"(value), "Nd"(port));
}

static inline u8 inb(u16 port) {
    u8 value;
    __asm__ volatile ("inb %1, %0" : "=a"(value) : "Nd"(port));
    return value;
}

// Fonctions de conversion
static void itoa(int value, char* str, int base) {
    char* ptr = str;
    char* ptr1 = str;
    char tmp_char;
    int tmp_value;
    
    do {
        tmp_value = value;
        value /= base;
        *ptr++ = "0123456789abcdef"[tmp_value - value * base];
    } while (value);
    
    *ptr-- = '\0';
    
    while (ptr1 < ptr) {
        tmp_char = *ptr;
        *ptr-- = *ptr1;
        *ptr1++ = tmp_char;
    }
}

static void uitoa(unsigned int value, char* str, int base) {
    char* ptr = str;
    char* ptr1 = str;
    char tmp_char;
    unsigned int tmp_value;
    
    do {
        tmp_value = value;
        value /= base;
        *ptr++ = "0123456789abcdef"[tmp_value - value * base];
    } while (value);
    
    *ptr-- = '\0';
    
    while (ptr1 < ptr) {
        tmp_char = *ptr;
        *ptr-- = *ptr1;
        *ptr1++ = tmp_char;
    }
}

static void ltoa(long value, char* str, int base) {
    char* ptr = str;
    char* ptr1 = str;
    char tmp_char;
    long tmp_value;
    
    do {
        tmp_value = value;
        value /= base;
        *ptr++ = "0123456789abcdef"[tmp_value - value * base];
    } while (value);
    
    *ptr-- = '\0';
    
    while (ptr1 < ptr) {
        tmp_char = *ptr;
        *ptr-- = *ptr1;
        *ptr1++ = tmp_char;
    }
}

static void ultoa(unsigned long value, char* str, int base) {
    char* ptr = str;
    char* ptr1 = str;
    char tmp_char;
    unsigned long tmp_value;
    
    do {
        tmp_value = value;
        value /= base;
        *ptr++ = "0123456789abcdef"[tmp_value - value * base];
    } while (value);
    
    *ptr-- = '\0';
    
    while (ptr1 < ptr) {
        tmp_char = *ptr;
        *ptr-- = *ptr1;
        *ptr1++ = tmp_char;
    }
}
