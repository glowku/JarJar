# Building JARJARVIS on Windows

This guide explains how to build JARJARVIS on Windows 10/11 and run it in a VM.

## ⚠️ Important Note

**MSVC (Visual Studio Compiler) CANNOT build JARJARVIS!** 

You MUST use one of these:
- **MinGW-w64** (easiest on Windows)
- **x86_64-elf-gcc cross-compiler** (most compatible)
- **WSL2** (recommended for serious development)

## Quick Start (Easiest Method)

### Option 1: Using PowerShell Script (Recommended)

```powershell
# 1. Open PowerShell as Administrator
# 2. Navigate to jarjarvis directory
cd C:\Users\slash\Downloads\VM\jarjarvis

# 3. Install dependencies (one-time)
.\build-windows.ps1 -InstallDeps

# 4. Restart PowerShell, then build
.\build-windows.ps1 all

# 5. Run in QEMU
.\build-windows.ps1 run
```

### Option 2: Manual Installation

#### Step 1: Install Prerequisites

**Using Winget (Windows 10/11):**
```powershell
# Install NASM
winget install -e --id NASM.NASM

# Install MinGW-w64
winget install -e --id Mingw-w64.Mingw-w64

# Install CMake
winget install -e --id Kitware.CMake

# Install QEMU (optional, for testing)
winget install -e --id SoftwareFreedomConservancy.QEMU

# Install Git (if not already installed)
winget install -e --id Git.Git
```

**Using Chocolatey:**
```powershell
# Run as Administrator
choco install nasm mingw cmake qemu git -y
```

**Manual Download:**
- NASM: https://www.nasm.us/pub/nasm/releasebuilds/2.16.03/win64/
- MinGW-w64: https://github.com/brechtswin/winlibs_mingw/releases/
- CMake: https://cmake.org/download/
- QEMU: https://www.qemu.org/download/#windows

#### Step 2: Add to PATH

After installation, add these to your system PATH:
```
C:\Program Files\NASM
C:\mingw64\bin
C:\Program Files\CMake\bin
C:\Program Files\qemu
```

**Verify installations:**
```powershell
nasm -v
gcc --version
cmake --version
qemu-system-x86_64 --version
```

#### Step 3: Build JARJARVIS

```powershell
# Navigate to project
cd C:\Users\slash\Downloads\VM\jarjarvis

# Create build directory
mkdir build
cd build

# Configure with CMake (using MinGW)
cmake .. -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release

# Build
mingw32-make -j4

# Or use cmake
cmake --build . --parallel
```

#### Step 4: Run in QEMU

```powershell
# Run kernel directly
qemu-system-x86_64 -kernel jarjarvis.kernel -m 4G -smp 4 -vga std -serial stdio

# Or if ISO was created
qemu-system-x86_64 -cdrom jarjarvis.iso -m 4G -smp 4 -vga std -serial stdio
```

## Alternative: Using MSYS2

MSYS2 provides a Linux-like environment on Windows.

### Install MSYS2

1. Download from https://www.msys2.org/
2. Install and open MSYS2 terminal
3. Update packages:
```bash
pacman -Syu
```

### Install Toolchain

```bash
# Install MinGW-w64 toolchain
pacman -S mingw-w64-x86_64-gcc mingw-w64-x86_64-gdb mingw-w64-x86_64-make

# Install other tools
pacman -S nasm cmake git

# Install QEMU
pacman -S mingw-w64-x86_64-qemu
```

### Build in MSYS2

```bash
cd /c/Users/slash/Downloads/VM/jarjarvis

mkdir build && cd build
cmake .. -G "MinGW Makefiles"
mingw32-make -j4
```

## Alternative: Using WSL2 (Most Compatible)

WSL2 provides a full Linux environment and is the **recommended** method.

### Install WSL2

```powershell
# In PowerShell (Administrator)
wsl --install -d Ubuntu

# Restart computer when prompted
```

### Setup Build Environment

```bash
# In WSL2 Ubuntu
sudo apt-get update
sudo apt-get install -y build-essential nasm xorriso qemu-system-x86

# Install cross-compiler
sudo apt-get install -y gcc-x86-64-linux-gnu
# OR build from source (see BUILD.md)
```

### Build in WSL2

```bash
# Access Windows files
ls /mnt/c/Users/slash/Downloads/VM/jarjarvis

# Build
cd /mnt/c/Users/slash/Downloads/VM/jarjarvis
./build.sh all

# Run in QEMU
./build.sh run
```

## Alternative: Using Docker

Create `Dockerfile.windows`:

