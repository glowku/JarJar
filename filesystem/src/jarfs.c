#include <kernel/types.h>
#include <kernel/panic.h>
#include <string.h>

// --- CONSTANTES ---
#define JARFS_MAGIC          0x4A415256 // "JARV"
#define JARFS_BLOCK_SIZE     4096
#define JARFS_VERSION_MAJOR  1
#define JARFS_VERSION_MINOR  0
#define JARFS_MAX_FILENAME   256
#define JARFS_MAX_BLOCKS_PER_INODE 12

// --- ENUMS ---
// JAR_OK, JAR_ERROR, etc. sont déjà dans <kernel/types.h>
typedef enum {
    JARFS_TYPE_FILE = 1,
    JARFS_TYPE_DIRECTORY = 2
} jarfs_file_type_t;

// --- STRUCTURES ---
struct block_device {
    char name[32];
    u64 size;
    int (*read)(struct block_device* dev, u64 offset, void* buffer, u32 size);
    int (*write)(struct block_device* dev, u64 offset, const void* buffer, u32 size);
    void* priv;
};

typedef struct {
    u32 mode;
    u32 uid;
    u32 gid;
    u32 size;
    u32 atime, ctime, mtime;
    u32 links_count;
    u32 blocks;
    u32 block[JARFS_MAX_BLOCKS_PER_INODE];
} jarfs_inode_t;

typedef struct {
    u32 magic;
    u32 version_major, version_minor;
    u64 block_count, inode_count;
    u64 free_blocks, free_inodes;
    u64 block_bitmap_start, inode_bitmap_start, inode_table_start, data_start;
    u32 blocks_per_group, inodes_per_group;
    u32 creation_time, last_mount_time, last_write_time;
    u32 mount_count, max_mount_count;
    u32 features;
    u64 checksum;
} jarfs_superblock_t;

typedef struct {
    u32 block_bitmap, inode_bitmap, inode_table;
    u16 free_blocks_count, free_inodes_count, used_dirs_count;
    u16 flags;
    u32 checksum;
} jarfs_group_desc_t;

typedef struct {
    u32 inode;
    u16 rec_len;
    u8 name_len;
    u8 file_type;
    char name[JARFS_MAX_FILENAME];
} jarfs_dir_entry_t;

typedef struct jarfs_mount {
    struct block_device* device;
    jarfs_superblock_t superblock;
    u64 mount_flags;
    struct jarfs_file* root;
    struct jarfs_file* cwd;
    u64 read_ops, write_ops, cache_hits, cache_misses;
    struct jarfs_mount* next;
} jarfs_mount_t;

typedef struct jarfs_file {
    u32 inode_num;
    jarfs_inode_t inode;
    jarfs_mount_t* mount;
    u64 position;
    u32 flags;
    u32 refcount;
} jarfs_file_t;

typedef struct vfs_node {
    char name[JARFS_MAX_FILENAME];
    jarfs_file_type_t type;
    u32 permissions;
} vfs_node_t;

// --- PROTOTYPES (Pour éviter les erreurs "implicit declaration") ---
void vfs_init(void);
vfs_node_t* vfs_create_node(const char* name, jarfs_file_type_t type);
jarfs_file_t* jarfs_open(jarfs_mount_t* mount, const char* path, u32 flags);
void jarfs_close(jarfs_file_t* file);
jar_result_t jarfs_sync(jarfs_mount_t* mount);
u64 jarfs_get_free_space(jarfs_mount_t* mount);
void jarfs_flush_cache(jarfs_mount_t* mount);

extern void console_printf(const char* fmt, ...);
extern void* kmalloc(size_t size);
extern void kfree(void* ptr);

// --- VARIABLES GLOBALES ---
static jarfs_mount_t* mount_list = NULL;
static u32 mount_count = 0;
static vfs_node_t* vfs_root = NULL;

// --- FONCTIONS UTILITAIRES ---
static inline u64 div_round_up(u64 n, u64 d) {
    return (n + d - 1) / d;
}

static u64 calculate_checksum(void* data, size_t size) {
    u64 checksum = 0;
    u8* bytes = (u8*)data;
    for (size_t i = 0; i < size; i++) {
        checksum = (checksum << 1) ^ bytes[i];
    }
    return checksum;
}

// --- CORE FUNCTIONS ---

void jarfs_init(void) {
    console_printf("[JARFS] Initializing JARVIS File System...\n");
    vfs_init();
    console_printf("[JARFS] JARFS version %d.%d ready\n", JARFS_VERSION_MAJOR, JARFS_VERSION_MINOR);
}

jar_result_t jarfs_format(struct block_device* device, u64 size) {
    if (!device || size < 16 * 1024 * 1024) return JAR_INVAL;

    u64 block_count = size / JARFS_BLOCK_SIZE;
    u64 inode_count = block_count / 4;
    if (inode_count < 1024) inode_count = 1024;

    u64 block_bitmap_block = 2;
    u64 inode_bitmap_block = block_bitmap_block + div_round_up(block_count, 8 * JARFS_BLOCK_SIZE);
    u64 inode_table_block = inode_bitmap_block + div_round_up(inode_count, 8 * JARFS_BLOCK_SIZE);
    u64 data_start_block = inode_table_block + div_round_up(inode_count * sizeof(jarfs_inode_t), JARFS_BLOCK_SIZE);

    jarfs_superblock_t sb = {0};
    sb.magic = JARFS_MAGIC;
    sb.version_major = JARFS_VERSION_MAJOR;
    sb.version_minor = JARFS_VERSION_MINOR;
    sb.block_count = block_count;
    sb.inode_count = inode_count;
    sb.free_blocks = block_count - data_start_block - 1;
    sb.free_inodes = inode_count - 10;
    sb.block_bitmap_start = block_bitmap_block;
    sb.inode_bitmap_start = inode_bitmap_block;
    sb.inode_table_start = inode_table_block;
    sb.data_start = data_start_block;
    sb.checksum = calculate_checksum(&sb, sizeof(sb) - sizeof(u64));

    device->write(device, 0, &sb, sizeof(sb));

    // Créer l'inode racine
    jarfs_inode_t root_inode = {0};
    root_inode.mode = 0755 | (JARFS_TYPE_DIRECTORY << 12);
    root_inode.size = JARFS_BLOCK_SIZE;
    root_inode.blocks = 1;
    root_inode.block[0] = data_start_block;

    device->write(device, inode_table_block * JARFS_BLOCK_SIZE, &root_inode, sizeof(root_inode));

    return JAR_OK;
}

