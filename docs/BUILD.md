# Building JARJARVIS

This guide explains how to build JARJARVIS from source.

## Prerequisites

### Required Tools

- **Cross-compiler**: `x86_64-elf-gcc` and `x86_64-elf-ld`
- **Assembler**: `nasm`
- **GRUB**: `grub-mkrescue` for ISO creation
- **Make**: GNU Make 4.0+
- **CMake**: 3.20+ (optional, for CMake build)

### Optional Tools

- **QEMU**: For testing (`qemu-system-x86_64`)
- **GDB**: For debugging (`x86_64-elf-gdb`)
- **Xorriso**: For ISO creation

## Installing Prerequisites

### Ubuntu/Debian

```bash
# Install build tools
sudo apt-get update
sudo apt-get install build-essential nasm xorriso qemu-system-x86

# Install cross-compiler (using pre-built)
sudo apt-get install gcc-x86-64-linux-gnu

# Or build from source (see below)
```

### macOS

```bash
# Install Homebrew if not already installed
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"

# Install tools
brew install nasm qemu xorriso

# Install cross-compiler
brew install x86_64-elf-gcc
```

### Arch Linux

```bash
sudo pacman -S base-devel nasm qemu xorriso

# Install cross-compiler from AUR
yay -S x86_64-elf-gcc
```

## Building the Cross-Compiler

If pre-built packages are not available:

```bash
# Download binutils and gcc
wget https://ftp.gnu.org/gnu/binutils/binutils-2.40.tar.xz
wget https://ftp.gnu.org/gnu/gcc/gcc-12.2.0/gcc-12.2.0.tar.xz

# Extract
tar xf binutils-2.40.tar.xz
tar xf gcc-12.2.0.tar.xz

# Set up build directory
mkdir -p /opt/cross
export PREFIX=/opt/cross
export TARGET=x86_64-elf
export PATH=$PREFIX/bin:$PATH

# Build binutils
mkdir build-binutils
cd build-binutils
../binutils-2.40/configure --target=$TARGET --prefix=$PREFIX --disable-nls --disable-werror
make -j$(nproc)
sudo make install
cd ..

# Build GCC
mkdir build-gcc
cd build-gcc
../gcc-12.2.0/configure --target=$TARGET --prefix=$PREFIX --disable-nls --enable-languages=c,c++ --without-headers
make -j$(nproc) all-gcc
make -j$(nproc) all-target-libgcc
sudo make install-gcc
sudo make install-target-libgcc
cd ..

# Add to PATH
echo 'export PATH=/opt/cross/bin:$PATH' >> ~/.bashrc
source ~/.bashrc
```

## Building JARJARVIS

### Using Make (Recommended)

```bash
# Clone the repository
git clone https://github.com/jarjarvis/os.git
cd jarjarvis

# Full build
make all

# Or use the build script
./build.sh all
```

### Using CMake

```bash
# Create build directory
mkdir build && cd build

# Configure
cmake ..

# Build
make -j$(nproc)

# Create ISO
make iso
```

### Build Stages

The build process consists of several stages:

1. **Kernel Assembly** - Compile boot code
2. **Kernel C** - Compile kernel source
3. **libc** - Build standard C library
4. **Filesystem** - Build JARFS
5. **Drivers** - Build device drivers
6. **AI Core** - Build JARVIS AI
7. **GUI** - Build graphical interface
8. **Link** - Link everything into kernel binary
9. **ISO** - Create bootable ISO

## Build Options

### Debug Build

```bash
make clean
make CFLAGS="-g -O0 -DDEBUG" all
```

### Release Build

```bash
make clean
make CFLAGS="-O3 -DNDEBUG" all
```

### Minimal Build (no GUI/AI)

```bash
make KERNEL_ONLY=1 all
```

## Build Outputs

After successful build:

```
build/
├── jarjarvis.kernel    # Kernel binary
├── jarjarvis.iso       # Bootable ISO
├── kernel/             # Kernel objects
├── libc/               # libc objects
├── fs/                 # Filesystem objects
├── drivers/            # Driver objects
├── ai/                 # AI core objects
└── gui/                # GUI objects
```

## Testing

### Run in QEMU

```bash
# Using Make
make run

# Using build script
./build.sh run

# Manual
qemu-system-x86_64 -cdrom build/jarjarvis.iso -m 4G -smp 4
```

### Debug with GDB

```bash
# Terminal 1: Start QEMU in debug mode
make debug
# or
./build.sh debug

# Terminal 2: Connect GDB
target remote localhost:1234
symbol-file build/jarjarvis.kernel
break kernel_main
continue
```

### Run Tests

```bash
make test
```

## Troubleshooting

### Cross-compiler not found

```bash
# Check if it's in PATH
which x86_64-elf-gcc

# If not, add to PATH
export PATH=/opt/cross/bin:$PATH
```

### GRUB not found

```bash
# Ubuntu/Debian
sudo apt-get install grub-pc-bin grub-common

# The ISO can still be created without grub-mkrescue
# Use the kernel binary directly with QEMU
qemu-system-x86_64 -kernel build/jarjarvis.kernel
```

### Build fails with "undefined reference"

```bash
# Clean and rebuild
make clean
make all
```

### QEMU performance issues

```bash
# Enable KVM acceleration (Linux)
qemu-system-x86_64 -cdrom build/jarjarvis.iso -m 4G -smp 4 -enable-kvm

# Or reduce memory and cores
qemu-system-x86_64 -cdrom build/jarjarvis.iso -m 2G -smp 2
```

## Advanced Build Options

### Custom Linker Script

```bash
make LDFLAGS="-T custom.ld"
```

### Static Analysis

```bash
# With clang-static-analyzer
scan-build make

# With cppcheck
cppcheck --enable=all kernel/
```

### Code Coverage

```bash
make CFLAGS="--coverage" all
# Run tests
# Generate report
lcov --capture --directory . --output-file coverage.info
genhtml coverage.info --output-directory coverage-report
```

## Continuous Integration

Example GitHub Actions workflow:

```yaml
name: Build JARJARVIS

on: [push, pull_request]

jobs:
  build:
    runs-on: ubuntu-latest
    
    steps:
    - uses: actions/checkout@v2
    
    - name: Install dependencies
      run: |
        sudo apt-get update
        sudo apt-get install -y nasm xorriso qemu-system-x86
    
    - name: Install cross-compiler
      run: |
        wget https://github.com/jarjarvis/cross-compiler/releases/download/v1.0/x86_64-elf-gcc.tar.gz
        tar xzf x86_64-elf-gcc.tar.gz -C /opt
        echo "/opt/cross/bin" >> $GITHUB_PATH
    
    - name: Build
      run: make all
    
    - name: Test
      run: make test
    
    - name: Upload artifact
      uses: actions/upload-artifact@v2
      with:
        name: jarjarvis-iso
        path: build/jarjarvis.iso
```

## Getting Help

- Build issues: Open an issue on GitHub
- Questions: Join our Discord server
- Documentation: https://jarjarvis.org/docs/build

---

Happy building!
