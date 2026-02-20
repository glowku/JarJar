#ifndef JARJARVIS_TYPES_H
#define JARJARVIS_TYPES_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdarg.h>

#ifdef __cplusplus
extern "C" {
#endif

// Types de base
typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef int8_t   s8;
typedef int16_t  s16;
typedef int32_t  s32;
typedef int64_t  s64;

// Types spéciaux
typedef u64 phys_addr_t;
typedef u64 virt_addr_t;
typedef u64 size_t;
typedef s64 ssize_t;
typedef u32 pid_t;
typedef u32 tid_t;
typedef float  f32;
typedef double f64;
// Résultat d'opération
typedef enum {
    JAR_OK = 0,
    JAR_ERROR = -1,
    JAR_NOMEM = -2,
    JAR_INVAL = -3,
    JAR_NOTFOUND = -4,
    JAR_EXISTS = -5,
    JAR_ACCESS = -6,
    JAR_BUSY = -7,
    JAR_TIMEOUT = -8,
    JAR_IO = -9,
    JAR_CORRUPT = -10,
    JAR_LEARNING = -11,  // En cours d'apprentissage
} jar_result_t;

// Attributs
// Attributs
#define __packed __attribute__((packed))
#define __aligned(x) __attribute__((aligned(x)))   // <--- Ajout du #
#define __noreturn __attribute__((noreturn))       // <--- Ajout du #
#define __interrupt __attribute__((interrupt))     // <--- Ajout du #
#define __used __attribute__((used))               // <--- Ajout du #
#define __section(x) __attribute__((section(x)))   // <--- Ajout du #

// Macros utilitaires
#define ALIGN_UP(x, align) (((x) + (align) - 1) & ~((align) - 1))
#define ALIGN_DOWN(x, align) ((x) & ~((align) - 1))
#define DIV_ROUND_UP(x, y) (((x) + (y) - 1) / (y))
#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))
#define CONTAINER_OF(ptr, type, member) ((type *)((char *)(ptr) - offsetof(type, member)))

// Limites
#define KB (1024ULL)
#define MB (1024ULL * KB)
#define GB (1024ULL * MB)
#define TB (1024ULL * GB)

// Adresses virtuelles du noyau
#define KERNEL_VIRTUAL_BASE 0xFFFF800000000000ULL
#define KERNEL_HEAP_START   0xFFFF800010000000ULL
#define KERNEL_HEAP_SIZE    (512 * MB)
#define KERNEL_STACK_SIZE   (256 * KB)

// Conversion adresses
#define PHYS_TO_VIRT(addr) ((virt_addr_t)(addr) + KERNEL_VIRTUAL_BASE)
#define VIRT_TO_PHYS(addr) ((phys_addr_t)(addr) - KERNEL_VIRTUAL_BASE)
#define IS_KERNEL_ADDR(addr) ((addr) >= KERNEL_VIRTUAL_BASE)

#ifdef __cplusplus
}
#endif

#endif // JARJARVIS_TYPES_H
