#include <string.h>
#include <ctype.h>     // Pour tolower, isspace, isdigit, etc.
#include <kernel/types.h>
#include <stdbool.h>

// --- DÉCLARATIONS EXTERNES ---
// Ces fonctions doivent être implémentées dans ton kernel (ex: memory.c)
extern void* kmalloc(size_t size);
extern void kfree(void* ptr);

// --- OPÉRATIONS SUR LA MÉMOIRE ---

void* memcpy(void* dest, const void* src, size_t n) {
    u8* d = dest;
    const u8* s = src;
    while (n--) {
        *d++ = *s++;
    }
    return dest;
}

void* memmove(void* dest, const void* src, size_t n) {
    u8* d = dest;
    const u8* s = src;
    
    if (d < s) {
        while (n--) {
            *d++ = *s++;
        }
    } else {
        d += n;
        s += n;
        while (n--) {
            *--d = *--s;
        }
    }
    return dest;
}

void* memset(void* s, int c, size_t n) {
    u8* p = s;
    while (n--) {
        *p++ = (u8)c;
    }
    return s;
}

int memcmp(const void* s1, const void* s2, size_t n) {
    const u8* p1 = s1;
    const u8* p2 = s2;
    while (n--) {
        if (*p1 != *p2) {
            return *p1 - *p2;
        }
        p1++;
        p2++;
    }
    return 0;
}

void* memchr(const void* s, int c, size_t n) {
    const u8* p = s;
    while (n--) {
        if (*p == (u8)c) {
            return (void*)p;
        }
        p++;
    }
    return NULL;
}

// --- OPÉRATIONS SUR LES CHAÎNES ---

size_t strlen(const char* s) {
    const char* p = s;
    while (*p) p++;
    return p - s;
}

size_t strnlen(const char* s, size_t maxlen) {
    const char* p = s;
    while (*p && maxlen--) p++;
    return p - s;
}

char* strcpy(char* dest, const char* src) {
    char* d = dest;
    while ((*d++ = *src++));
    return dest;
}

char* strncpy(char* dest, const char* src, size_t n) {
    char* d = dest;
    while (n && (*d++ = *src++)) {
        n--;
    }
    while (n--) {
        *d++ = '\0';
    }
    return dest;
}

char* strcat(char* dest, const char* src) {
    char* d = dest;
    while (*d) d++;
    while ((*d++ = *src++));
    return dest;
}

char* strncat(char* dest, const char* src, size_t n) {
    char* d = dest;
    while (*d) d++;
    while (n-- && (*d++ = *src++));
    *d = '\0';
    return dest;
}

int strcmp(const char* s1, const char* s2) {
    while (*s1 && (*s1 == *s2)) {
        s1++;
        s2++;
    }
    return *(unsigned char*)s1 - *(unsigned char*)s2;
}

int strncmp(const char* s1, const char* s2, size_t n) {
    while (n && *s1 && (*s1 == *s2)) {
        s1++;
        s2++;
        n--;
    }
    if (n == 0) return 0;
    return *(unsigned char*)s1 - *(unsigned char*)s2;
}

int strcasecmp(const char* s1, const char* s2) {
    while (*s1 && (tolower((unsigned char)*s1) == tolower((unsigned char)*s2))) {
        s1++;
        s2++;
    }
    return tolower((unsigned char)*s1) - tolower((unsigned char)*s2);
}

// --- RECHERCHE ---

char* strchr(const char* s, int c) {
    while (*s) {
        if (*s == (char)c) return (char*)s;
        s++;
    }
    return (c == '\0') ? (char*)s : NULL;
}

char* strrchr(const char* s, int c) {
    const char* last = NULL;
    while (*s) {
        if (*s == (char)c) last = s;
        s++;
    }
    if (c == '\0') return (char*)s;
    return (char*)last;
}

char* strstr(const char* haystack, const char* needle) {
    if (!*needle) return (char*)haystack;
    while (*haystack) {
        const char* h = haystack;
        const char* n = needle;
        while (*h && *n && *h == *n) {
            h++; n++;
        }
        if (!*n) return (char*)haystack;
        haystack++;
    }
    return NULL;
}

char* strpbrk(const char* s, const char* accept) {
    while (*s) {
        if (strchr(accept, *s)) return (char*)s;
        s++;
    }
    return NULL;
}

