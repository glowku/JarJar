#ifndef JARJARVIS_STDLIB_H
#define JARJARVIS_STDLIB_H

#include <kernel/types.h>

#ifdef __cplusplus
extern "C" {
#endif

// Div types
typedef struct {
    int quot;
    int rem;
} div_t;

typedef struct {
    long quot;
    long rem;
} ldiv_t;

typedef struct {
    long long quot;
    long long rem;
} lldiv_t;

// String conversion
int atoi(const char* str);
long atol(const char* str);
double atof(const char* str);

long strtol(const char* nptr, char** endptr, int base);
unsigned long strtoul(const char* nptr, char** endptr, int base);
long long strtoll(const char* nptr, char** endptr, int base);
unsigned long long strtoull(const char* nptr, char** endptr, int base);
double strtod(const char* nptr, char** endptr);
float strtof(const char* nptr, char** endptr);

// Memory allocation
void* malloc(size_t size);
void* calloc(size_t nmemb, size_t size);
void* realloc(void* ptr, size_t size);
void free(void* ptr);

// Process control
void abort(void);
void exit(int status);
int atexit(void (*func)(void));
void _Exit(int status);

// Environment
char* getenv(const char* name);
int setenv(const char* name, const char* value, int overwrite);
int unsetenv(const char* name);
int putenv(char* string);
int system(const char* command);

// Searching and sorting
void* bsearch(const void* key, const void* base, size_t nmemb, size_t size,
              int (*compar)(const void*, const void*));
void qsort(void* base, size_t nmemb, size_t size,
           int (*compar)(const void*, const void*));

// Integer arithmetic
int abs(int n);
long labs(long n);
long long llabs(long long n);

div_t div(int numer, int denom);
ldiv_t ldiv(long numer, long denom);
lldiv_t lldiv(long long numer, long long denom);

// Random numbers
int rand(void);
void srand(unsigned int seed);

// Multibyte characters (stubs)
int mblen(const char* s, size_t n);
int mbtowc(wchar_t* pwc, const char* s, size_t n);
int wctomb(char* s, wchar_t wchar);
size_t mbstowcs(wchar_t* dest, const char* src, size_t n);
size_t wcstombs(char* dest, const wchar_t* src, size_t n);

// Non-standard but useful
char* itoa(int value, char* str, int base);
char* ltoa(long value, char* str, int base);
char* utoa(unsigned int value, char* str, int base);
char* ultoa(unsigned long value, char* str, int base);

// Constants
#define EXIT_SUCCESS 0
#define EXIT_FAILURE 1

#define RAND_MAX 32767

#ifdef __cplusplus
}
#endif

#endif // JARJARVIS_STDLIB_H
