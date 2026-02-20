#ifndef JARJARVIS_STRING_H
#define JARJARVIS_STRING_H

#include <kernel/types.h>

#ifdef __cplusplus
extern "C" {
#endif

// Opérations sur la mémoire
void* memcpy(void* dest, const void* src, size_t n);
void* memmove(void* dest, const void* src, size_t n);
void* memset(void* s, int c, size_t n);
int memcmp(const void* s1, const void* s2, size_t n);
void* memchr(const void* s, int c, size_t n);
void* memrchr(const void* s, int c, size_t n);
void* memmem(const void* haystack, size_t haystacklen, const void* needle, size_t needlelen);

// Opérations sur les chaînes
size_t strlen(const char* s);
size_t strnlen(const char* s, size_t maxlen);
char* strcpy(char* dest, const char* src);
char* strncpy(char* dest, const char* src, size_t n);
char* strcat(char* dest, const char* src);
char* strncat(char* dest, const char* src, size_t n);
int strcmp(const char* s1, const char* s2);
int strncmp(const char* s1, const char* s2, size_t n);
int strcasecmp(const char* s1, const char* s2);
int strncasecmp(const char* s1, const char* s2, size_t n);

// Recherche dans les chaînes
char* strchr(const char* s, int c);
char* strrchr(const char* s, int c);
char* strstr(const char* haystack, const char* needle);
char* strcasestr(const char* haystack, const char* needle);
char* strpbrk(const char* s, const char* accept);
size_t strspn(const char* s, const char* accept);
size_t strcspn(const char* s, const char* reject);
char* strtok(char* str, const char* delim);
char* strtok_r(char* str, const char* delim, char** saveptr);

// Manipulation de chaînes
char* strdup(const char* s);
char* strndup(const char* s, size_t n);
char* strerror(int errnum);

// Conversion
long strtol(const char* nptr, char** endptr, int base);
long long strtoll(const char* nptr, char** endptr, int base);
unsigned long strtoul(const char* nptr, char** endptr, int base);
unsigned long long strtoull(const char* nptr, char** endptr, int base);
double strtod(const char* nptr, char** endptr);
float strtof(const char* nptr, char** endptr);

// Informations sur les caractères
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
int tolower(int c);
int toupper(int c);

// Chaînes sécurisées (BSD)
size_t strlcpy(char* dst, const char* src, size_t size);
size_t strlcat(char* dst, const char* src, size_t size);

// Extensions GNU
char* strsep(char** stringp, const char* delim);
void explicit_bzero(void* s, size_t n);

// Fonctions spécifiques à JARJARVIS
char* strreplace(char* str, const char* old, const char* new);
char** strsplit(const char* str, const char* delim, int* count);
void strsplit_free(char** tokens);
char* strtrim(char* str);
char* strltrim(char* str);
char* strrtrim(char* str);
bool startswith(const char* str, const char* prefix);
bool endswith(const char* str, const char* suffix);

#ifdef __cplusplus
}
#endif

#endif // JARJARVIS_STRING_H
