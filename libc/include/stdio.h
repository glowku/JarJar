#ifndef JARJARVIS_STDIO_H
#define JARJARVIS_STDIO_H

#include <kernel/types.h>
#include <stdarg.h>

#ifdef __cplusplus
extern "C" {
#endif

// --- 1. DÉFINITION DES TYPES DE BASE ---
// On utilise u64 pour fpos_t (position dans le fichier)
typedef u64 fpos_t;

// On s'assure que size_t est défini (souvent u64 sur archi 64-bits)
// Si size_t cause encore une erreur, vérifie kernel/types.h
#ifndef _SIZE_T_DEFINED
#define _SIZE_T_DEFINED
typedef u64 size_t;
#endif

// --- 2. STRUCTURE FILE ---
typedef struct {
    int fd;
    int flags;
    int eof;
    int error;
    char* buffer;
    size_t buffer_size;
    size_t buffer_pos;
} FILE;

// Standard streams
extern FILE* stdin;
extern FILE* stdout;
extern FILE* stderr;

// --- 3. OPÉRATIONS SUR LES FICHIERS ---
FILE* fopen(const char* path, const char* mode);
int fclose(FILE* stream);
size_t fread(void* ptr, size_t size, size_t nmemb, FILE* stream);
size_t fwrite(const void* ptr, size_t size, size_t nmemb, FILE* stream);
int fseek(FILE* stream, long offset, int whence);
long ftell(FILE* stream);
void rewind(FILE* stream);
int fflush(FILE* stream);
int feof(FILE* stream);
int ferror(FILE* stream);
void clearerr(FILE* stream);

// Formatted I/O
int fprintf(FILE* stream, const char* format, ...);
int printf(const char* format, ...);
int sprintf(char* str, const char* format, ...);
int snprintf(char* str, size_t size, const char* format, ...);
int vfprintf(FILE* stream, const char* format, va_list ap);
int vprintf(const char* format, va_list ap);
int vsprintf(char* str, const char* format, va_list ap);
int vsnprintf(char* str, size_t size, const char* format, va_list ap);

// Character I/O
int fgetc(FILE* stream);
int getc(FILE* stream);
int getchar(void);
char* fgets(char* s, int size, FILE* stream);
int fputc(int c, FILE* stream);
int putc(int c, FILE* stream);
int putchar(int c);
int fputs(const char* s, FILE* stream);
int puts(const char* s);
int ungetc(int c, FILE* stream);

// File positioning
// Maintenant fpos_t est connu !
int fgetpos(FILE* stream, fpos_t* pos);
int fsetpos(FILE* stream, const fpos_t* pos);

// File operations
int remove(const char* pathname);
int rename(const char* oldpath, const char* newpath);
FILE* tmpfile(void);
char* tmpnam(char* s);

// Error handling
void perror(const char* s);

// Buffering
void setbuf(FILE* stream, char* buf);
int setvbuf(FILE* stream, char* buf, int mode, size_t size);

// --- 4. CONSTANTES ---
#ifndef EOF
#define EOF (-1)
#endif

#define BUFSIZ 8192

#define SEEK_SET 0
#define SEEK_CUR 1
#define SEEK_END 2

#define _IOFBF 0
#define _IOLBF 1
#define _IONBF 2

// Internal use
int kernel_printf(const char* format, ...);
int kernel_vprintf(const char* format, va_list args);

#ifdef __cplusplus
}
#endif

#endif // JARJARVIS_STDIO_H