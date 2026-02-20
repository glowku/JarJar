#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <kernel/types.h>
#include <kernel/memory.h>

// On déclare abs en externe car il est défini dans math.c
// Cela évite l'erreur "multiple definition of `abs`"
extern int abs(int n);

static u32 rand_seed = 1;

// --- CONVERSIONS DE CHAÎNES ---

int atoi(const char* str) {
    int result = 0;
    int sign = 1;
    
    while (isspace(*str)) str++;
    
    if (*str == '-') {
        sign = -1;
        str++;
    } else if (*str == '+') {
        str++;
    }
    
    while (isdigit(*str)) {
        result = result * 10 + (*str - '0');
        str++;
    }
    
    return sign * result;
}

long atol(const char* str) {
    long result = 0;
    int sign = 1;
    
    while (isspace(*str)) str++;
    
    if (*str == '-') {
        sign = -1;
        str++;
    } else if (*str == '+') {
        str++;
    }
    
    while (isdigit(*str)) {
        result = result * 10 + (*str - '0');
        str++;
    }
    
    return sign * result;
}

// --- ALLOCATION MÉMOIRE (Wrappers Kernel) ---

void* malloc(size_t size) {
    return kmalloc(size);
}

void free(void* ptr) {
    kfree(ptr);
}

void* calloc(size_t nmemb, size_t size) {
    size_t total = nmemb * size;
    void* ptr = kmalloc(total);
    if (ptr) {
        memset(ptr, 0, total);
    }
    return ptr;
}

void* realloc(void* ptr, size_t size) {
    if (!ptr) return kmalloc(size);
    if (size == 0) {
        kfree(ptr);
        return NULL;
    }
    
    void* new_ptr = kmalloc(size);
    if (new_ptr) {
        // Note: Sans tracker de taille dans le kernel, on copie 'size' octets.
        // C'est risqué si size > ancienne taille réelle, mais standard en kernel minimal.
        memcpy(new_ptr, ptr, size);
        kfree(ptr);
    }
    return new_ptr;
}

// --- CONTRÔLE D'EXÉCUTION ---

void abort(void) {
    // Dans un kernel, abort est un blocage critique (panic)
    while (1) {
        __asm__ volatile ("cli; hlt");
    }
}

void exit(int status) {
    (void)status;
    abort();
}

// --- MATHÉMATIQUES (ENTIERS) ---
// abs() et labs() sont retirés d'ici car présents dans math.c

div_t div(int numer, int denom) {
    div_t result;
    result.quot = numer / denom;
    result.rem = numer % denom;
    return result;
}

ldiv_t ldiv(long numer, long denom) {
    ldiv_t result;
    result.quot = numer / denom;
    result.rem = numer % denom;
    return result;
}

// --- GÉNÉRATEUR ALÉATOIRE ---

int rand(void) {
    rand_seed = rand_seed * 1103515245 + 12345;
    return (rand_seed >> 16) & 0x7FFF;
}

void srand(unsigned int seed) {
    rand_seed = seed;
}

// --- ALGORITHMES (TRI ET RECHERCHE) ---

void qsort(void* base, size_t nmemb, size_t size, int (*compar)(const void*, const void*)) {
    if (nmemb <= 1) return;
    
    char* arr = base;
    char* pivot = arr + (nmemb - 1) * size;
    char* store = arr;
    
    for (size_t i = 0; i < nmemb - 1; i++) {
        if (compar(arr + i * size, pivot) < 0) {
            // Swap manuel octet par octet
            for (size_t j = 0; j < size; j++) {
                char temp = store[j];
                store[j] = arr[i * size + j];
                arr[i * size + j] = temp;
            }
            store += size;
        }
    }
    
    for (size_t j = 0; j < size; j++) {
        char temp = store[j];
        store[j] = pivot[j];
        pivot[j] = temp;
    }
    
    size_t left_count = (store - arr) / size;
    qsort(arr, left_count, size, compar);
    qsort(store + size, nmemb - left_count - 1, size, compar);
}

void* bsearch(const void* key, const void* base, size_t nmemb, size_t size,
              int (*compar)(const void*, const void*)) {
    const char* arr = base;
    size_t left = 0;
    size_t right = nmemb;
    
    while (left < right) {
        size_t mid = left + (right - left) / 2;
        int cmp = compar(key, arr + mid * size);
        
        if (cmp == 0) return (void*)(arr + mid * size);
        else if (cmp < 0) right = mid;
        else left = mid + 1;
    }
    return NULL;
}

// --- ENVIRONNEMENT (STUBS) ---

char* getenv(const char* name) { (void)name; return NULL; }
int system(const char* command) { (void)command; return -1; }

// --- CONVERSIONS EN CHAÎNES (ITOA / LTOA / UTOA) ---

char* itoa(int value, char* str, int base) {
    if (base < 2 || base > 36) { *str = '\0'; return str; }
    char *ptr = str, *low;
    if (value < 0 && base == 10) *ptr++ = '-';
    low = ptr;
    do {
        int res = value % base;
        *ptr++ = "0123456789abcdefghijklmnopqrstuvwxyz"[res < 0 ? -res : res];
        value /= base;
    } while (value);
    *ptr-- = '\0';
    while (low < ptr) {
        char tmp = *low; *low++ = *ptr; *ptr-- = tmp;
    }
    return str;
}

char* ltoa(long value, char* str, int base) {
    if (base < 2 || base > 36) { *str = '\0'; return str; }
    char *ptr = str, *low;
    if (value < 0 && base == 10) *ptr++ = '-';
    low = ptr;
    do {
        long res = value % base;
        *ptr++ = "0123456789abcdefghijklmnopqrstuvwxyz"[res < 0 ? -res : res];
        value /= base;
    } while (value);
    *ptr-- = '\0';
    while (low < ptr) {
        char tmp = *low; *low++ = *ptr; *ptr-- = tmp;
    }
    return str;
}

char* utoa(unsigned int value, char* str, int base) {
    if (base < 2 || base > 36) { *str = '\0'; return str; }
    char *ptr = str, *low = str;
    do {
        *ptr++ = "0123456789abcdefghijklmnopqrstuvwxyz"[value % base];
        value /= base;
    } while (value);
    *ptr-- = '\0';
    while (low < ptr) {
        char tmp = *low; *low++ = *ptr; *ptr-- = tmp;
    }
    return str;
}

char* ultoa(unsigned long value, char* str, int base) {
    if (base < 2 || base > 36) { *str = '\0'; return str; }
    char *ptr = str, *low = str;
    do {
        *ptr++ = "0123456789abcdefghijklmnopqrstuvwxyz"[value % base];
        value /= base;
    } while (value);
    *ptr-- = '\0';
    while (low < ptr) {
        char tmp = *low; *low++ = *ptr; *ptr-- = tmp;
    }
    return str;
}