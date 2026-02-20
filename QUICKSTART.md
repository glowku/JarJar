# JARJARVIS Quick Start Guide

## ⚡ Get Started in 5 Minutes

### 1. Prerequisites

Ensure you have the required tools:
```bash
# Ubuntu/Debian
sudo apt-get install build-essential nasm xorriso qemu-system-x86

# macOS
brew install nasm qemu xorriso

# Arch Linux
sudo pacman -S base-devel nasm qemu xorriso
```

### 2. Install Cross-Compiler

```bash
# Download pre-built cross-compiler
wget https://github.com/jarjarvis/cross-compiler/releases/download/v1.0/x86_64-elf-gcc.tar.gz
sudo tar xzf x86_64-elf-gcc.tar.gz -C /opt
export PATH=/opt/cross/bin:$PATH
```

### 3. Clone and Build

```bash
git clone https://github.com/jarjarvis/os.git
cd jarjarvis
./build.sh all
```

### 4. Run in QEMU

```bash
./build.sh run
```

That's it! JARJARVIS will boot in the QEMU emulator.

## 🎯 First Steps

### Interactive Shell

Once booted, you'll see the prompt:
```
JARVIS>
```

Try these commands:
```bash
# Get help
help

# Check system status
status

# View memory usage
meminfo

# List files
ls

# Interact with AI
jarvis Hello!

# Teach JARVIS something
learn "programming" "The art of writing computer programs"

# Ask JARVIS to think
think
```

### GUI Mode

If your system supports graphics:
```bash
# Start GUI (if not automatic)
startgui

# Or run with GUI enabled
./build.sh run --gui
```

## 📚 Common Tasks

### File Operations
```bash
# Create a file
echo "Hello World" > hello.txt

# View file
cat hello.txt

# Create directory
mkdir myfolder

# Copy file
cp hello.txt myfolder/

# Remove file
rm hello.txt
```

### System Information
```bash
# CPU info
cpuinfo

# Memory info
meminfo

# Disk usage
diskinfo

# Network info
netinfo

# Uptime
uptime
```

### AI Features
```bash
# Ask a question
jarvis What can you do?

# Search knowledge
jarvis Tell me about the kernel

# Generate code
jarvis Generate a function to calculate factorial

# Analyze system
think

# Trigger evolution
evolve
```

### Network
```bash
# Configure network
ifconfig eth0 192.168.1.100 netmask 255.255.255.0

# Test connectivity
ping 8.8.8.8

# View network stats
netinfo
```

## 🔧 Development

### Build Options
```bash
# Full build
make all

# Kernel only
make kernel

# Debug build
make CFLAGS="-g -O0" all

# Clean build
make clean
```

### Testing
```bash
# Run tests
make test

# Debug with GDB
make debug
# In another terminal:
gdb -ex "target remote localhost:1234" build/jarjarvis.kernel
```

### Creating ISO
```bash
make iso
# Output: build/jarjarvis.iso
```

## 🐛 Troubleshooting

### Build Fails
```bash
# Clean and retry
make clean
make all

# Check cross-compiler
which x86_64-elf-gcc

# Verbose build
make V=1 all
```

### QEMU Won't Start
```bash
# Check QEMU installation
qemu-system-x86_64 --version

# Try with less memory
qemu-system-x86_64 -cdrom build/jarjarvis.iso -m 2G

# Enable KVM (Linux)
qemu-system-x86_64 -cdrom build/jarjarvis.iso -m 4G -enable-kvm
```

### AI Not Responding
```bash
# Check AI status
jarvis status

# Restart AI core
service restart jarvis

# Reset AI
jarvis reset
```

## 🎓 Learning Resources

### Documentation
- [Architecture](docs/ARCHITECTURE.md) - System design
- [User Guide](docs/USER_GUIDE.md) - Complete manual
- [Build Guide](docs/BUILD.md) - Build instructions

### Online
- Website: https://jarjarvis.org
- GitHub: https://github.com/jarjarvis/os
- Wiki: https://wiki.jarjarvis.org

### Community
- Discord: https://discord.gg/jarjarvis
- Forum: https://forum.jarjarvis.org
- IRC: #jarjarvis on Libera.Chat

## 💡 Tips

1. **Use Tab Completion**
   ```bash
   JARVIS> cd /u<TAB>  # Completes to /usr
   ```

2. **Command History**
   - Up/Down arrows: Navigate history
   - Ctrl+R: Search history

3. **AI Assistance**
   ```bash
   JARVIS> How do I compress files?
   JARVIS> Explain the memory command
   ```

4. **Shortcuts**
   - `Ctrl+L`: Clear screen
   - `Ctrl+C`: Cancel command
   - `Ctrl+D`: Exit shell

## 🚀 Next Steps

1. Explore the [User Guide](docs/USER_GUIDE.md)
2. Read about the [Architecture](docs/ARCHITECTURE.md)
3. Try building from source
4. Join the community
5. Contribute to the project!

## 📞 Support

- **Issues**: https://github.com/jarjarvis/os/issues
- **Email**: support@jarjarvis.org
- **Chat**: Discord or IRC

---

**Welcome to the future of AI operating systems!**

*JARJARVIS - Intelligence at the core*
