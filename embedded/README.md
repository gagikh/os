# Embedded Linux · Ներդրված Linux համակարգեր

**Course**: Embedded Linux for LusIR lab students: lectures and labs (3 days)  
**Boards**: NVIDIA Jetson Orin / Nano · Raspberry Pi 4 / 5 · SolidRun HummingBoard (NXP i.MX8)  
**Languages**: C, C++, Python  
**Instructor**: Gagik Hakobyan

📖 [Glossary](GLOSSARY.md). դասընթացի տերմինները՝ այբբենական կարգով, նշված այն օրով, երբ ներմուծվում են։  
⌨️ [Commands](COMMANDS.md). բոլոր հրամանները՝ խմբերով։  
📐 [API Index](API.md). C/C++ և Python ֆունկցիաները՝ ըստ օրերի։

Դասընթացը չի ենթադրում Linux-ի նախնական գիտելիք։ Օր 1-ը սկսվում է զրոյից։

---

## 💻 What you need · Ինչ է պետք

- **Բորդ**՝ երեքից որևէ մեկը, հոսանքի աղբյուրով և ցանցային կապով (Ethernet կամ Wi-Fi), արդեն տեղադրված OS-ով։
- **PC**՝ Linux (Ubuntu), կամ Windows **WSL2**-ով (Ubuntu), կամ macOS։ Windows-ում WSL2-ը տեղադրվում է PowerShell-ից՝ `wsl --install`։
- Ցանկալի է. **USB-to-UART մալուխ** (3.3 V) serial console-ի համար և **VS Code**՝ Remote-SSH extension-ով։

PC-ի վրա մեկ անգամ (Ubuntu / WSL2).

```bash
sudo apt update
sudo apt install -y build-essential cmake git g++-aarch64-linux-gnu qemu-user minicom
```

Բորդ չկա՞։ Օր 1-ի ամբողջ նյութը աշխատում է PC-ի Ubuntu-ի կամ WSL2-ի վրա, իսկ ARM binary-ները կարելի է գործարկել `qemu-aarch64`-ով։

## 🛠️ Build · Հավաքում

```bash
cd embedded
cmake -S . -B build && cmake --build build                    # native (բորդի կամ PC-ի վրա)
cmake -S . -B build-arm64 -DCMAKE_TOOLCHAIN_FILE=cmake/aarch64-linux-gnu.cmake
cmake --build build-arm64                                     # cross (PC → բորդ)
```

Binary-ները հայտնվում են `build*/bin/`-ում։ Python ծրագրերը հավաքում չեն պահանջում։

---

## 📘 Course Outline · Դասընթացի պլան

### [Day 1: Linux from Zero, on Your Board](day01/README.md)
- Ինչ է Linux-ը. kernel, distribution, user space և kernel space
- Միանալ բորդին. SSH, serial console, `scp`
- Shell, ֆայլային ծառ, pipe և redirection
- Users, groups, permissions, `sudo`
- Process-ներ և փաթեթներ
- Առաջին ծրագրերը C, C++, Python լեզուներով. native և cross build

### Day 2: How the Board Starts, and Talking to Hardware *(պատրաստվում է)*
- Boot-ի շղթան. bootloader, kernel, device tree, systemd
- «Ամեն ինչ ֆայլ է». `/dev`, `/sys`
- GPIO (`libgpiod`), I2C, UART. Python-ով և C/C++-ով

### Day 3: Inside the Kernel, and Timing *(պատրաստվում է)*
- Kernel module. «hello» module և պարզ character device
- Real-time. scheduling, `SCHED_FIFO`, `mlockall`, jitter-ի չափում
