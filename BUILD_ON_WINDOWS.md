# Building JARJARVIS on Windows - Exact Steps

## ⚠️ CRITICAL: MSVC Cannot Build This!

Visual Studio's compiler (MSVC) **CANNOT** build operating system kernels.

You MUST use:
- **MinGW-w64** (easiest)
- **x86_64-elf-gcc** cross-compiler (best)
- **WSL2** (most compatible)

---

## Method 1: Using MinGW-w64 (Easiest)

### Step 1: Install Prerequisites

Open **PowerShell as Administrator**:

```powershell
# Install NASM
winget install -e --id NASM.NASM

# Install MinGW-w64
winget install -e --id Mingw-w64.Mingw-w64

# Install CMake
winget install -e --id Kitware.CMake

# Install QEMU (optional, for testing)
winget install -e --id SoftwareFreedomConservancy.QEMU
```

**Close and reopen PowerShell** after installation!

### Step 2: Verify Installation

```powershell
nasm -v
gcc --version
cmake --version
```

You should see version numbers, not errors.

### Step 3: Build JARJARVIS

```powershell
# Navigate to your jarjarvis folder
cd C:\Users\slash\Downloads\VM\jarjarvis

# Delete old build folder (if exists)
Remove-Item -Recurse -Force build -ErrorAction SilentlyContinue

# Create build folder
mkdir build
cd build

# Configure with CMake (FORCE MinGW, not MSVC!)
cmake .. -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release

# Build
mingw32-make -j4
```

### Step 4: Run in QEMU

```powershell
# Make sure you're in the build folder
qemu-system-x86_64 -kernel jarjarvis.kernel -m 4G -smp 4 -vga std -serial stdio
```

---

## Method 2: Using WSL2 (Recommended)

### Step 1: Install WSL2

```powershell
# In PowerShell (Administrator)
wsl --install -d Ubuntu

# Restart your computer when prompted
```

### Step 2: Setup in WSL2

```bash
# In WSL2 Ubuntu terminal
sudo apt update
sudo apt install -y build-essential nasm xorriso qemu-system-x86

# Build JARJARVIS
cd /mnt/c/Users/slash/Downloads/VM/jarjarvis
./build.sh all

# Run
./build.sh run
```

---

## Troubleshooting

### Error: "No CMAKE_ASM_NASM_COMPILER could be found"

**Fix:**
```powershell
$env:PATH = "C:\Program Files\NASM;$env:PATH"
# Then retry cmake
```

### Error: "The C compiler identification is MSVC"

**Fix:** Force MinGW:
```powershell
cmake .. -G "MinGW Makefiles" -DCMAKE_C_COMPILER=gcc -DCMAKE_CXX_COMPILER=g++
```

### Error: "cmake .." fails

**Fix:** Make sure you're IN the build folder:
```powershell
cd C:\Users\slash\Downloads\VM\jarjarvis\build
cmake .. -G "MinGW Makefiles"
```

### Error: "mingw32-make" not found

**Fix:** Use `make` instead:
```powershell
make -j4
```

### Build fails with linker errors

**Problem:** MinGW may not produce valid kernel binaries.

**Solution:** Use the x86_64-elf cross-compiler:

1. Download from: https://github.com/lordmilko/i686-elf-tools/releases
2. Extract to `C:\cross-compiler`
3. Add to PATH
4. Rebuild:
```powershell
cmake .. -DCMAKE_C_COMPILER=x86_64-elf-gcc -DCMAKE_CXX_COMPILER=x86_64-elf-g++
```

---

## Quick PowerShell Script

Save this as `build.ps1` in your jarjarvis folder:

```powershell
param([string]$Action = "build")

switch ($Action) {
    "clean" {
        Remove-Item -Recurse -Force build -ErrorAction SilentlyContinue
        Write-Host "Cleaned!" -ForegroundColor Green
    }
    "build" {
        if (!(Test-Path build)) { mkdir build }
        Set-Location build
        cmake .. -G "MinGW Makefiles"
        mingw32-make -j4
        Set-Location ..
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

---

## Expected Output

After successful build, you should have:

```
build/
├── jarjarvis.kernel       ← Kernel binary (this is what you run!)
├── jarjarvis.elf          ← ELF format for debugging
└── CMakeFiles/            ← Build artifacts
```

---

## Running in VirtualBox/VMware

1. Build the project
2. Create a new VM:
   - Type: Other
   - Version: Other/Unknown (64-bit)
   - Memory: 4096 MB
3. Use the kernel file directly or create an ISO in WSL2

---

## Need More Help?

- Full Windows guide: `docs/WINDOWS_BUILD.md`
- General build guide: `docs/BUILD.md`
- Discord: https://discord.gg/jarjarvis

---

**Remember: NEVER use `cmake ..` without `-G "MinGW Makefiles"` on Windows!**
