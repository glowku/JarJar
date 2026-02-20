# JARJARVIS on Windows - Quick Setup

## ⚠️ IMPORTANT: Read This First!

**You CANNOT use Visual Studio/MSVC to build JARJARVIS!**

JARJARVIS is a **kernel operating system** that requires:
- A **cross-compiler** (x86_64-elf-gcc) or **MinGW-w64**
- **NASM** assembler
- **CMake** build system

## 🚀 Quick Start (5 minutes)

### Step 1: Install Prerequisites

Open **PowerShell as Administrator** and run:

```powershell
# Install NASM
winget install -e --id NASM.NASM

# Install MinGW-w64
winget install -e --id Mingw-w64.Mingw-w64

# Install CMake
winget install -e --id Kitware.CMake

# Install QEMU (for testing)
winget install -e --id SoftwareFreedomConservancy.QEMU
```

**Restart PowerShell** after installation!

### Step 2: Verify Installation

```powershell
nasm -v
gcc --version
cmake --version
qemu-system-x86_64 --version
```

### Step 3: Build JARJARVIS

```powershell
cd C:\Users\slash\Downloads\VM\jarjarvis

# Clean previous attempts
Remove-Item -Recurse -Force build -ErrorAction SilentlyContinue

# Create build directory
mkdir build
cd build

# Configure (force MinGW, not MSVC!)
cmake .. -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release

# Build
mingw32-make -j4
```

### Step 4: Run in QEMU

```powershell
# Run the kernel directly
qemu-system-x86_64 -kernel jarjarvis.kernel -m 4G -smp 4 -vga std -serial stdio
```

## 🔧 Troubleshooting

### "No CMAKE_ASM_NASM_COMPILER could be found"

NASM is not in PATH. Fix:
```powershell
$env:PATH = "C:\Program Files\NASM;$env:PATH"
# Or restart PowerShell
```

### "The C compiler identification is MSVC"

CMake is using Visual Studio instead of MinGW. Force it:
```powershell
cmake .. -G "MinGW Makefiles" -DCMAKE_C_COMPILER=gcc -DCMAKE_CXX_COMPILER=g++
```

### "cmake .." fails with generator error

Make sure you're in the `build` directory:
```powershell
cd C:\Users\slash\Downloads\VM\jarjarvis\build
cmake .. -G "MinGW Makefiles"
```

### "mingw32-make" not found

Use `make` instead, or find the full path:
```powershell
Get-Command mingw32-make
Get-Command make
# Use whichever is found
```

### Build fails with linker errors

MinGW might not be suitable for kernel development. Install the **x86_64-elf cross-compiler**:

1. Download from: https://github.com/lordmilko/i686-elf-tools/releases
2. Extract to `C:\cross-compiler`
3. Add to PATH: `C:\cross-compiler\bin`
4. Rebuild:
```powershell
cmake .. -DCMAKE_C_COMPILER=x86_64-elf-gcc -DCMAKE_CXX_COMPILER=x86_64-elf-g++
```

## 🖥️ Running in VM

### QEMU (Recommended for Testing)

```powershell
# Direct kernel boot (fastest)
qemu-system-x86_64 -kernel jarjarvis.kernel -m 4G -smp 4 -vga std -serial stdio

# With ISO (if created)
qemu-system-x86_64 -cdrom jarjarvis.iso -m 4G -smp 4 -vga std -serial stdio

# With KVM acceleration (Linux only)
qemu-system-x86_64 -kernel jarjarvis.kernel -m 4G -smp 4 -enable-kvm
```

### VirtualBox

1. Open VirtualBox
2. Create New VM:
   - Type: Other
   - Version: Other/Unknown (64-bit)
   - Memory: 4096 MB
   - Storage: Do not add virtual hard disk
3. Settings → Storage → Add Optical Drive → Choose `jarjarvis.iso`
4. Start VM

### VMware

1. Create New VM
2. Typical configuration
3. Installer disc image file: `jarjarvis.iso`
4. Guest OS: Other → Other 64-bit
5. Finish and start

## 📋 Alternative: Using WSL2 (Best Option!)

If you have Windows 10/11 Pro, use WSL2 for the best compatibility:

```powershell
# Install WSL2
wsl --install -d Ubuntu

# Restart, then in WSL:
sudo apt update
sudo apt install build-essential nasm xorriso qemu-system-x86

# Build
cd /mnt/c/Users/slash/Downloads/VM/jarjarvis
./build.sh all

# Run
./build.sh run
```

## 📂 Build Outputs

After successful build, you'll have:

```
build/
├── jarjarvis.kernel       ← Kernel binary (boot this!)
├── jarjarvis.elf          ← ELF format (for debugging)
├── jarjarvis.iso          ← Bootable ISO (if GRUB available)
└── CMakeFiles/            ← Build artifacts
```

## 🎯 PowerShell Build Script

Save this as `build.ps1` in the jarjarvis folder:

```powershell
param([string]$Action="build")

switch ($Action) {
    "clean" {
        Remove-Item -Recurse -Force build -ErrorAction SilentlyContinue
        Write-Host "Build directory cleaned" -ForegroundColor Green
    }
    "build" {
        if (!(Test-Path build)) { mkdir build }
        cd build
        cmake .. -G "MinGW Makefiles"
        mingw32-make -j4
        cd ..
        Write-Host "Build complete!" -ForegroundColor Green
    }
    "run" {
        qemu-system-x86_64 -kernel build/jarjarvis.kernel -m 4G -smp 4 -vga std -serial stdio
    }
    default {
        Write-Host "Usage: .\build.ps1 [clean|build|run]"
    }
}
```

Usage:
```powershell
.\build.ps1 clean
.\build.ps1 build
.\build.ps1 run
```

## ❓ Common Issues

**Q: Can I use Visual Studio?**
A: No. MSVC cannot compile kernel code. Use MinGW or x86_64-elf-gcc.

**Q: Can I use Visual Studio Code?**
A: Yes! VS Code works great with the C/C++ extension. Just configure it to use MinGW.

**Q: Where do I get the cross-compiler?**
A: Download from: https://github.com/lordmilko/i686-elf-tools/releases

**Q: ISO is not created?**
A: GRUB tools are not available on Windows. Use the kernel directly with QEMU, or create ISO in WSL2/Linux.

**Q: Build is slow?**
A: Use `-j4` or `-j8` flag to parallelize compilation based on your CPU cores.

## 📞 Need Help?

- GitHub Issues: https://github.com/jarjarvis/os/issues
- Discord: https://discord.gg/jarjarvis
- Full docs: See `docs/WINDOWS_BUILD.md`

---

**Ready to build the future of AI operating systems! 🚀**