```dockerfile
FROM ubuntu:22.04

RUN apt-get update && apt-get install -y \
    build-essential \
    nasm \
    xorriso \
    grub-pc-bin \
    grub-common \
    cmake \
    git

# Install cross-compiler
RUN apt-get install -y gcc-x86-64-linux-gnu || \
    (apt-get install -y wget && \
     wget https://github.com/jarjarvis/cross-compiler/releases/download/v1.0/x86_64-elf-gcc.tar.gz && \
     tar xzf x86_64-elf-gcc.tar.gz -C /opt)

WORKDIR /jarjarvis
COPY . .

CMD ["./build.sh", "all"]
```

Build and run:
```powershell
# Build Docker image
docker build -f Dockerfile.windows -t jarjarvis-builder .

# Run build
docker run -v ${PWD}:/jarjarvis jarjarvis-builder

# Output will be in build/ directory
```

## Troubleshooting

### "No CMAKE_ASM_NASM_COMPILER could be found"

**Solution:** Install NASM and add to PATH
```powershell
winget install -e --id NASM.NASM
# Restart PowerShell
```

### "No CMAKE_C_COMPILER could be found"

**Solution:** Install MinGW-w64
```powershell
winget install -e --id Mingw-w64.Mingw-w64
# Or download from: https://winlibs.com/
```

### "The C compiler identification is MSVC"

**Problem:** CMake is using Visual Studio compiler instead of MinGW

**Solution:** Force MinGW generator
```powershell
cmake .. -G "MinGW Makefiles" -DCMAKE_C_COMPILER=gcc -DCMAKE_CXX_COMPILER=g++
```

### "undefined reference to..." errors

**Problem:** Linker issues with MinGW

**Solution:** Use x86_64-elf-gcc cross-compiler instead of MinGW
```powershell
# Download pre-built cross-compiler
# https://github.com/lordmilko/i686-elf-tools/releases
# Extract to C:\cross-compiler

$env:PATH = "C:\cross-compiler\bin;$env:PATH"
cmake .. -DCMAKE_C_COMPILER=x86_64-elf-gcc -DCMAKE_CXX_COMPILER=x86_64-elf-g++
```

### "grub-mkrescue not found"

**Problem:** GRUB tools not available on Windows

**Solution:** 
1. Use WSL2 for ISO creation
2. Or use the kernel directly with QEMU: `qemu-system-x86_64 -kernel jarjarvis.kernel`
3. Or use a Linux VM to create the ISO

### QEMU won't start

```powershell
# Check QEMU installation
qemu-system-x86_64 --version

# Try with minimal options
qemu-system-x86_64 -kernel jarjarvis.kernel -m 2G

# Enable KVM on Linux (not Windows)
qemu-system-x86_64 -kernel jarjarvis.kernel -m 4G -enable-kvm
```

## Running in VM

After building, you have several options:

### Option 1: QEMU (Recommended for testing)
```powershell
qemu-system-x86_64 -kernel jarjarvis.kernel -m 4G -smp 4 -vga std -serial stdio
```

### Option 2: VirtualBox
1. Create new VM (Type: Other, Version: Other/Unknown 64-bit)
2. Mount `jarjarvis.iso` if created, or use kernel directly
3. Start VM

### Option 3: VMware
1. Create new VM (Guest OS: Other, Version: Other 64-bit)
2. Use ISO or kernel file
3. Start VM

### Option 4: Hyper-V (Windows Pro/Enterprise)
1. Enable Hyper-V in Windows Features
2. Create Generation 2 VM
3. Attach ISO or use kernel

## Build Outputs

After successful build:
```
build/
├── jarjarvis.kernel       # Kernel binary (bootable)
├── jarjarvis.elf          # Kernel ELF (for debugging)
├── jarjarvis.iso          # Bootable ISO (if GRUB available)
└── [object files...]
```

## Development Workflow

```powershell
# 1. Make changes to source code

# 2. Build
.\build-windows.ps1 build

# 3. Test in QEMU
.\build-windows.ps1 run

# 4. Debug (if needed)
.\build-windows.ps1 gdb
# In another terminal, connect GDB
```

## Recommended Setup

For the best Windows development experience:

1. **Install VS Code** with extensions:
   - C/C++
   - CMake Tools
   - x86 and x86_64 Assembly

2. **Use WSL2** for full compatibility

3. **Keep PowerShell open** for quick builds

4. **Use QEMU** for rapid testing

## Getting Help

- GitHub Issues: https://github.com/jarjarvis/os/issues
- Discord: https://discord.gg/jarjarvis
- Documentation: https://jarjarvis.org/docs

---

**Note:** Windows builds may have limitations compared to Linux builds. For production use or serious development, WSL2 or a Linux VM is strongly recommended.
