#ifndef JARJARVIS_SCHEDULER_H
#define JARJARVIS_SCHEDULER_H

#include <kernel/types.h>
#include <kernel/interrupt.h>
#include <kernel/memory.h>

#ifdef __cplusplus
extern "C" {
#endif

// États d'un processus
typedef enum {
    PROC_STATE_NEW = 0,
    PROC_STATE_READY,
    PROC_STATE_RUNNING,
    PROC_STATE_BLOCKED,
    PROC_STATE_SLEEPING,
    PROC_STATE_ZOMBIE,
    PROC_STATE_DEAD,
} process_state_t;

// Types de processus
typedef enum {
    PROC_TYPE_KERNEL = 0,
    PROC_TYPE_USER,
    PROC_TYPE_AI_DAEMON,    // Processus IA spécial
    PROC_TYPE_SERVICE,      // Service système
} process_type_t;

// Priorités
typedef enum {
    PRIORITY_IDLE = 0,
    PRIORITY_LOW = 1,
    PRIORITY_NORMAL = 2,
    PRIORITY_HIGH = 3,
    PRIORITY_REALTIME = 4,
    PRIORITY_AI_CRITICAL = 5,  // Priorité maximale pour l'IA
} priority_t;

// Structure de contexte de processus
typedef struct process_context {
    u64 rax, rbx, rcx, rdx;
    u64 rsi, rdi, rbp;
    u64 r8, r9, r10, r11;
    u64 r12, r13, r14, r15;
    u64 rip, rflags;
    u64 rsp;
    u64 cr3;
} process_context_t;

// Structure de file descriptor
typedef struct fd_table {
    struct file* files[256];
    u32 count;
} fd_table_t;

// Structure de processus
typedef struct process {
    pid_t pid;
    pid_t ppid;
    tid_t main_thread;
    
    char name[256];
    process_state_t state;
    process_type_t type;
    priority_t priority;
    priority_t base_priority;
    
    process_context_t context;
    vmm_t* vmm;
    
    u64 kernel_stack;
    u64 user_stack;
    
    u64 creation_time;
    u64 cpu_time;
    u64 sleep_until;
    
    fd_table_t* fd_table;
    
    // Signaux
    u32 pending_signals;
    u32 signal_mask;
    void* signal_handlers[32];
    
    // Statistiques
    u64 context_switches;
    u64 page_faults;
    u64 syscalls;
    
    // IA spécifique
    bool ai_enhanced;
    u64 ai_memory_quota;
    u64 ai_cpu_quota;
    
    // Liste chaînée
    struct process* next;
    struct process* prev;
    struct process* parent;
    struct process* children;
    struct process* sibling;
} process_t;

// Structure de thread
typedef struct thread {
    tid_t tid;
    pid_t pid;
    
    char name[64];
    process_state_t state;
    priority_t priority;
    
    process_context_t context;
    u64 kernel_stack;
    u64 user_stack;
    
    u64 cpu_time;
    u64 sleep_until;
    
    struct thread* next;
    struct thread* prev;
} thread_t;

// Ordonnanceur
typedef struct {
    process_t* current;
    process_t* idle;
    
    process_t* ready_queues[6];  // Une par priorité
    u32 ready_count;
    
    u64 ticks;
    u32 time_slice;
    u32 quantum;
    
    bool enabled;
    bool need_reschedule;
    
    // Statistiques
    u64 context_switches;
    u64 preempt_count;
    u64 idle_time;
    u64 busy_time;
} scheduler_t;

// Initialisation
void scheduler_init(void);
void scheduler_enable(void);
void scheduler_disable(void);

// Gestion des processus
process_t* process_create(const char* name, process_type_t type, priority_t priority);
void process_destroy(process_t* proc);
jar_result_t process_exec(process_t* proc, const char* path, char* const argv[]);
void process_exit(int code);
pid_t process_wait(pid_t pid, int* status, int options);
process_t* process_get_current(void);
process_t* process_get_by_pid(pid_t pid);
void process_set_priority(process_t* proc, priority_t priority);
void process_block(process_t* proc);
void process_unblock(process_t* proc);
void process_sleep(u64 ms);
void process_yield(void);

// Threads
thread_t* thread_create(process_t* proc, void (*entry)(void*), void* arg);
void thread_destroy(thread_t* thread);
void thread_exit(void);

// Ordonnancement
void schedule(void);
void schedule_tick(void);
void schedule_reschedule(void);
void context_switch(process_context_t* old, process_context_t* new);

// IDLE
void idle_task(void);
void idle_init(void);

// Statistiques
typedef struct {
    u32 process_count;
    u32 thread_count;
    u32 running_count;
    u32 blocked_count;
    u64 total_cpu_time;
    u64 idle_cpu_time;
    float cpu_usage;
} scheduler_stats_t;

void scheduler_get_stats(scheduler_stats_t* stats);
void scheduler_dump_processes(void);

// IA spécifique
void scheduler_boost_ai_processes(void);
void scheduler_set_ai_quotas(process_t* proc, u64 memory, u64 cpu_percent);

#ifdef __cplusplus
}
#endif

#endif // JARJARVIS_SCHEDULER_H
