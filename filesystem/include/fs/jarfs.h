#ifndef JARJARVIS_JARFS_H
#define JARJARVIS_JARFS_H

#include <kernel/types.h>

#ifdef __cplusplus
extern "C" {
#endif

// Magic number pour JARFS
#define JARFS_MAGIC 0x4A415246534D4153ULL  // "JARFSMAS"

#define JARFS_VERSION_MAJOR 1
#define JARFS_VERSION_MINOR 0

// Taille des blocs
#define JARFS_BLOCK_SIZE 4096
#define JARFS_BLOCK_SHIFT 12

#define JARFS_MAX_FILENAME 255
#define JARFS_MAX_PATH 4096
#define JARFS_MAX_SYMLINK 1024

// Nombre d'inodes et blocs directs/indirects
#define JARFS_DIRECT_BLOCKS 12
#define JARFS_INDIRECT_BLOCKS 3  // Simple, double, triple

#define JARFS_INODE_CACHE_SIZE 1024
#define JARFS_BLOCK_CACHE_SIZE 4096

// Types de fichiers
typedef enum {
    JARFS_TYPE_UNKNOWN = 0,
    JARFS_TYPE_REGULAR = 1,
    JARFS_TYPE_DIRECTORY = 2,
    JARFS_TYPE_SYMLINK = 3,
    JARFS_TYPE_BLOCK_DEV = 4,
    JARFS_TYPE_CHAR_DEV = 5,
    JARFS_TYPE_FIFO = 6,
    JARFS_TYPE_SOCKET = 7,
} jarfs_file_type_t;

// Permissions
typedef enum {
    JARFS_PERM_READ = 0x04,
    JARFS_PERM_WRITE = 0x02,
    JARFS_PERM_EXEC = 0x01,
} jarfs_perm_t;

// Flags de montage
typedef enum {
    JARFS_MOUNT_READONLY = 1 << 0,
    JARFS_MOUNT_NOEXEC = 1 << 1,
    JARFS_MOUNT_NOSUID = 1 << 2,
    JARFS_MOUNT_SYNC = 1 << 3,
    JARFS_MOUNT_AI_OPTIMIZED = 1 << 4,  // Optimisé pour l'accès IA
} jarfs_mount_flags_t;

// Superblock JARFS
typedef struct __packed {
    u64 magic;
    u32 version_major;
    u32 version_minor;
    u64 block_count;
    u64 inode_count;
    u64 free_blocks;
    u64 free_inodes;
    u64 block_bitmap_start;
    u64 inode_bitmap_start;
    u64 inode_table_start;
    u64 data_start;
    u32 blocks_per_group;
    u32 inodes_per_group;
    u64 creation_time;
    u64 last_mount_time;
    u64 last_write_time;
    u16 mount_count;
    u16 max_mount_count;
    u32 features;
    u32 reserved[29];
    u64 checksum;
} jarfs_superblock_t;

// Descripteur de groupe
typedef struct __packed {
    u64 block_bitmap;
    u64 inode_bitmap;
    u64 inode_table;
    u16 free_blocks_count;
    u16 free_inodes_count;
    u16 used_dirs_count;
    u16 flags;
    u32 reserved[3];
    u32 checksum;
} jarfs_group_desc_t;

// Inode JARFS
typedef struct __packed {
    u16 mode;
    u16 uid;
    u32 size;
    u32 atime;
    u32 ctime;
    u32 mtime;
    u32 dtime;
    u16 gid;
    u16 links_count;
    u32 blocks;
    u32 flags;
    u32 osd1;
    u64 block[JARFS_DIRECT_BLOCKS + JARFS_INDIRECT_BLOCKS];
    u32 generation;
    u32 file_acl;
    u32 dir_acl;
    u32 faddr;
    u8 osd2[12];
    // Extensions JARFS
    u64 ai_metadata;      // Pointeur vers métadonnées IA
    u64 semantic_hash;    // Hash sémantique pour recherche IA
    u32 access_pattern;   // Pattern d'accès pour optimisation
    u32 reserved;
} jarfs_inode_t;

// Entrée de répertoire
typedef struct __packed {
    u32 inode;
    u16 rec_len;
    u8 name_len;
    u8 file_type;
    char name[JARFS_MAX_FILENAME];
} jarfs_dir_entry_t;

// Métadonnées IA
typedef struct {
    u64 creation_context;    // Contexte de création
    u64 last_ai_access;      // Dernier accès par l'IA
    u32 importance_score;    // Score d'importance pour l'IA
    u32 access_frequency;    // Fréquence d'accès
    u64 semantic_vector[8];  // Vecteur sémantique pour recherche
    char ai_tags[256];       // Tags générés par l'IA
    char summary[512];       // Résumé généré par l'IA
} jarfs_ai_metadata_t;

// Structure de fichier ouvert
typedef struct jarfs_file {
    u32 inode_num;
    jarfs_inode_t inode;
    u64 position;
    u32 flags;
    u32 refcount;
    struct jarfs_mount* mount;
    struct jarfs_file* next;
    struct jarfs_file* prev;
} jarfs_file_t;

// Structure de montage
typedef struct jarfs_mount {
    jarfs_superblock_t superblock;
    struct block_device* device;
    u64 mount_flags;
    jarfs_file_t* root;
    jarfs_file_t* cwd;
    struct jarfs_mount* next;
    // Cache
    void* inode_cache;
    void* block_cache;
    // Statistiques
    u64 cache_hits;
    u64 cache_misses;
    u64 read_ops;
    u64 write_ops;
} jarfs_mount_t;

// Structure VFS (Virtual File System)
typedef struct vfs_node {
    char name[JARFS_MAX_FILENAME];
    jarfs_file_type_t type;
    u32 permissions;
    u64 size;
    u64 inode;
    struct vfs_node* parent;
    struct vfs_node* children;
    struct vfs_node* next;
    jarfs_mount_t* mount;
} vfs_node_t;

// Initialisation
void jarfs_init(void);
jar_result_t jarfs_format(struct block_device* device, u64 size);

// Montage/Démontage
jarfs_mount_t* jarfs_mount(struct block_device* device, const char* mountpoint, u64 flags);
jar_result_t jarfs_unmount(jarfs_mount_t* mount);
jar_result_t jarfs_sync(jarfs_mount_t* mount);

// Opérations sur les fichiers
jarfs_file_t* jarfs_open(jarfs_mount_t* mount, const char* path, u32 flags);
void jarfs_close(jarfs_file_t* file);
ssize_t jarfs_read(jarfs_file_t* file, void* buf, size_t count);
ssize_t jarfs_write(jarfs_file_t* file, const void* buf, size_t count);
u64 jarfs_seek(jarfs_file_t* file, s64 offset, int whence);
jar_result_t jarfs_truncate(jarfs_file_t* file, u64 size);

// Opérations sur les répertoires
jar_result_t jarfs_mkdir(jarfs_mount_t* mount, const char* path, u32 mode);
jar_result_t jarfs_rmdir(jarfs_mount_t* mount, const char* path);
jar_result_t jarfs_readdir(jarfs_file_t* dir, jarfs_dir_entry_t* entry);
jarfs_file_t* jarfs_opendir(jarfs_mount_t* mount, const char* path);

// Opérations sur les liens
jar_result_t jarfs_symlink(jarfs_mount_t* mount, const char* target, const char* linkpath);
ssize_t jarfs_readlink(jarfs_mount_t* mount, const char* path, char* buf, size_t bufsize);

// Opérations sur les inodes
jar_result_t jarfs_stat(jarfs_mount_t* mount, const char* path, jarfs_inode_t* inode);
jar_result_t jarfs_chmod(jarfs_mount_t* mount, const char* path, u32 mode);
jar_result_t jarfs_chown(jarfs_mount_t* mount, const char* path, u32 uid, u32 gid);
jar_result_t jarfs_unlink(jarfs_mount_t* mount, const char* path);
jar_result_t jarfs_rename(jarfs_mount_t* mount, const char* oldpath, const char* newpath);

// Fonctions IA spécifiques
jar_result_t jarfs_ai_index_file(jarfs_mount_t* mount, const char* path);
jar_result_t jarfs_ai_search_semantic(jarfs_mount_t* mount, const char* query, char** results, u32 max_results);
jar_result_t jarfs_ai_get_related(jarfs_mount_t* mount, const char* path, char** results, u32 max_results);
jar_result_t jarfs_ai_optimize_layout(jarfs_mount_t* mount);
jar_result_t jarfs_ai_generate_summary(jarfs_mount_t* mount, const char* path, char* summary, size_t max_len);

// Cache
void jarfs_flush_cache(jarfs_mount_t* mount);
void jarfs_invalidate_cache(jarfs_mount_t* mount, u64 block);

// Utilitaires
u64 jarfs_get_free_space(jarfs_mount_t* mount);
u64 jarfs_get_total_space(jarfs_mount_t* mount);
void jarfs_dump_stats(jarfs_mount_t* mount);

// VFS
void vfs_init(void);
vfs_node_t* vfs_create_node(const char* name, jarfs_file_type_t type);
jar_result_t vfs_mount(const char* path, jarfs_mount_t* fs);
jar_result_t vfs_unmount(const char* path);
vfs_node_t* vfs_lookup(const char* path);
jar_result_t vfs_read(vfs_node_t* node, void* buf, size_t offset, size_t count);
jar_result_t vfs_write(vfs_node_t* node, const void* buf, size_t offset, size_t count);

#ifdef __cplusplus
}
#endif

#endif // JARJARVIS_JARFS_H
