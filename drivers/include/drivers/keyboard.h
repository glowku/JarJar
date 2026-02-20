#ifndef JARJARVIS_KEYBOARD_H
#define JARJARVIS_KEYBOARD_H

#include <kernel/types.h>
#include <kernel/interrupt.h>

#ifdef __cplusplus
extern "C" {
#endif

// Ports PS/2
define KEYBOARD_DATA_PORT 0x60
define KEYBOARD_STATUS_PORT 0x64
define KEYBOARD_COMMAND_PORT 0x64

// Commandes
define KEYBOARD_CMD_READ_CONFIG 0x20
define KEYBOARD_CMD_WRITE_CONFIG 0x60
define KEYBOARD_CMD_DISABLE_PS2 0xAD
define KEYBOARD_CMD_ENABLE_PS2 0xAE
define KEYBOARD_CMD_SELF_TEST 0xAA
define KEYBOARD_CMD_TEST_PS2 0xAB
define KEYBOARD_CMD_DISABLE_SCANNING 0xF5
define KEYBOARD_CMD_ENABLE_SCANNING 0xF4
define KEYBOARD_CMD_SET_DEFAULTS 0xF6
define KEYBOARD_CMD_SET_SCANCODE 0xF0
define KEYBOARD_CMD_SET_TYPEMATIC 0xF3
define KEYBOARD_CMD_RESET 0xFF
define KEYBOARD_CMD_RESEND 0xFE
define KEYBOARD_CMD_ACK 0xFA
define KEYBOARD_CMD_ECHO 0xEE

// Réponses
define KEYBOARD_RESP_ACK 0xFA
define KEYBOARD_RESP_RESEND 0xFE
define KEYBOARD_RESP_ERROR 0xFC
define KEYBOARD_RESP_SELF_TEST_OK 0x55
define KEYBOARD_RESP_ECHO 0xEE

// Scancodes
define SCANCODE_ESC 0x01
define SCANCODE_1 0x02
define SCANCODE_2 0x03
define SCANCODE_3 0x04
define SCANCODE_4 0x05
define SCANCODE_5 0x06
define SCANCODE_6 0x07
define SCANCODE_7 0x08
define SCANCODE_8 0x09
define SCANCODE_9 0x0A
define SCANCODE_0 0x0B
define SCANCODE_MINUS 0x0C
define SCANCODE_EQUAL 0x0D
define SCANCODE_BACKSPACE 0x0E
define SCANCODE_TAB 0x0F
define SCANCODE_Q 0x10
define SCANCODE_W 0x11
define SCANCODE_E 0x12
define SCANCODE_R 0x13
define SCANCODE_T 0x14
define SCANCODE_Y 0x15
define SCANCODE_U 0x16
define SCANCODE_I 0x17
define SCANCODE_O 0x18
define SCANCODE_P 0x19
define SCANCODE_LBRACKET 0x1A
define SCANCODE_RBRACKET 0x1B
define SCANCODE_ENTER 0x1C
define SCANCODE_LCTRL 0x1D
define SCANCODE_A 0x1E
define SCANCODE_S 0x1F
define SCANCODE_D 0x20
define SCANCODE_F 0x21
define SCANCODE_G 0x22
define SCANCODE_H 0x23
define SCANCODE_J 0x24
define SCANCODE_K 0x25
define SCANCODE_L 0x26
define SCANCODE_SEMICOLON 0x27
define SCANCODE_QUOTE 0x28
define SCANCODE_BACKTICK 0x29
define SCANCODE_LSHIFT 0x2A
define SCANCODE_BACKSLASH 0x2B
define SCANCODE_Z 0x2C
define SCANCODE_X 0x2D
define SCANCODE_C 0x2E
define SCANCODE_V 0x2F
define SCANCODE_B 0x30
define SCANCODE_N 0x31
define SCANCODE_M 0x32
define SCANCODE_COMMA 0x33
define SCANCODE_PERIOD 0x34
define SCANCODE_SLASH 0x35
define SCANCODE_RSHIFT 0x36
define SCANCODE_KEYPAD_STAR 0x37
define SCANCODE_LALT 0x38
define SCANCODE_SPACE 0x39
define SCANCODE_CAPSLOCK 0x3A
define SCANCODE_F1 0x3B
define SCANCODE_F2 0x3C
define SCANCODE_F3 0x3D
define SCANCODE_F4 0x3E
define SCANCODE_F5 0x3F
define SCANCODE_F6 0x40
define SCANCODE_F7 0x41
define SCANCODE_F8 0x42
define SCANCODE_F9 0x43
define SCANCODE_F10 0x44
define SCANCODE_NUMLOCK 0x45
define SCANCODE_SCROLLLOCK 0x46
define SCANCODE_KEYPAD_7 0x47
define SCANCODE_KEYPAD_8 0x48
define SCANCODE_KEYPAD_9 0x49
define SCANCODE_KEYPAD_MINUS 0x4A
define SCANCODE_KEYPAD_4 0x4B
define SCANCODE_KEYPAD_5 0x4C
define SCANCODE_KEYPAD_6 0x4D
define SCANCODE_KEYPAD_PLUS 0x4E
define SCANCODE_KEYPAD_1 0x4F
define SCANCODE_KEYPAD_2 0x50
define SCANCODE_KEYPAD_3 0x51
define SCANCODE_KEYPAD_0 0x52
define SCANCODE_KEYPAD_PERIOD 0x53
define SCANCODE_F11 0x57
define SCANCODE_F12 0x58

// Touches spéciales (scancode set 1, avec E0 préfixe)
define SCANCODE_INSERT 0xE052
define SCANCODE_DELETE 0xE053
define SCANCODE_HOME 0xE047
define SCANCODE_END 0xE04F
define SCANCODE_PAGEUP 0xE049
define SCANCODE_PAGEDOWN 0xE051
define SCANCODE_LEFT 0xE04B
define SCANCODE_RIGHT 0xE04D
define SCANCODE_UP 0xE048
define SCANCODE_DOWN 0xE050
define SCANCODE_RCTRL 0xE01D
define SCANCODE_RALT 0xE038
define SCANCODE_LGUI 0xE05B
define SCANCODE_RGUI 0xE05C
define SCANCODE_APPS 0xE05D
define SCANCODE_KEYPAD_ENTER 0xE01C
define SCANCODE_KEYPAD_SLASH 0xE035

// Modifiers
define MODIFIER_LSHIFT (1 << 0)
define MODIFIER_RSHIFT (1 << 1)
define MODIFIER_LCTRL (1 << 2)
define MODIFIER_RCTRL (1 << 3)
define MODIFIER_LALT (1 << 4)
define MODIFIER_RALT (1 << 5)
define MODIFIER_LGUI (1 << 6)
define MODIFIER_RGUI (1 << 7)
define MODIFIER_CAPSLOCK (1 << 8)
define MODIFIER_NUMLOCK (1 << 9)
define MODIFIER_SCROLLLOCK (1 << 10)

// Type de touche
typedef enum {
    KEY_TYPE_NORMAL = 0,
    KEY_TYPE_MODIFIER,
    KEY_TYPE_FUNCTION,
    KEY_TYPE_KEYPAD,
    KEY_TYPE_SPECIAL,
} key_type_t;

// Événement clavier
typedef struct {
    u16 scancode;
    char ascii;
    bool pressed;
    bool repeat;
    u32 modifiers;
    key_type_t type;
} key_event_t;

// Callback pour les événements clavier
typedef void (*keyboard_callback_t)(key_event_t* event);

// Initialisation
void keyboard_init(void);
void keyboard_shutdown(void);

// Configuration
void keyboard_set_layout(const char* layout);
void keyboard_set_scancode_set(u8 set);
void keyboard_set_typematic(u8 delay, u8 rate);

// État
bool keyboard_is_key_pressed(u16 scancode);
u32 keyboard_get_modifiers(void);
bool keyboard_get_leds(void);
void keyboard_set_leds(bool numlock, bool capslock, bool scrolllock);

// Buffer
bool keyboard_has_input(void);
key_event_t keyboard_read(void);
char keyboard_getchar(void);
void keyboard_getline(char* buffer, size_t size);

// Callbacks
void keyboard_register_callback(keyboard_callback_t callback);
void keyboard_unregister_callback(keyboard_callback_t callback);

// Handler d'interruption
void keyboard_handler(interrupt_frame_t* frame);

// Tables de conversion
extern const char scancode_to_ascii_us[];
extern const char scancode_to_ascii_us_shifted[];

#ifdef __cplusplus
}
#endif

#endif // JARJARVIS_KEYBOARD_H
