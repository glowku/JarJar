#include <ctype.h>

// Character classification tables
static const unsigned char ctype_table[256] = {
    // 0-31: Control characters
    _C, _C, _C, _C, _C, _C, _C, _C, _C, _C|_S, _C|_S, _C|_S, _C|_S, _C|_S, _C, _C,
    _C, _C, _C, _C, _C, _C, _C, _C, _C, _C, _C, _C, _C, _C, _C, _C,
    // 32-47: Space and punctuation
    _S, _P, _P, _P, _P, _P, _P, _P, _P, _P, _P, _P, _P, _P, _P, _P,
    // 48-57: Digits
    _D|_X, _D|_X, _D|_X, _D|_X, _D|_X, _D|_X, _D|_X, _D|_X, _D|_X, _D|_X,
    // 58-64: Punctuation
    _P, _P, _P, _P, _P, _P, _P,
    // 65-70: Uppercase A-F (hex)
    _U|_X, _U|_X, _U|_X, _U|_X, _U|_X, _U|_X,
    // 71-90: Uppercase G-Z
    _U, _U, _U, _U, _U, _U, _U, _U, _U, _U, _U, _U, _U, _U, _U, _U, _U, _U, _U, _U,
    // 91-96: Punctuation
    _P, _P, _P, _P, _P, _P,
    // 97-102: Lowercase a-f (hex)
    _L|_X, _L|_X, _L|_X, _L|_X, _L|_X, _L|_X,
    // 103-122: Lowercase g-z
    _L, _L, _L, _L, _L, _L, _L, _L, _L, _L, _L, _L, _L, _L, _L, _L, _L, _L, _L, _L,
    // 123-127: Punctuation
    _P, _P, _P, _P, _P,
    // 128-255: Extended ASCII (all zero)
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
};

int isalnum(int c) {
    return (ctype_table[(unsigned char)c] & (_U|_L|_D)) != 0;
}

int isalpha(int c) {
    return (ctype_table[(unsigned char)c] & (_U|_L)) != 0;
}

int isascii(int c) {
    return (unsigned)c < 128;
}

int isblank(int c) {
    return c == ' ' || c == '\t';
}

int iscntrl(int c) {
    return (ctype_table[(unsigned char)c] & _C) != 0;
}

int isdigit(int c) {
    return (ctype_table[(unsigned char)c] & _D) != 0;
}

int isgraph(int c) {
    return (unsigned)c > 32 && (unsigned)c < 127;
}

int islower(int c) {
    return (ctype_table[(unsigned char)c] & _L) != 0;
}

int isprint(int c) {
    return (unsigned)c >= 32 && (unsigned)c < 127;
}

int ispunct(int c) {
    return (ctype_table[(unsigned char)c] & _P) != 0;
}

int isspace(int c) {
    return (ctype_table[(unsigned char)c] & _S) != 0;
}

int isupper(int c) {
    return (ctype_table[(unsigned char)c] & _U) != 0;
}

int isxdigit(int c) {
    return (ctype_table[(unsigned char)c] & _X) != 0;
}

int tolower(int c) {
    if (isupper(c)) {
        return c + ('a' - 'A');
    }
    return c;
}

int toupper(int c) {
    if (islower(c)) {
        return c - ('a' - 'A');
    }
    return c;
}
