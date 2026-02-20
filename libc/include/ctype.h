#ifndef JARJARVIS_CTYPE_H
#define JARJARVIS_CTYPE_H

#ifdef __cplusplus
extern "C" {
#endif

// Character class flags
#define _U  0x01  // Uppercase
#define _L  0x02  // Lowercase
#define _D  0x04  // Digit
#define _S  0x08  // Space
#define _P  0x10  // Punctuation
#define _C  0x20  // Control
#define _X  0x40  // Hex digit
#define _B  0x80  // Blank

// Character classification functions
int isalnum(int c);
int isalpha(int c);
int isascii(int c);
int isblank(int c);
int iscntrl(int c);
int isdigit(int c);
int isgraph(int c);
int islower(int c);
int isprint(int c);
int ispunct(int c);
int isspace(int c);
int isupper(int c);
int isxdigit(int c);

// Character conversion functions
int tolower(int c);
int toupper(int c);

// POSIX extensions
int isascii(int c);
int toascii(int c);

#ifdef __cplusplus
}
#endif

#endif // JARJARVIS_CTYPE_H
