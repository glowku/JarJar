#ifndef JARJARVIS_GUI_H
#define JARJARVIS_GUI_H

#include <kernel/types.h>

#ifdef __cplusplus
extern "C" {
#endif

// Dimensions
#define GUI_DEFAULT_WIDTH 1920
#define GUI_DEFAULT_HEIGHT 1080
#define GUI_DEFAULT_BPP 32

// Couleurs JARVIS (thème Iron Man)
#define JARVIS_COLOR_PRIMARY 0xFF00D4FF      // Cyan brillant
#define JARVIS_COLOR_SECONDARY 0xFFFF6B00    // Orange
#define JARVIS_COLOR_ACCENT 0xFFFFD700       // Or
#define JARVIS_COLOR_BACKGROUND 0xFF0A0A0F   // Noir bleuté
#define JARVIS_COLOR_SURFACE 0xFF151520      // Gris foncé
#define JARVIS_COLOR_TEXT 0xFFFFFFFF         // Blanc
#define JARVIS_COLOR_TEXT_DIM 0xFFAAAAAA     // Gris clair
#define JARVIS_COLOR_SUCCESS 0xFF00FF88      // Vert
#define JARVIS_COLOR_WARNING 0xFFFFCC00      // Jaune
#define JARVIS_COLOR_ERROR 0xFFFF4444        // Rouge
#define JARVIS_COLOR_GRID 0xFF1A1A2E         // Grille

// Types de widgets
typedef enum {
    WIDGET_WINDOW = 0,
    WIDGET_BUTTON,
    WIDGET_LABEL,
    WIDGET_TEXTBOX,
    WIDGET_PANEL,
    WIDGET_CANVAS,
    WIDGET_LIST,
    WIDGET_TREE,
    WIDGET_GRAPH,
    WIDGET_HOLOGRAM,
    WIDGET_TERMINAL,
    WIDGET_NOTIFICATION,
    WIDGET_MENU,
    WIDGET_TOOLBAR,
    WIDGET_STATUSBAR,
    WIDGET_PROGRESS,
    WIDGET_SLIDER,
    WIDGET_CHECKBOX,
    WIDGET_RADIO,
    WIDGET_COMBOBOX,
    WIDGET_TAB,
    WIDGET_SCROLLBAR,
    WIDGET_TOOLTIP,
    WIDGET_CONTEXT_MENU,
    WIDGET_COUNT,
} widget_type_t;

// États des widgets
typedef enum {
    WIDGET_STATE_NORMAL = 0,
    WIDGET_STATE_HOVER,
    WIDGET_STATE_PRESSED,
    WIDGET_STATE_DISABLED,
    WIDGET_STATE_FOCUSED,
    WIDGET_STATE_SELECTED,
    WIDGET_STATE_ACTIVE,
    WIDGET_STATE_COUNT,
} widget_state_t;

// Alignement
typedef enum {
    ALIGN_LEFT = 0,
    ALIGN_CENTER,
    ALIGN_RIGHT,
    ALIGN_TOP,
    ALIGN_BOTTOM,
    ALIGN_JUSTIFY,
} alignment_t;

// Structure de couleur
typedef struct {
    u8 r, g, b, a;
} color_t;

// Structure de point
typedef struct {
    s32 x, y;
} point_t;

// Structure de rectangle
typedef struct {
    s32 x, y;
    s32 width, height;
} rect_t;

// Structure de taille
typedef struct {
    s32 width, height;
} gui_size_t;

// Structure de marge
typedef struct {
    s32 left, top, right, bottom;
} margin_t;

// Structure de police
typedef struct {
    char name[64];
    s32 size;
    bool bold;
    bool italic;
    u8* bitmap;
    s32 char_width;
    s32 char_height;
} font_t;

// Structure de buffer de frame
typedef struct {
    u32* pixels;
    s32 width;
    s32 height;
    s32 pitch;
    bool double_buffered;
    u32* back_buffer;
} framebuffer_t;

// Structure de widget de base
typedef struct widget {
    widget_type_t type;
    char id[64];
    char text[256];
    rect_t bounds;
    margin_t margin;
    margin_t padding;
    color_t background_color;
    color_t foreground_color;
    color_t border_color;
    s32 border_width;
    s32 border_radius;
    bool visible;
    bool enabled;
    widget_state_t state;
    
    font_t* font;
    alignment_t text_align;
    alignment_t vertical_align;
    
    struct widget* parent;
    struct widget* children;
    struct widget* next;
    struct widget* prev;
    
    void (*on_paint)(struct widget* w, framebuffer_t* fb);
    void (*on_click)(struct widget* w, s32 x, s32 y);
    void (*on_hover)(struct widget* w, s32 x, s32 y);
    void (*on_key)(struct widget* w, u32 key);
    void (*on_focus)(struct widget* w, bool focused);
    void* user_data;
} widget_t;

// Structure de fenêtre
typedef struct {
    widget_t base;
    char title[256];
    bool maximized;
    bool minimized;
    bool resizable;
    bool movable;
    s32 min_width;
    s32 min_height;
    widget_t* title_bar;
    widget_t* client_area;
    widget_t* close_button;
    widget_t* maximize_button;
    widget_t* minimize_button;
} window_t;

// Structure de bouton
typedef struct {
    widget_t base;
    bool toggle;
    bool checked;
    u32 icon;
    char tooltip[256];
} button_t;

// Structure de label
typedef struct {
    widget_t base;
    bool auto_size;
    bool word_wrap;
} label_t;

// Structure de zone de texte
typedef struct {
    widget_t base;
    char* text_buffer;
    s32 buffer_size;
    s32 text_length;
    s32 cursor_pos;
    s32 selection_start;
    s32 selection_end;
    bool multiline;
    bool password;
    bool readonly;
    s32 scroll_x;
    s32 scroll_y;
} textbox_t;

// Structure de panneau
typedef struct {
    widget_t base;
    bool scrollable;
    s32 scroll_x;
    s32 scroll_y;
    s32 content_width;
    s32 content_height;
    color_t gradient_start;
    color_t gradient_end;
    bool has_gradient;
} panel_t;

// Structure de canvas (dessin)
typedef struct {
    widget_t base;
    u32* pixels;
    bool transparent;
} canvas_t;

// Structure de graphique
typedef struct {
    widget_t base;
    f32* data;
    s32 data_count;
    f32 min_value;
    f32 max_value;
    color_t line_color;
    color_t fill_color;
    bool filled;
    bool show_grid;
    bool show_labels;
} graph_t;

// Structure d'hologramme 3D
typedef struct {
    widget_t base;
    f32 rotation_x;
    f32 rotation_y;
    f32 rotation_z;
    f32 scale;
    u32* texture;
    s32 texture_width;
    s32 texture_height;
    bool wireframe;
    bool animated;
    f32 animation_speed;
} hologram_t;

// Structure de terminal
typedef struct {
    widget_t base;
    char** lines;
    s32 num_lines;
    s32 max_lines;
    s32 current_line;
    s32 scroll_offset;
    color_t prompt_color;
    color_t output_color;
    color_t error_color;
    char prompt[32];
} terminal_t;

// Structure de notification
typedef struct notification {   // <--- On ajoute "notification" ici
    widget_t base;
    char message[512];
    u32 type;  // 0=info, 1=success, 2=warning, 3=error
    u64 duration;
    u64 start_time;
    bool dismissable;
    void (*on_dismiss)(struct notification* n); // Maintenant il sait ce qu'est struct notification
} notification_t;

// Structure de barre de progression
typedef struct {
    widget_t base;
    f32 value;  // 0.0 - 100.0
    bool indeterminate;
    bool show_percentage;
    color_t progress_color;
} progress_t;

// Structure de gestionnaire de fenêtres
typedef struct {
    window_t** windows;
    s32 num_windows;
    s32 capacity;
    window_t* active_window;
    window_t* focused_window;
    widget_t* hovered_widget;
    widget_t* captured_widget;
    
    s32 mouse_x;
    s32 mouse_y;
    bool mouse_down;
    u32 modifiers;
    
    font_t* default_font;
    font_t* monospace_font;
    
    framebuffer_t* framebuffer;
    
    bool running;
    bool needs_redraw;
    
    notification_t** notifications;
    s32 num_notifications;
} window_manager_t;

// Structure de thème
typedef struct {
    color_t background;
    color_t surface;
    color_t primary;
    color_t secondary;
    color_t accent;
    color_t text;
    color_t text_secondary;
    color_t success;
    color_t warning;
    color_t error;
    
    s32 border_radius;
    s32 border_width;
    s32 shadow_offset;
    s32 shadow_blur;
    color_t shadow_color;
    
    font_t* font_normal;
    font_t* font_bold;
    font_t* font_title;
    font_t* font_monospace;
} theme_t;

// Initialisation
void gui_init(void);
void gui_shutdown(void);

// Gestion du framebuffer
framebuffer_t* framebuffer_create(s32 width, s32 height, bool double_buffered);
void framebuffer_destroy(framebuffer_t* fb);
void framebuffer_clear(framebuffer_t* fb, color_t color);
void framebuffer_present(framebuffer_t* fb);
void framebuffer_draw_pixel(framebuffer_t* fb, s32 x, s32 y, color_t color);
void framebuffer_draw_line(framebuffer_t* fb, s32 x1, s32 y1, s32 x2, s32 y2, color_t color);
void framebuffer_draw_rect(framebuffer_t* fb, rect_t rect, color_t color);
void framebuffer_fill_rect(framebuffer_t* fb, rect_t rect, color_t color);
void framebuffer_draw_circle(framebuffer_t* fb, s32 x, s32 y, s32 radius, color_t color);
void framebuffer_fill_circle(framebuffer_t* fb, s32 x, s32 y, s32 radius, color_t color);
void framebuffer_draw_text(framebuffer_t* fb, s32 x, s32 y, const char* text, font_t* font, color_t color);
void framebuffer_blit(framebuffer_t* dst, s32 x, s32 y, u32* src, s32 width, s32 height);

// Gestion des widgets
widget_t* widget_create(widget_type_t type, const char* id, rect_t bounds);
void widget_destroy(widget_t* widget);
void widget_add_child(widget_t* parent, widget_t* child);
void widget_remove_child(widget_t* parent, widget_t* child);
void widget_set_text(widget_t* widget, const char* text);
void widget_set_visible(widget_t* widget, bool visible);
void widget_set_enabled(widget_t* widget, bool enabled);
void widget_set_state(widget_t* widget, widget_state_t state);
void widget_invalidate(widget_t* widget);
void widget_paint(widget_t* widget, framebuffer_t* fb);
widget_t* widget_find_by_id(widget_t* root, const char* id);
widget_t* widget_hit_test(widget_t* root, s32 x, s32 y);

// Création de widgets spécifiques
window_t* window_create(const char* title, rect_t bounds);
void window_destroy(window_t* window);
void window_show(window_t* window);
void window_hide(window_t* window);
void window_maximize(window_t* window);
void window_minimize(window_t* window);
void window_restore(window_t* window);
void window_close(window_t* window);

button_t* button_create(const char* id, rect_t bounds, const char* text);
label_t* label_create(const char* id, rect_t bounds, const char* text);
textbox_t* textbox_create(const char* id, rect_t bounds);
panel_t* panel_create(const char* id, rect_t bounds);
canvas_t* canvas_create(const char* id, rect_t bounds);
graph_t* graph_create(const char* id, rect_t bounds);
hologram_t* hologram_create(const char* id, rect_t bounds);
terminal_t* terminal_create(const char* id, rect_t bounds);
progress_t* progress_create(const char* id, rect_t bounds);

// Gestionnaire de fenêtres
window_manager_t* wm_create(void);
void wm_destroy(window_manager_t* wm);
void wm_add_window(window_manager_t* wm, window_t* window);
void wm_remove_window(window_manager_t* wm, window_t* window);
void wm_set_active_window(window_manager_t* wm, window_t* window);
window_t* wm_get_active_window(window_manager_t* wm);
void wm_handle_mouse_move(window_manager_t* wm, s32 x, s32 y);
void wm_handle_mouse_down(window_manager_t* wm, s32 x, s32 y, u32 button);
void wm_handle_mouse_up(window_manager_t* wm, s32 x, s32 y, u32 button);
void wm_handle_key(window_manager_t* wm, u32 key);
void wm_update(window_manager_t* wm);
void wm_render(window_manager_t* wm);
void wm_run(window_manager_t* wm);

// Thèmes
theme_t* theme_create_default(void);
theme_t* theme_create_jarvis(void);
void theme_apply(widget_t* widget, theme_t* theme);

// Notifications
notification_t* notification_create(const char* message, u32 type, u64 duration);
void notification_show(window_manager_t* wm, notification_t* notification);
void notification_dismiss(notification_t* notification);

// Dessin avancé
void draw_holographic_frame(framebuffer_t* fb, rect_t rect, color_t color);
void draw_hex_grid(framebuffer_t* fb, rect_t rect, color_t color, s32 spacing);
void draw_scanline(framebuffer_t* fb, s32 y, color_t color);
void draw_glow_effect(framebuffer_t* fb, s32 x, s32 y, s32 radius, color_t color);
void draw_tech_border(framebuffer_t* fb, rect_t rect, color_t color);
void draw_progress_ring(framebuffer_t* fb, s32 x, s32 y, s32 radius, f32 progress, color_t color);
void draw_waveform(framebuffer_t* fb, rect_t rect, f32* data, s32 count, color_t color);
void draw_particle_system(framebuffer_t* fb, void* particles, s32 count);

// Animation
void animate_widget(widget_t* widget, f32 duration, void (*callback)(widget_t* w, f32 t));
void animate_fade_in(widget_t* widget, f32 duration);
void animate_fade_out(widget_t* widget, f32 duration);
void animate_slide_in(widget_t* widget, f32 duration, s32 direction);
void animate_pulse(widget_t* widget, f32 duration);
void animate_hologram(hologram_t* hologram, f32 delta_time);

// Interface JARVIS spécifique
void jarvis_gui_init(void);
void jarvis_gui_create_main_interface(void);
void jarvis_gui_create_dashboard(void);
void jarvis_gui_create_terminal(void);
void jarvis_gui_create_system_monitor(void);
void jarvis_gui_create_ai_panel(void);
void jarvis_gui_create_file_manager(void);
void jarvis_gui_show_notification(const char* message, u32 type);
void jarvis_gui_update_system_stats(void);

// Événements
void gui_handle_event(void* event);
void gui_process_events(void);

#ifdef __cplusplus
}
#endif

#endif // JARJARVIS_GUI_H