jarfs_mount_t* jarfs_mount(struct block_device* device, const char* mountpoint, u64 flags) {
    if (!device) return NULL;
    jarfs_mount_t* mount = kmalloc(sizeof(jarfs_mount_t));
    if (!mount) return NULL;

    memset(mount, 0, sizeof(jarfs_mount_t));
    if (device->read(device, 0, &mount->superblock, sizeof(jarfs_superblock_t)) != sizeof(jarfs_superblock_t)) {
        kfree(mount);
        return NULL;
    }

    if (mount->superblock.magic != JARFS_MAGIC) {
        kfree(mount);
        return NULL;
    }

    mount->device = device;
    mount->root = jarfs_open(mount, "/", 0);
    mount->cwd = mount->root;
    mount->next = mount_list;
    mount_list = mount;
    mount_count++;

    console_printf("[JARFS] Mounted at %s, %lu MB free\n", mountpoint, jarfs_get_free_space(mount) / (1024 * 1024));
    return mount;
}

jar_result_t jarfs_unmount(jarfs_mount_t* mount) {
    if (!mount) return JAR_INVAL;
    jarfs_sync(mount);
    if (mount->root) jarfs_close(mount->root);
    kfree(mount);
    return JAR_OK;
}

jar_result_t jarfs_sync(jarfs_mount_t* mount) {
    if (!mount) return JAR_INVAL;
    mount->superblock.checksum = calculate_checksum(&mount->superblock, sizeof(jarfs_superblock_t) - sizeof(u64));
    mount->device->write(mount->device, 0, &mount->superblock, sizeof(jarfs_superblock_t));
    jarfs_flush_cache(mount);
    return JAR_OK;
}

jarfs_file_t* jarfs_open(jarfs_mount_t* mount, const char* path, u32 flags) {
    (void)path; // Pour l'instant on ignore le path et on ouvre la racine
    jarfs_file_t* file = kmalloc(sizeof(jarfs_file_t));
    if (!file) return NULL;

    file->inode_num = 1;
    file->mount = mount;
    file->flags = flags;
    file->refcount = 1;

    u64 offset = mount->superblock.inode_table_start * JARFS_BLOCK_SIZE;
    mount->device->read(mount->device, offset, &file->inode, sizeof(jarfs_inode_t));
    return file;
}

void jarfs_close(jarfs_file_t* file) {
    if (!file) return;
    if (--file->refcount == 0) {
        kfree(file);
    }
}

ssize_t jarfs_read(jarfs_file_t* file, void* buf, size_t count) {
    if (!file || !buf) return -1;
    if (file->position + count > file->inode.size) count = file->inode.size - file->position;
    if (count == 0) return 0;

    u64 block = file->inode.block[0];
    u8* temp = kmalloc(JARFS_BLOCK_SIZE);
    file->mount->device->read(file->mount->device, block * JARFS_BLOCK_SIZE, temp, JARFS_BLOCK_SIZE);
    memcpy(buf, temp + (file->position % JARFS_BLOCK_SIZE), count);
    kfree(temp);

    file->position += count;
    return count;
}

ssize_t jarfs_write(jarfs_file_t* file, const void* buf, size_t count) {
    (void)file; (void)buf;
    return count; // Stub
}

u64 jarfs_get_free_space(jarfs_mount_t* mount) {
    return mount ? mount->superblock.free_blocks * JARFS_BLOCK_SIZE : 0;
}

void jarfs_flush_cache(jarfs_mount_t* mount) { (void)mount; }

// --- VFS ---

void vfs_init(void) {
    vfs_root = vfs_create_node("/", JARFS_TYPE_DIRECTORY);
    if (!vfs_root) panic("VFS root failed");
}

vfs_node_t* vfs_create_node(const char* name, jarfs_file_type_t type) {
    vfs_node_t* node = kmalloc(sizeof(vfs_node_t));
    if (!node) return NULL;
    memset(node, 0, sizeof(vfs_node_t));
    strncpy(node->name, name, JARFS_MAX_FILENAME - 1);
    node->type = type;
    return node;
}

jar_result_t vfs_read(vfs_node_t* node, void* buf, size_t offset, size_t count) {
    (void)node; (void)buf; (void)offset; (void)count;
    return JAR_OK;
}

jar_result_t vfs_write(vfs_node_t* node, const void* buf, size_t offset, size_t count) {
    (void)node; (void)buf; (void)offset; (void)count;
    return JAR_OK;
}

// --- AI STUBS ---
jar_result_t jarfs_ai_search_semantic(jarfs_mount_t* mount, const char* query, char** results, u32 max_results) {
    (void)mount; (void)query; (void)results; (void)max_results;
    return JAR_OK;
}

jar_result_t jarfs_ai_get_related(jarfs_mount_t* mount, const char* path, char** results, u32 max_results) {
    (void)mount; (void)path; (void)results; (void)max_results;
    return JAR_OK;
}