#ifndef JARJARVIS_ASSERT_H
#define JARJARVIS_ASSERT_H

#include <kernel/panic.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifdef NDEBUG
    #define assert(expr) ((void)0)
#else
    #define assert(expr) \
        do { \
            if (!(expr)) { \
                panic_assert(__FILE__, __LINE__, #expr); \
            } \
        } while (0)
#endif

#define static_assert _Static_assert

#ifdef __cplusplus
}
#endif

#endif // JARJARVIS_ASSERT_H
