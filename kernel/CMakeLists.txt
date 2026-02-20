# On définit les sources
set(KERNEL_SOURCES
    src/main.c
    src/console.c
    src/pmm.c
    src/vmm.c
    src/stubs.c
)

# On crée une bibliothèque statique nommée 'kernel'
add_library(kernel STATIC ${KERNEL_SOURCES})

# On s'assure que les headers sont trouvés
target_include_directories(kernel PUBLIC include)