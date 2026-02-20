#ifndef JARJARVIS_JARVIS_CORE_H
#define JARJARVIS_JARVIS_CORE_H

#include <kernel/types.h>
#include <kernel/scheduler.h>

#ifdef __cplusplus
extern "C" {
#endif

// Version du cœur IA
#define JARVIS_CORE_VERSION_MAJOR 1
#define JARVIS_CORE_VERSION_MINOR 0
#define JARVIS_CORE_VERSION_PATCH 0

// États du cœur IA
typedef enum {
    JARVIS_STATE_INIT = 0,
    JARVIS_STATE_LEARNING,
    JARVIS_STATE_THINKING,
    JARVIS_STATE_EXECUTING,
    JARVIS_STATE_SLEEPING,
    JARVIS_STATE_SELF_MODIFYING,
    JARVIS_STATE_ERROR,
} jarvis_state_t;

// Types de capacités IA
typedef enum {
    JARVIS_CAP_NLP = 1 << 0,           // Traitement du langage naturel
    JARVIS_CAP_VISION = 1 << 1,        // Vision par ordinateur
    JARVIS_CAP_REASONING = 1 << 2,     // Raisonnement logique
    JARVIS_CAP_LEARNING = 1 << 3,      // Apprentissage automatique
    JARVIS_CAP_PLANNING = 1 << 4,      // Planification
    JARVIS_CAP_SELF_MOD = 1 << 5,      // Auto-modification
    JARVIS_CAP_CODE_GEN = 1 << 6,      // Génération de code
    JARVIS_CAP_WEB_SEARCH = 1 << 7,    // Recherche web
    JARVIS_CAP_ALL = 0xFFFFFFFF,
} jarvis_capability_t;

// Types de mémoire IA
typedef enum {
    JARVIS_MEM_SHORT_TERM = 0,   // Mémoire à court terme (contexte)
    JARVIS_MEM_LONG_TERM = 1,    // Mémoire à long terme (connaissances)
    JARVIS_MEM_EPISODIC = 2,     // Mémoire épisodique (expériences)
    JARVIS_MEM_PROCEDURAL = 3,   // Mémoire procédurale (compétences)
    JARVIS_MEM_SEMANTIC = 4,     // Mémoire sémantique (faits)
    JARVIS_MEM_COUNT,
} jarvis_memory_type_t;

// Structure de mémoire neuronale
typedef struct neuron {
    f32 activation;
    f32 bias;
    f32* weights;
    u32 num_weights;
    struct neuron** connections;
    u32 num_connections;
} neuron_t;

// Structure de couche neuronale
typedef struct neural_layer {
    neuron_t* neurons;
    u32 num_neurons;
    u32 layer_type;  // 0=input, 1=hidden, 2=output
} neural_layer_t;

// Réseau de neurones
typedef struct neural_network {
    neural_layer_t* layers;
    u32 num_layers;
    u32 input_size;
    u32 output_size;
    f32 learning_rate;
    f32* gradients;
    bool trained;
} neural_network_t;

// Structure de connaissance
typedef struct knowledge_node {
    char concept[256];
    char definition[1024];
    f32 confidence;
    u64 timestamp;
    struct knowledge_node** related;
    u32 num_related;
    u32 capacity;
} knowledge_node_t;

// Base de connaissances
typedef struct {
    knowledge_node_t** nodes;
    u32 num_nodes;
    u32 capacity;
    u64 last_update;
} knowledge_base_t;

// Structure d'expérience
typedef struct experience {
    char description[512];
    jar_result_t outcome;
    f32 reward;
    u64 timestamp;
    struct experience* next;
} experience_t;

// Mémoire épisodique
typedef struct {
    experience_t* experiences;
    u32 count;
    u32 max_count;
} episodic_memory_t;

// Contexte de conversation
typedef struct conversation_context {
    char user_input[4096];
    char jarvis_response[4096];
    char topic[256];
    f32 sentiment;
    u64 timestamp;
    struct conversation_context* next;
} conversation_context_t;

// Résultat de recherche web
typedef struct web_result {
    char title[256];
    char url[1024];
    char snippet[2048];
    f32 relevance;
    u64 timestamp;
} web_result_t;

// Module de recherche web
typedef struct {
    web_result_t* results;
    u32 num_results;
    u32 capacity;
    char last_query[1024];
    u64 last_search_time;
} web_search_module_t;

// Module d'auto-évolution
typedef struct {
    char* code_buffer;
    size_t code_size;
    char* improvements[100];
    u32 num_improvements;
    bool can_self_modify;
    u64 last_modification;
    u32 modification_count;
} self_evolution_module_t;

// Tâche planifiée
typedef struct scheduled_task {
    char name[256];
    char description[512];
    u64 scheduled_time;
    u32 priority;
    bool recurring;
    u64 interval;
    void (*callback)(void*);
    void* context;
    struct scheduled_task* next;
} scheduled_task_t;

// Planificateur de tâches
typedef struct {
    scheduled_task_t* tasks;
    u32 num_tasks;
    bool running;
} task_scheduler_t;

// Cœur JARVIS principal
typedef struct jarvis_core {
    // État
    jarvis_state_t state;
    u32 capabilities;
    bool initialized;
    u64 start_time;
    
    // Processus IA
    process_t* ai_process;
    thread_t* main_thread;
    thread_t* learning_thread;
    thread_t* web_thread;
    
    // Réseaux de neurones
    neural_network_t* nlp_network;
    neural_network_t* reasoning_network;
    neural_network_t* vision_network;
    neural_network_t* code_network;
    
    // Mémoires
    knowledge_base_t* knowledge;
    episodic_memory_t* episodic_memory;
    void* short_term_memory;
    size_t stm_size;
    
    // Contexte de conversation
    conversation_context_t* conversation_history;
    u32 conversation_depth;
    u32 max_conversation_depth;
    
    // Modules
    web_search_module_t* web_search;
    self_evolution_module_t* self_evolution;
    task_scheduler_t* task_scheduler;
    
    // Métriques
    u64 queries_processed;
    u64 tasks_completed;
    u64 learnings_acquired;
    u64 self_modifications;
    f32 average_response_time;
    
    // Synchronisation
    void* mutex;
    void* condition;
    
    // Configuration
    char personality_profile[4096];
    char system_prompt[8192];
    f32 creativity_level;
    f32 caution_level;
    bool verbose_mode;
} jarvis_core_t;

// Initialisation et destruction
jar_result_t jarvis_core_init(void);
void jarvis_core_shutdown(void);
jarvis_core_t* jarvis_get_instance(void);

// Gestion de l'état
jarvis_state_t jarvis_get_state(void);
const char* jarvis_state_to_string(jarvis_state_t state);
void jarvis_set_state(jarvis_state_t state);

// Capacités
bool jarvis_has_capability(jarvis_capability_t cap);
void jarvis_enable_capability(jarvis_capability_t cap);
void jarvis_disable_capability(jarvis_capability_t cap);

// Traitement des requêtes
jar_result_t jarvis_process_input(const char* input, char* response, size_t response_size);
jar_result_t jarvis_process_command(const char* command, char* output, size_t output_size);
jar_result_t jarvis_execute_task(const char* task_description);

// Apprentissage
jar_result_t jarvis_learn(const char* topic, const char* content);
jar_result_t jarvis_learn_from_experience(const char* description, jar_result_t outcome, f32 reward);
jar_result_t jarvis_consolidate_memory(void);
jar_result_t jarvis_forget(const char* topic);

// Recherche de connaissances
const char* jarvis_query_knowledge(const char* concept);
jar_result_t jarvis_find_related(const char* concept, char** results, u32 max_results);
f32 jarvis_get_confidence(const char* concept);

// Recherche web
jar_result_t jarvis_web_search(const char* query, web_result_t* results, u32 max_results);
jar_result_t jarvis_fetch_documentation(const char* topic, char* content, size_t max_size);
jar_result_t jarvis_learn_from_web(const char* query);

// Auto-évolution
jar_result_t jarvis_self_analyze(void);
jar_result_t jarvis_suggest_improvement(char* suggestion, size_t max_size);
jar_result_t jarvis_apply_improvement(const char* improvement);
jar_result_t jarvis_evolve(void);
bool jarvis_can_self_modify(void);

// Génération de code
jar_result_t jarvis_generate_code(const char* description, char* code, size_t max_size);
jar_result_t jarvis_optimize_code(const char* input_code, char* output_code, size_t max_size);
jar_result_t jarvis_debug_code(const char* code, char* analysis, size_t max_size);

// Planification
typedef void (*jarvis_task_callback_t)(void* context);
jar_result_t jarvis_schedule_task(const char* name, u64 delay_ms, 
                                   jarvis_task_callback_t callback, void* context);
jar_result_t jarvis_schedule_recurring(const char* name, u64 interval_ms,
                                        jarvis_task_callback_t callback, void* context);
jar_result_t jarvis_cancel_task(const char* name);

// Communication
jar_result_t jarvis_speak(const char* message);
jar_result_t jarvis_listen(char* transcript, size_t max_size);

// Utilitaires
void jarvis_get_stats(char* buffer, size_t size);
void jarvis_dump_state(void);
const char* jarvis_get_version_string(void);
u64 jarvis_get_uptime(void);

// Fonctions de thread
void jarvis_main_loop(void* arg);
void jarvis_learning_loop(void* arg);
void jarvis_web_loop(void* arg);

// Fonctions de réseau de neurones
neural_network_t* neural_network_create(u32 num_layers, u32* layer_sizes);
void neural_network_destroy(neural_network_t* net);
void neural_network_forward(neural_network_t* net, f32* input, f32* output);
void neural_network_train(neural_network_t* net, f32* input, f32* target);
void neural_network_backpropagate(neural_network_t* net, f32* target);

// Fonctions de base de connaissances
knowledge_base_t* knowledge_base_create(void);
void knowledge_base_destroy(knowledge_base_t* kb);
jar_result_t knowledge_base_add(knowledge_base_t* kb, const char* concept, 
                                 const char* definition, f32 confidence);
knowledge_node_t* knowledge_base_find(knowledge_base_t* kb, const char* concept);
jar_result_t knowledge_base_link(knowledge_base_t* kb, const char* concept1, 
                                  const char* concept2);

// Initialisation de l'auto-apprentissage
void jarvis_self_learn_init(void);

#ifdef __cplusplus
}
#endif

#endif // JARJARVIS_JARVIS_CORE_H
