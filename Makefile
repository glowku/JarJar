# JARJARVIS Operating System Makefile
# Complete build system for JARJARVIS OS

#==============================================================================
# Configuration
#==============================================================================

# Target architecture
ARCH := x86_64
TARGET := $(ARCH)-elf

# Directories
BUILD_DIR := build
ISO_DIR := $(BUILD_DIR)/iso
KERNEL_DIR := kernel
LIBC_DIR := libc
FS_DIR := filesystem
DRIVERS_DIR := drivers
AI_DIR := ai_core
GUI_DIR := gui

# Toolchain
CC := $(TARGET)-gcc
CXX := $(TARGET)-g++
AS := nasm
LD := $(TARGET)-ld
AR := $(TARGET)-ar
OBJCOPY := $(TARGET)-objcopy
OBJDUMP := $(TARGET)-objdump

# Flags
CFLAGS := -ffreestanding -O2 -Wall -Wextra -nostdlib -nostartfiles
CFLAGS += -fno-exceptions -fno-rtti -fno-stack-protector
CFLAGS += -mno-red-zone -mno-mmx -mno-sse -mno-sse2
CFLAGS += -mcmodel=large -m64
CFLAGS += -I$(KERNEL_DIR)/include -I$(LIBC_DIR)/include
CFLAGS += -I$(FS_DIR)/include -I$(DRIVERS_DIR)/include
CFLAGS += -I$(AI_DIR)/include -I$(GUI_DIR)/include
CFLAGS += -D__JARJARVIS__ -D$(ARCH)

CXXFLAGS := $(CFLAGS) -std=c++17

ASFLAGS := -f elf64 -F dwarf

LDFLAGS := -T $(KERNEL_DIR)/linker.ld
LDFLAGS += -nostdlib -static
LDFLAGS += -z max-page-size=0x1000

#==============================================================================
# Source Files
#==============================================================================

