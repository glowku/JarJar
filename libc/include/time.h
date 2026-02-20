#include <kernel/types.h>
#ifndef JARJARVIS_TIME_H
#define JARJARVIS_TIME_H


#ifdef __cplusplus
extern "C" {
#endif

// 2. Clock ID et types de base
typedef int clockid_t;

// On utilise un define pour empêcher MSYS2 de redéfinir time_t
#ifndef _TIME_T_DECLARED
#define _TIME_T_DECLARED
typedef s64 time_t; 
#endif

#ifndef _CLOCK_T_DECLARED
#define _CLOCK_T_DECLARED
typedef s64 clock_t;
#endif

typedef unsigned int useconds_t;
typedef s64 suseconds_t;

// 3. On inclut les types du kernel APRES nos définitions prioritaires
#include <kernel/types.h>
#include <stddef.h>

// Clock ID
#define CLOCK_REALTIME           0
#define CLOCK_MONOTONIC          1
#define CLOCK_PROCESS_CPUTIME_ID 2
#define CLOCK_THREAD_CPUTIME_ID  3
#define CLOCKS_PER_SEC 1000000L

struct tm {
    int tm_sec; int tm_min; int tm_hour;
    int tm_mday; int tm_mon; int tm_year;
    int tm_wday; int tm_yday; int tm_isdst;
};

struct timespec {
    time_t tv_sec;
    long tv_nsec;
};

struct timeval {
    time_t tv_sec;
    suseconds_t tv_usec;
};

struct timezone {
    int tz_minuteswest;
    int tz_dsttime;
};

// Fonctions
time_t time(time_t* tloc);
double difftime(time_t time1, time_t time0);
time_t mktime(struct tm* tm);
struct tm* gmtime(const time_t* timep);
struct tm* localtime(const time_t* timep);
size_t strftime(char* s, size_t max, const char* format, const struct tm* tm);
char* asctime(const struct tm* tm);
char* ctime(const time_t* timep);

clock_t clock(void);
int clock_gettime(clockid_t clk_id, struct timespec* tp);
int clock_settime(clockid_t clk_id, const struct timespec* tp);
int clock_getres(clockid_t clk_id, struct timespec* res);

unsigned int sleep(unsigned int seconds);
int usleep(useconds_t usec);
int nanosleep(const struct timespec* req, struct timespec* rem);

typedef int timer_t;
struct itimerspec {
    struct timespec it_interval;
    struct timespec it_value;
};

int timer_create(clockid_t clockid, void* evp, timer_t* timerid);
int timer_delete(timer_t timerid);
int timer_settime(timer_t timerid, int flags, const struct itimerspec* new_value, struct itimerspec* old_value);
int timer_gettime(timer_t timerid, struct itimerspec* curr_value);
int timer_getoverrun(timer_t timerid);

#ifdef __cplusplus
}
#endif

#endif // JARJARVIS_TIME_H