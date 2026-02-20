#include <gui/jarvis_gui.h>
#include <kernel/console.h>
#include <kernel/memory.h>
#include <kernel/panic.h>
#include <kernel/types.h>
#include <string.h>

struct notification; // Forward declaration
// --- FONCTIONS MATHÉMATIQUES INTERNES (Correction Linkage) ---

// Utilisation des builtins GCC pour éviter "undefined reference to sinf/cosf"
#undef abs
static inline s32 abs(s32 x) {
    return (x < 0) ? -x : x;
}

// On utilise les intrinsèques du compilateur pour le bare-metal
#define cosf(x) __builtin_cosf(x)
#define sinf(x) __builtin_sinf(x)

// --- VARIABLES GLOBALES ---

static window_manager_t* global_wm = NULL;
static theme_t* jarvis_theme = NULL;

static window_t* main_window = NULL;
static window_t* dashboard_window = NULL;
static window_t* terminal_window = NULL;

// --- DÉCLARATIONS EXTERNES (Si non définies dans les headers) ---
extern void draw_hex_grid(framebuffer_t* fb, rect_t rect, color_t color, s32 spacing);
extern void draw_tech_border(framebuffer_t* fb, rect_t rect, color_t color);

// --- STUBS POUR FONCTIONS MANQUANTES ---

terminal_t* terminal_create(const char* id, rect_t bounds) {
    terminal_t* term = kmalloc(sizeof(terminal_t));
    if (term) {
        memset(term, 0, sizeof(terminal_t));
        term->base.type = WIDGET_TERMINAL;
        term->base.bounds = bounds;
        strncpy(term->base.id, id, sizeof(term->base.id) - 1);
    }
    return term;
}

// --- INITIALISATION ---

void gui_init(void) {
    console_printf("[GUI] Initializing JARVIS Graphical Interface...\n");
    
    // Note: Dans un vrai OS, ces paramètres viendraient du tag Multiboot2 Framebuffer
    framebuffer_t* fb = framebuffer_create(1024, 768, true);
    if (!fb) {
        panic("Failed to create framebuffer");
    }
    
    global_wm = wm_create();
    if (!global_wm) panic("Failed to create Window Manager");
    
    global_wm->framebuffer = fb;
    jarvis_theme = theme_create_jarvis();
    
    console_printf("[GUI] Framebuffer: %dx%d @ 32bpp\n", fb->width, fb->height);
    jarvis_gui_init();
}

void gui_shutdown(void) {
    if (global_wm) wm_destroy(global_wm);
    if (jarvis_theme) kfree(jarvis_theme);
}

// --- FRAMEBUFFER OPS ---

framebuffer_t* framebuffer_create(s32 width, s32 height, bool double_buffered) {
    framebuffer_t* fb = kmalloc(sizeof(framebuffer_t));
    if (!fb) return NULL;
    
    fb->width = width;
    fb->height = height;
    fb->pitch = width * 4;
    fb->double_buffered = double_buffered;
    
    size_t size = width * height * 4;
    fb->pixels = kmalloc(size);
    if (!fb->pixels) {
        kfree(fb);
        return NULL;
    }
    memset(fb->pixels, 0, size);
    
    if (double_buffered) {
        fb->back_buffer = kmalloc(size);
        if (!fb->back_buffer) {
            kfree(fb->pixels);
            kfree(fb);
            return NULL;
        }
        memset(fb->back_buffer, 0, size);
    } else {
        fb->back_buffer = NULL;
    }
    return fb;
}

void framebuffer_present(framebuffer_t* fb) {
    if (!fb || !fb->double_buffered || !fb->back_buffer) return;
    // Copie du back_buffer vers le front_buffer (VRAM réelle)
    memcpy(fb->pixels, fb->back_buffer, fb->width * fb->height * 4);
}

void framebuffer_draw_pixel(framebuffer_t* fb, s32 x, s32 y, color_t color) {
    if (!fb || x < 0 || x >= fb->width || y < 0 || y >= fb->height) return;
    u32* buffer = fb->double_buffered ? fb->back_buffer : fb->pixels;
    u32 pixel = (color.a << 24) | (color.r << 16) | (color.g << 8) | color.b;
    buffer[y * fb->width + x] = pixel;
}

