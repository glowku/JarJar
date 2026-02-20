#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <kernel/console.h>

// Standard streams (defined in kernel)
extern void console_printf(const char* fmt, ...);
extern void console_vprintf(const char* fmt, va_list args);

// Simplified FILE implementation for kernel
static FILE stdin_file = {0};
static FILE stdout_file = {1};
static FILE stderr_file = {2};

FILE* stdin = &stdin_file;
FILE* stdout = &stdout_file;
FILE* stderr = &stderr_file;

// Formatted output
int printf(const char* format, ...) {
    va_list args;
    va_start(args, format);
    int ret = vprintf(format, args);
    va_end(args);
    return ret;
}

int vprintf(const char* format, va_list ap) {
    console_vprintf(format, ap);
    return 0;  // Return number of chars printed in real implementation
}

int fprintf(FILE* stream, const char* format, ...) {
    (void)stream;
    va_list args;
    va_start(args, format);
    int ret = vprintf(format, args);
    va_end(args);
    return ret;
}

int vfprintf(FILE* stream, const char* format, va_list ap) {
    (void)stream;
    console_vprintf(format, ap);
    return 0;
}

int sprintf(char* str, const char* format, ...) {
    va_list args;
    va_start(args, format);
    int ret = vsprintf(str, format, args);
    va_end(args);
    return ret;
}

int snprintf(char* str, size_t size, const char* format, ...) {
    va_list args;
    va_start(args, format);
    int ret = vsnprintf(str, size, format, args);
    va_end(args);
    return ret;
}

int vsprintf(char* str, const char* format, va_list ap) {
    return vsnprintf(str, (size_t)-1, format, ap);
}

int vsnprintf(char* str, size_t size, const char* format, va_list ap) {
    // Simple implementation - just format into buffer
    char* out = str;
    size_t count = 0;
    
    while (*format && count < size - 1) {
        if (*format == '%' && *(format + 1)) {
            format++;
            switch (*format) {
                case 'd':
                case 'i': {
                    int val = va_arg(ap, int);
                    char buf[32];
                    itoa(val, buf, 10);
                    for (char* p = buf; *p && count < size - 1; p++) {
                        *out++ = *p;
                        count++;
                    }
                    break;
                }
                case 'u': {
                    unsigned int val = va_arg(ap, unsigned int);
                    char buf[32];
                    utoa(val, buf, 10);
                    for (char* p = buf; *p && count < size - 1; p++) {
                        *out++ = *p;
                        count++;
                    }
                    break;
                }
                case 'x':
                case 'X': {
                    unsigned int val = va_arg(ap, unsigned int);
                    char buf[32];
                    utoa(val, buf, 16);
                    for (char* p = buf; *p && count < size - 1; p++) {
                        *out++ = *p;
                        count++;
                    }
                    break;
                }
                case 'p': {
                    void* val = va_arg(ap, void*);
                    char buf[32];
                    strcpy(buf, "0x");
                    ultoa((unsigned long)val, buf + 2, 16);
                    for (char* p = buf; *p && count < size - 1; p++) {
                        *out++ = *p;
                        count++;
                    }
                    break;
                }
                case 's': {
                    const char* val = va_arg(ap, const char*);
                    if (!val) val = "(null)";
                    while (*val && count < size - 1) {
                        *out++ = *val++;
                        count++;
                    }
                    break;
                }
                case 'c': {
                    char val = (char)va_arg(ap, int);
                    *out++ = val;
                    count++;
                    break;
                }
                case '%':
                    *out++ = '%';
                    count++;
                    break;
                default:
                    *out++ = '%';
                    *out++ = *format;
                    count += 2;
                    break;
            }
        } else {
            *out++ = *format;
            count++;
        }
        format++;
    }
    
    if (size > 0) {
        *out = '\0';
    }
    
    return (int)count;
}

// Character I/O
int putchar(int c) {
    char str[2] = {(char)c, '\0'};
    console_printf(str);
    return c;
}

int puts(const char* s) {
    console_printf(s);
    console_printf("\n");
    return 0;
}

int fputc(int c, FILE* stream) {
    (void)stream;
    return putchar(c);
}

int fputs(const char* s, FILE* stream) {
    (void)stream;
    return puts(s);
}

// File operations (stubs for kernel)
FILE* fopen(const char* path, const char* mode) {
    (void)path;
    (void)mode;
    // Kernel doesn't have real file streams
    return NULL;
}

int fclose(FILE* stream) {
    (void)stream;
    return 0;
}

size_t fread(void* ptr, size_t size, size_t nmemb, FILE* stream) {
    (void)ptr;
    (void)size;
    (void)nmemb;
    (void)stream;
    return 0;
}

size_t fwrite(const void* ptr, size_t size, size_t nmemb, FILE* stream) {
    (void)ptr;
    (void)size;
    (void)nmemb;
    (void)stream;
    return 0;
}

int fseek(FILE* stream, long offset, int whence) {
    (void)stream;
    (void)offset;
    (void)whence;
    return -1;
}

long ftell(FILE* stream) {
    (void)stream;
    return -1;
}

void rewind(FILE* stream) {
    (void)stream;
}

int fflush(FILE* stream) {
    (void)stream;
    return 0;
}

int feof(FILE* stream) {
    (void)stream;
    return 0;
}

int ferror(FILE* stream) {
    (void)stream;
    return 0;
}

void clearerr(FILE* stream) {
    (void)stream;
}

// Error handling
void perror(const char* s) {
    if (s) {
        console_printf(s);
        console_printf(": ");
    }
    console_printf("error\n");
}

// File operations
int remove(const char* pathname) {
    (void)pathname;
    return -1;
}

int rename(const char* oldpath, const char* newpath) {
    (void)oldpath;
    (void)newpath;
    return -1;
}

FILE* tmpfile(void) {
    return NULL;
}

char* tmpnam(char* s) {
    (void)s;
    return NULL;
}

// Buffering
void setbuf(FILE* stream, char* buf) {
    (void)stream;
    (void)buf;
}

int setvbuf(FILE* stream, char* buf, int mode, size_t size) {
    (void)stream;
    (void)buf;
    (void)mode;
    (void)size;
    return 0;
}

// Kernel printf wrapper
int kernel_printf(const char* format, ...) {
    va_list args;
    va_start(args, format);
    int ret = kernel_vprintf(format, args);
    va_end(args);
    return ret;
}

int kernel_vprintf(const char* format, va_list args) {
    console_vprintf(format, args);
    return 0;
}