# Kernel sources
KERNEL_ASM := $(wildcard $(KERNEL_DIR)/arch/$(ARCH)/*.asm)
KERNEL_C := $(wildcard $(KERNEL_DIR)/src/*.c)
KERNEL_OBJ := $(patsubst $(KERNEL_DIR)/arch/$(ARCH)/%.asm,$(BUILD_DIR)/kernel/%.o,$(KERNEL_ASM))
KERNEL_OBJ += $(patsubst $(KERNEL_DIR)/src/%.c,$(BUILD_DIR)/kernel/%.o,$(KERNEL_C))

# Libc sources
LIBC_C := $(wildcard $(LIBC_DIR)/src/*.c)
LIBC_OBJ := $(patsubst $(LIBC_DIR)/src/%.c,$(BUILD_DIR)/libc/%.o,$(LIBC_C))

# Filesystem sources
FS_C := $(wildcard $(FS_DIR)/src/*.c)
FS_OBJ := $(patsubst $(FS_DIR)/src/%.c,$(BUILD_DIR)/fs/%.o,$(FS_C))

# Driver sources
DRIVERS_C := $(wildcard $(DRIVERS_DIR)/src/*.c)
DRIVERS_OBJ := $(patsubst $(DRIVERS_DIR)/src/%.c,$(BUILD_DIR)/drivers/%.o,$(DRIVERS_C))

# AI Core sources
AI_C := $(wildcard $(AI_DIR)/src/*.c)
AI_OBJ := $(patsubst $(AI_DIR)/src/%.c,$(BUILD_DIR)/ai/%.o,$(AI_C))

# GUI sources
GUI_C := $(wildcard $(GUI_DIR)/src/*.c)
GUI_OBJ := $(patsubst $(GUI_DIR)/src/%.c,$(BUILD_DIR)/gui/%.o,$(GUI_C))

# All objects
ALL_OBJ := $(KERNEL_OBJ) $(LIBC_OBJ) $(FS_OBJ) $(DRIVERS_OBJ) $(AI_OBJ) $(GUI_OBJ)

#==============================================================================
# Targets
#==============================================================================

.PHONY: all clean iso run debug install

# Default target
all: $(BUILD_DIR)/jarjarvis.kernel

# Create build directories
$(BUILD_DIR):
	@mkdir -p $(BUILD_DIR)/kernel
	@mkdir -p $(BUILD_DIR)/libc
	@mkdir -p $(BUILD_DIR)/fs
	@mkdir -p $(BUILD_DIR)/drivers
	@mkdir -p $(BUILD_DIR)/ai
	@mkdir -p $(BUILD_DIR)/gui
	@mkdir -p $(ISO_DIR)/boot/grub

# Compile assembly files
$(BUILD_DIR)/kernel/%.o: $(KERNEL_DIR)/arch/$(ARCH)/%.asm | $(BUILD_DIR)
	@echo "AS $<"
	@$(AS) $(ASFLAGS) -o $@ $<

# Compile C files (kernel)
$(BUILD_DIR)/kernel/%.o: $(KERNEL_DIR)/src/%.c | $(BUILD_DIR)
	@echo "CC $<"
	@$(CC) $(CFLAGS) -c -o $@ $<

# Compile C files (libc)
$(BUILD_DIR)/libc/%.o: $(LIBC_DIR)/src/%.c | $(BUILD_DIR)
	@echo "CC $<"
	@$(CC) $(CFLAGS) -c -o $@ $<

# Compile C files (filesystem)
$(BUILD_DIR)/fs/%.o: $(FS_DIR)/src/%.c | $(BUILD_DIR)
	@echo "CC $<"
	@$(CC) $(CFLAGS) -c -o $@ $<

# Compile C files (drivers)
$(BUILD_DIR)/drivers/%.o: $(DRIVERS_DIR)/src/%.c | $(BUILD_DIR)
	@echo "CC $<"
	@$(CC) $(CFLAGS) -c -o $@ $<

# Compile C files (AI)
$(BUILD_DIR)/ai/%.o: $(AI_DIR)/src/%.c | $(BUILD_DIR)
	@echo "CC $<"
	@$(CC) $(CFLAGS) -c -o $@ $<

# Compile C files (GUI)
$(BUILD_DIR)/gui/%.o: $(GUI_DIR)/src/%.c | $(BUILD_DIR)
	@echo "CC $<"
	@$(CC) $(CFLAGS) -c -o $@ $<

# Link kernel
$(BUILD_DIR)/jarjarvis.kernel: $(ALL_OBJ) $(KERNEL_DIR)/linker.ld
	@echo "LD $@"
	@$(LD) $(LDFLAGS) -o $@ $(ALL_OBJ)
	@echo "Kernel built successfully!"
	@echo "Size: $$(stat -f%z $@ 2>/dev/null || stat -c%s $@) bytes"

# Create ISO
iso: $(BUILD_DIR)/jarjarvis.iso

$(BUILD_DIR)/jarjarvis.iso: $(BUILD_DIR)/jarjarvis.kernel boot/grub.cfg
	@echo "Creating ISO image..."
	@cp $(BUILD_DIR)/jarjarvis.kernel $(ISO_DIR)/boot/
	@cp boot/grub.cfg $(ISO_DIR)/boot/grub/
	@grub-mkrescue -o $@ $(ISO_DIR) 2>/dev/null || \
		echo "Warning: grub-mkrescue not found, ISO not created"
	@echo "ISO created: $@"

# Run in QEMU
run: $(BUILD_DIR)/jarjarvis.iso
	@echo "Starting JARJARVIS in QEMU..."
	@qemu-system-$(ARCH) \
		-cdrom $(BUILD_DIR)/jarjarvis.iso \
		-m 4G \
		-smp 4 \
		-vga std \
		-netdev user,id=net0 \
		-device e1000,netdev=net0 \
		-boot d \
		-serial stdio

# Debug mode
debug: $(BUILD_DIR)/jarjarvis.iso
	@echo "Starting JARJARVIS in debug mode..."
	@qemu-system-$(ARCH) \
		-cdrom $(BUILD_DIR)/jarjarvis.iso \
		-m 4G \
		-smp 4 \
		-vga std \
		-s -S \
		-serial stdio &
	@echo "Waiting for GDB connection on port 1234..."

# Run tests
test: all
	@echo "Running tests..."
	@$(MAKE) -C tests

# Clean build files
clean:
	@echo "Cleaning build files..."
	@rm -rf $(BUILD_DIR)
	@echo "Clean complete!"

# Install (copy to USB or disk)
install: $(BUILD_DIR)/jarjarvis.iso
	@echo "Installing JARJARVIS..."
	@echo "To install to USB drive, use:"
	@echo "  dd if=$(BUILD_DIR)/jarjarvis.iso of=/dev/sdX bs=4M status=progress"
	@echo "  (replace /dev/sdX with your USB device)"

# Generate documentation
docs:
	@echo "Generating documentation..."
	@doxygen Doxyfile 2>/dev/null || echo "Doxygen not installed"

# Show help
help:
	@echo "JARJARVIS Operating System Build System"
	@echo ""
	@echo "Available targets:"
	@echo "  all       - Build the kernel (default)"
	@echo "  iso       - Create bootable ISO image"
	@echo "  run       - Run in QEMU emulator"
	@echo "  debug     - Run in debug mode with GDB"
	@echo "  test      - Run test suite"
	@echo "  clean     - Remove build files"
	@echo "  install   - Show installation instructions"
	@echo "  docs      - Generate documentation"
	@echo "  help      - Show this help message"
	@echo ""
	@echo "Configuration:"
	@echo "  ARCH      = $(ARCH)"
	@echo "  CC        = $(CC)"
	@echo "  CFLAGS    = $(CFLAGS)"

# Print build info
info:
	@echo "JARJARVIS Build Information"
	@echo "==========================="
	@echo "Architecture: $(ARCH)"
	@echo "Target: $(TARGET)"
	@echo "Compiler: $(CC)"
	@echo ""
	@echo "Source files:"
	@echo "  Kernel:   $$(echo $(KERNEL_C) $(KERNEL_ASM) | wc -w) files"
	@echo "  Libc:     $$(echo $(LIBC_C) | wc -w) files"
	@echo "  FS:       $$(echo $(FS_C) | wc -w) files"
	@echo "  Drivers:  $$(echo $(DRIVERS_C) | wc -w) files"
	@echo "  AI Core:  $$(echo $(AI_C) | wc -w) files"
	@echo "  GUI:      $$(echo $(GUI_C) | wc -w) files"