// --- DESSIN DES FORMES ---

void framebuffer_clear(framebuffer_t* fb, color_t color) {
    if (!fb) return;
    u32 pixel = (color.a << 24) | (color.r << 16) | (color.g << 8) | color.b;
    u32* buffer = fb->double_buffered ? fb->back_buffer : fb->pixels;
    u32 count = fb->width * fb->height;
    for (u32 i = 0; i < count; i++) buffer[i] = pixel;
}

void framebuffer_draw_line(framebuffer_t* fb, s32 x1, s32 y1, s32 x2, s32 y2, color_t color) {
    s32 dx = abs(x2 - x1), sx = x1 < x2 ? 1 : -1;
    s32 dy = -abs(y2 - y1), sy = y1 < y2 ? 1 : -1;
    s32 err = dx + dy, e2;

    while (1) {
        framebuffer_draw_pixel(fb, x1, y1, color);
        if (x1 == x2 && y1 == y2) break;
        e2 = 2 * err;
        if (e2 >= dy) { err += dy; x1 += sx; }
        if (e2 <= dx) { err += dx; y1 += sy; }
    }
}

// --- WINDOW MANAGER & WIDGETS ---

window_t* window_create(const char* title, rect_t bounds) {
    window_t* win = kmalloc(sizeof(window_t));
    if (!win) return NULL;
    memset(win, 0, sizeof(window_t));
    
    win->base.type = WIDGET_WINDOW;
    win->base.bounds = bounds;
    win->base.visible = true;
    strncpy(win->title, title, sizeof(win->title) - 1);
    
    return win;
}

void wm_render(window_manager_t* wm) {
    if (!wm || !wm->framebuffer) return;
    
    // Fond d'écran style JARVIS
    framebuffer_clear(wm->framebuffer, (color_t){5, 5, 10, 255});
    draw_hex_grid(wm->framebuffer, (rect_t){0, 0, wm->framebuffer->width, wm->framebuffer->height}, (color_t){0, 40, 80, 50}, 45);
    
    for (s32 i = 0; i < wm->num_windows; i++) {
        if (wm->windows[i] && wm->windows[i]->base.visible) {
            draw_tech_border(wm->framebuffer, wm->windows[i]->base.bounds, (color_t){0, 180, 255, 255});
        }
    }
    
    framebuffer_present(wm->framebuffer);
}

// --- JARVIS SPECIFIC ---

void draw_progress_ring(framebuffer_t* fb, s32 x, s32 y, s32 radius, f32 progress, color_t color) {
    s32 segments = (s32)(360 * progress);
    for (s32 i = 0; i < segments; i++) {
        f32 angle = (f32)i * 3.14159f / 180.0f;
        s32 px = x + (s32)(cosf(angle) * radius);
        s32 py = y + (s32)(sinf(angle) * radius);
        framebuffer_draw_pixel(fb, px, py, color);
    }
}

void jarvis_gui_init(void) {
    main_window = window_create("JARVIS CORE v1.0", (rect_t){50, 50, 800, 600});
    wm_add_window(global_wm, main_window);
    
    dashboard_window = window_create("SENSOR DATA", (rect_t){870, 50, 400, 300});
    wm_add_window(global_wm, dashboard_window);
}

// Stubs restants pour éviter les erreurs de linkage
window_manager_t* wm_create(void) {
    window_manager_t* wm = kmalloc(sizeof(window_manager_t));
    if (wm) {
        memset(wm, 0, sizeof(window_manager_t));
        wm->capacity = 32;
        wm->windows = kmalloc(sizeof(window_t*) * wm->capacity);
    }
    return wm;
}

void wm_add_window(window_manager_t* wm, window_t* window) {
    if (wm && window && wm->num_windows < wm->capacity) {
        wm->windows[wm->num_windows++] = window;
    }
}

theme_t* theme_create_jarvis(void) {
    theme_t* t = kmalloc(sizeof(theme_t));
    if (t) memset(t, 0, sizeof(theme_t));
    return t;
}

void wm_destroy(window_manager_t* wm) { if(wm) kfree(wm); }
void jarvis_gui_create_dashboard(void) {}
void jarvis_gui_create_terminal(void) {}