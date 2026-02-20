#!/bin/bash
# Configuration des chemins
export PATH=$PATH:/opt/cross/bin:/usr/local/bin:/usr/bin:/bin
WORKING_DIR="/c/Users/slash/Downloads/VM/p/jarjarvis"
cd "$WORKING_DIR"

REPORT="diag.json"
echo "{" > $REPORT
echo "  \"timestamp\": \"$(date)\"," >> $REPORT

echo "===================================================="
echo "🕵️ MODE BOÎTE NOIRE : DIAGNOSTIC INTENSIF"
echo "===================================================="

# 1. Vérification des sources
echo "  \"files_check\": {" >> $REPORT
for dir in kernel libc drivers ai_core gui filesystem; do
    count=$(find $dir -name "*.c" | wc -l)
    echo "    \"$dir\": $count," >> $REPORT
done
echo "    \"linker\": $([ -f kernel/linker.ld ] && echo true || echo false)" >> $REPORT
echo "  }," >> $REPORT

# 2. Nettoyage et Compil
rm -rf *.o *.elf iso_root 2>/dev/null
mkdir -p iso_root

echo "🔨 Compilation..."
COMPILE_ERRORS=0
for f in $(find kernel libc drivers ai_core gui filesystem -name "*.c" | grep -vE "CMakeCCompilerId.c|dummy.c"); do
    obj_name=$(echo "$f" | sed 's/\//_/g' | sed 's/\.c$/.o/')
    x86_64-elf-gcc -c "$f" -o "$obj_name" -ffreestanding -O2 -m64 -mcmodel=kernel \
        -fno-stack-protector -fno-pie -fno-builtin -nostdlib \
        -I. -I./kernel/include -I./libc/include -I./ai_core/include -I./gui/include -I./filesystem/include 2>> compile_log.txt
    if [ $? -ne 0 ]; then ((COMPILE_ERRORS++)); fi
done
echo "  \"compile_errors\": $COMPILE_ERRORS," >> $REPORT

# 3. Linkage
echo "🔗 Linkage..."
x86_64-elf-ld -T kernel/linker.ld -o jarvis.elf --static -z max-page-size=0x1000 *.o 2>> link_log.txt
LINK_RES=$?
echo "  \"link_success\": $([ $LINK_RES -eq 0 ] && echo true || echo false)," >> $REPORT

# 4. Config Stricte (Unix format)
# On utilise printf pour éviter les résidus Windows qui causent le "Configuration is INVALID"
printf "TIMEOUT=5\nVERBOSE=yes\n\n:JarJarvis\n    PROTOCOL=limine\n    KERNEL_PATH=boot:///jarvis.elf\n" > iso_root/limine.conf

# 5. Préparation ISO
cp jarvis.elf iso_root/
cp limine/limine-bios.sys iso_root/
cp limine/limine-bios-cd.bin iso_root/

# 6. XORRISO
echo "💿 ISO Generation..."
xorriso -as mkisofs -R -J -V "JARVIS" -b limine-bios-cd.bin \
    -no-emul-boot -boot-load-size 4 -boot-info-table iso_root -o jarvis.iso 2> xorriso_diag.txt
echo "  \"iso_size\": $(stat -c%s jarvis.iso 2>/dev/null || echo 0)," >> $REPORT

# 7. Limine Install
./limine/limine.exe bios-install jarvis.iso >> install_log.txt 2>&1
echo "  \"limine_install\": $?" >> $REPORT
echo "}" >> $REPORT

echo "===================================================="
echo "🚀 LANCEMENT QEMU + LOGS LOGICIELS"
echo "===================================================="

# -d int,cpu_reset : Log les interruptions et les resets CPU (écran noir = souvent triple fault)
qemu-system-x86_64 -cdrom jarvis.iso \
    -serial stdio \
    -m 512M \
    -no-reboot \
    -d guest_errors,cpu_reset,int \
    -D qemu_full_debug.log