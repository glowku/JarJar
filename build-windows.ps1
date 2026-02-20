# JARJARVIS Windows Build Script
# PowerShell script to build JARJARVIS on Windows

param(
    [string]$Action = "all",
    [switch]$InstallDeps,
    [switch]$Clean
)

$ErrorActionPreference = "Stop"

# Colors
function Write-Color($Text, $Color) {
    Write-Host $Text -ForegroundColor $Color
}

function Write-Info($Text) {
    Write-Color "[INFO] $Text" Cyan
}

function Write-Success($Text) {
    Write-Color "[SUCCESS] $Text" Green
}

function Write-Warning($Text) {
    Write-Color "[WARNING] $Text" Yellow
}

function Write-Error($Text) {
    Write-Color "[ERROR] $Text" Red
}

# Header
function Show-Header {
    Write-Color @"
╔══════════════════════════════════════════════════════════════╗
║              JARJARVIS Windows Build System                  ║
║         Building the Future of AI Operating Systems          ║
╚══════════════════════════════════════════════════════════════╝
"@ Cyan
    Write-Host ""
}

# Check prerequisites
function Test-Prerequisites {
    Write-Info "Checking prerequisites..."
    
    $missing = @()
    
    # Check NASM
    $nasm = Get-Command nasm -ErrorAction SilentlyContinue
    if (-not $nasm) {
        $missing += "NASM"
    } else {
        Write-Success "NASM found: $($nasm.Source)"
    }
    
    # Check for cross-compiler
    $elfGcc = Get-Command x86_64-elf-gcc -ErrorAction SilentlyContinue
    $mingwGcc = Get-Command x86_64-w64-mingw32-gcc -ErrorAction SilentlyContinue
    
    if (-not $elfGcc -and -not $mingwGcc) {
        $missing += "Cross-compiler (x86_64-elf-gcc or MinGW-w64)"
    } else {
        if ($elfGcc) {
            Write-Success "x86_64-elf-gcc found: $($elfGcc.Source)"
        }
        if ($mingwGcc) {
            Write-Success "x86_64-w64-mingw32-gcc found: $($mingwGcc.Source)"
        }
    }
    
    # Check CMake
    $cmake = Get-Command cmake -ErrorAction SilentlyContinue
    if (-not $cmake) {
        $missing += "CMake"
    } else {
        Write-Success "CMake found: $($cmake.Source)"
    }
    
    # Check QEMU (optional)
    $qemu = Get-Command qemu-system-x86_64 -ErrorAction SilentlyContinue
    if (-not $qemu) {
        Write-Warning "QEMU not found (optional for testing)"
    } else {
        Write-Success "QEMU found: $($qemu.Source)"
    }
    
    if ($missing.Count -gt 0) {
        Write-Error "Missing prerequisites:"
        foreach ($item in $missing) {
            Write-Host "  - $item" -ForegroundColor Red
        }
        Write-Host ""
        Write-Info "Run with -InstallDeps to install dependencies automatically"
        return $false
    }
    
    return $true
}

# Install dependencies
function Install-Dependencies {
    Write-Info "Installing dependencies..."
    
    # Check if winget is available
    $winget = Get-Command winget -ErrorAction SilentlyContinue
    $choco = Get-Command choco -ErrorAction SilentlyContinue
    
    # Install NASM
    Write-Info "Installing NASM..."
    if ($winget) {
        winget install -e --id NASM.NASM --accept-source-agreements --accept-package-agreements
    } elseif ($choco) {
        choco install nasm -y
    } else {
        Write-Error "Neither winget nor chocolatey found. Please install NASM manually."
        Write-Info "Download from: https://www.nasm.us/pub/nasm/releasebuilds/"
        return $false
    }
    
    # Install MinGW-w64
    Write-Info "Installing MinGW-w64..."
    if ($winget) {
        winget install -e --id Mingw-w64.Mingw-w64 --accept-source-agreements --accept-package-agreements
    } elseif ($choco) {
        choco install mingw -y
    } else {
        Write-Error "Please install MinGW-w64 manually"
        return $false
    }
    
    # Install CMake
    Write-Info "Installing CMake..."
    if ($winget) {
        winget install -e --id Kitware.CMake --accept-source-agreements --accept-package-agreements
    } elseif ($choco) {
        choco install cmake -y
    }
    
    # Install QEMU (optional)
    Write-Info "Installing QEMU..."
    if ($winget) {
        winget install -e --id SoftwareFreedomConservancy.QEMU --accept-source-agreements --accept-package-agreements
    } elseif ($choco) {
        choco install qemu -y
    }
    
    Write-Success "Dependencies installed!"
    Write-Warning "Please restart PowerShell and run this script again."
    return $true
}