size_t strspn(const char* s, const char* accept) {
    const char* p = s;
    while (*p && strchr(accept, *p)) p++;
    return p - s;
}

size_t strcspn(const char* s, const char* reject) {
    const char* p = s;
    while (*p && !strchr(reject, *p)) p++;
    return p - s;
}

// --- TOKENIZATION ---

char* strtok_r(char* str, const char* delim, char** saveptr) {
    if (str) *saveptr = str;
    if (!*saveptr) return NULL;

    *saveptr += strspn(*saveptr, delim);
    if (!**saveptr) {
        *saveptr = NULL;
        return NULL;
    }

    char* token = *saveptr;
    *saveptr += strcspn(*saveptr, delim);

    if (**saveptr) {
        **saveptr = '\0';
        (*saveptr)++;
    } else {
        *saveptr = NULL;
    }
    return token;
}

char* strtok(char* str, const char* delim) {
    static char* saveptr = NULL;
    return strtok_r(str, delim, &saveptr);
}

// --- ALLOCATION ---

char* strdup(const char* s) {
    size_t len = strlen(s) + 1;
    char* dup = kmalloc(len);
    if (dup) memcpy(dup, s, len);
    return dup;
}

char* strndup(const char* s, size_t n) {
    size_t len = strnlen(s, n);
    char* dup = kmalloc(len + 1);
    if (dup) {
        memcpy(dup, s, len);
        dup[len] = '\0';
    }
    return dup;
}

// --- CONVERSION (strtol / strtoul) ---

unsigned long strtoul(const char* nptr, char** endptr, int base) {
    const char* p = nptr;
    unsigned long result = 0;
    while (isspace((unsigned char)*p)) p++;
    if (*p == '+' || *p == '-') p++; // Simplifié : ignore le signe pour unsigned
    
    if (base == 0) {
        if (*p == '0') {
            if (tolower((unsigned char)p[1]) == 'x') { base = 16; p += 2; }
            else { base = 8; p++; }
        } else base = 10;
    } else if (base == 16 && *p == '0' && tolower((unsigned char)p[1]) == 'x') p += 2;

    while (*p) {
        int digit = isdigit((unsigned char)*p) ? *p - '0' : 
                    (isalpha((unsigned char)*p) ? tolower((unsigned char)*p) - 'a' + 10 : -1);
        if (digit < 0 || digit >= base) break;
        result = result * base + digit;
        p++;
    }
    if (endptr) *endptr = (char*)p;
    return result;
}

long strtol(const char* nptr, char** endptr, int base) {
    const char* p = nptr;
    while (isspace((unsigned char)*p)) p++;
    bool neg = (*p == '-');
    if (neg || *p == '+') p++;
    unsigned long res = strtoul(p, endptr, base);
    return neg ? -(long)res : (long)res;
}

// --- CHAINES SÉCURISÉES ---

size_t strlcpy(char* dst, const char* src, size_t size) {
    size_t len = strlen(src);
    if (size > 0) {
        size_t copy = (len < size - 1) ? len : size - 1;
        memcpy(dst, src, copy);
        dst[copy] = '\0';
    }
    return len;
}

size_t strlcat(char* dst, const char* src, size_t size) {
    size_t dlen = strnlen(dst, size);
    if (dlen == size) return size + strlen(src);
    return dlen + strlcpy(dst + dlen, src, size - dlen);
}

// --- FONCTIONS SPÉCIFIQUES JARVIS ---

char* strltrim(char* str) {
    while (*str && isspace((unsigned char)*str)) str++;
    return str;
}

char* strrtrim(char* str) {
    size_t len = strlen(str);
    if (len == 0) return str;
    char* end = str + len - 1;
    while (end > str && isspace((unsigned char)*end)) end--;
    *(end + 1) = '\0';
    return str;
}

char* strtrim(char* str) {
    return strrtrim(strltrim(str));
}

bool startswith(const char* str, const char* prefix) {
    return strncmp(str, prefix, strlen(prefix)) == 0;
}

bool endswith(const char* str, const char* suffix) {
    size_t str_len = strlen(str);
    size_t suffix_len = strlen(suffix);
    if (suffix_len > str_len) return false;
    return strcmp(str + str_len - suffix_len, suffix) == 0;
}