# Clean build
function Clear-Build {
    Write-Info "Cleaning build directory..."
    if (Test-Path "build") {
        Remove-Item -Recurse -Force "build"
    }
    New-Item -ItemType Directory -Force -Path "build" | Out-Null
    Write-Success "Build directory cleaned"
}

# Build project
function Build-Project {
    param([string]$BuildType = "Release")
    
    Write-Info "Building JARJARVIS ($BuildType)..."
    
    # Create build directory
    if (-not (Test-Path "build")) {
        New-Item -ItemType Directory -Force -Path "build" | Out-Null
    }
    
    Set-Location "build"
    
    # Configure with CMake
    Write-Info "Configuring with CMake..."
    $cmakeArgs = @(
        "..",
        "-G", "MinGW Makefiles",
        "-DCMAKE_BUILD_TYPE=$BuildType"
    )
    
    & cmake @cmakeArgs
    if ($LASTEXITCODE -ne 0) {
        Write-Error "CMake configuration failed!"
        Set-Location ..
        return $false
    }
    
    # Build
    Write-Info "Compiling..."
    & cmake --build . --parallel
    if ($LASTEXITCODE -ne 0) {
        Write-Error "Build failed!"
        Set-Location ..
        return $false
    }
    
    Set-Location ..
    Write-Success "Build completed successfully!"
    return $true
}

# Run in QEMU
function Start-QEMU {
    param([switch]$Iso)
    
    $qemu = Get-Command qemu-system-x86_64 -ErrorAction SilentlyContinue
    if (-not $qemu) {
        Write-Error "QEMU not found! Please install QEMU."
        return
    }
    
    if ($Iso) {
        $isoPath = "build/jarjarvis.iso"
        if (-not (Test-Path $isoPath)) {
            Write-Error "ISO not found! Build first with: .\build-windows.ps1 all"
            return
        }
        Write-Info "Starting JARJARVIS from ISO..."
        & qemu-system-x86_64 -cdrom $isoPath -m 4G -smp 4 -vga std -serial stdio
    } else {
        $kernelPath = "build/jarjarvis.kernel"
        if (-not (Test-Path $kernelPath)) {
            Write-Error "Kernel not found! Build first with: .\build-windows.ps1 all"
            return
        }
        Write-Info "Starting JARJARVIS kernel..."
        & qemu-system-x86_64 -kernel $kernelPath -m 4G -smp 4 -vga std -serial stdio
    }
}

# Debug mode
function Start-Debug {
    $kernelPath = "build/jarjarvis.kernel"
    if (-not (Test-Path $kernelPath)) {
        Write-Error "Kernel not found! Build first with: .\build-windows.ps1 all"
        return
    }
    
    Write-Info "Starting JARJARVIS in debug mode..."
    Write-Info "Connect GDB with: target remote localhost:1234"
    
    Start-Process qemu-system-x86_64 -ArgumentList "-kernel", $kernelPath, "-m", "4G", "-smp", "4", "-vga", "std", "-s", "-S", "-serial", "stdio"
}

# Main
Show-Header

if ($InstallDeps) {
    Install-Dependencies
    exit 0
}

if ($Clean) {
    Clear-Build
    exit 0
}

# Check prerequisites
if (-not (Test-Prerequisites)) {
    exit 1
}

# Execute action
switch ($Action.ToLower()) {
    "all" {
        Clear-Build
        Build-Project "Release"
    }
    "build" {
        Build-Project "Release"
    }
    "debug" {
        Build-Project "Debug"
    }
    "run" {
        Start-QEMU
    }
    "run-iso" {
        Start-QEMU -Iso
    }
    "gdb" {
        Start-Debug
    }
    "clean" {
        Clear-Build
    }
    "help" {
        Write-Host "Usage: .\build-windows.ps1 [Action] [Options]"
        Write-Host ""
        Write-Host "Actions:"
        Write-Host "  all       - Clean and build everything (default)"
        Write-Host "  build     - Build without cleaning"
        Write-Host "  debug     - Build with debug symbols"
        Write-Host "  run       - Run kernel in QEMU"
        Write-Host "  run-iso   - Run ISO in QEMU"
        Write-Host "  gdb       - Start debug server for GDB"
        Write-Host "  clean     - Clean build directory"
        Write-Host "  help      - Show this help"
        Write-Host ""
        Write-Host "Options:"
        Write-Host "  -InstallDeps  - Install dependencies"
        Write-Host "  -Clean        - Clean before build"
        Write-Host ""
        Write-Host "Examples:"
        Write-Host "  .\build-windows.ps1 -InstallDeps"
        Write-Host "  .\build-windows.ps1 all"
        Write-Host "  .\build-windows.ps1 run"
    }
    default {
        Write-Error "Unknown action: $Action"
        Write-Info "Use 'help' for usage information"
        exit 1
    }
}
