# Linux Course · Linux օպերացիոն համակարգ

**Title**: Linux: Usage, Administration and System Programming  
**University**: Yerevan State University – Lectures & Labs (11 Days)  
**Instructor**: Gagik Hakobyan

📖 [Glossary](GLOSSARY.md) — the terms this course uses, alphabetical, tagged with the day it's introduced.  
⌨️ [Commands](COMMANDS.md) — every shell command the course uses, grouped by topic and tagged by day.  
📐 [API Index](API.md) — every C/C++ function and system call the course uses, mapped to the day that introduces it; each day's `Functions` section has the signatures.  
📋 [Practice Tasks](TASKS.md) — every day's Self-Learning tasks in one list.  
🔧 [Embedded Linux](../embedded/README.md) — the follow-up course: the same system on Jetson, Raspberry Pi and i.MX8 boards.

---

## 💻 Setup

You need any Linux: Ubuntu 22.04 or 24.04 is the reference.

- **Windows**: WSL2. In PowerShell, `wsl --install`, then Ubuntu from the Microsoft Store.
- **macOS**: a virtual machine (UTM, VirtualBox) with Ubuntu. macOS's own terminal is Unix, but it has no `/proc` and lacks several Linux system calls this course uses.
- **Linux**: you're done.

Once, inside Ubuntu:

```bash
sudo apt update
sudo apt install -y build-essential cmake git man-db manpages-dev ncal netcat-openbsd
```

Build every day's template at once:

```bash
cd linux
cmake -S . -B build && cmake --build build
./build/bin/day01
```

Every template uses [`common/sys_check.h`](common/sys_check.h): wrap a system call in `SYS_CHECK(...)` and the program stops at that line with the reason (`strerror(errno)`) instead of failing silently later.

---

## 📘 Course Outline

### [Day 1: Introduction to Linux](day01/README.md)
- What an OS is; Unix, GNU, MINIX, Linux, POSIX  
- The Unix philosophy  
- Kernel structure: monolithic, modules, microkernel  
- Distributions  
- Open source and licenses: MIT, BSD, Apache, LGPL, GPL  
- Working environment; the shell, help and first commands

### [Day 2: The Shell and the Program's Environment](day02/README.md)
- What a program receives: arguments, environment, standard streams; what it returns: exit status  
- `argc`/`argv`, GNU option conventions, `getopt_long`  
- Redirection (`>`, `2>`, `2>&1`) and buffering  
- Exit codes, `$?`, `&&`, `||`  
- Shell vs environment variables, `export`, `PATH`, inheritance  
- Temporary files; system information: `who`, `stat`, `vmstat`, `lsof`, `strace`

### [Day 3: Files and Directories](day03/README.md)
- File types; the directory tree, partitions and mount points  
- Creating, copying, moving and removing files; `ls` options  
- Inodes; the ext2 layout: blocks, groups, superblock, bitmaps, direct and indirect blocks  
- Directories and path lookup  
- Hard and symbolic links  
- VFS, sparse files, EOF

### [Day 4: Pipes, Filters and Regular Expressions](day04/README.md)
- What a pipe is: a kernel buffer with two file descriptors; `pipe()` in C  
- Parent and child: closing unused ends, EOF, the read/write rules  
- Buffer size (16 pages, `F_GETPIPE_SZ`/`F_SETPIPE_SZ`), backpressure, deadlock; `PIPE_BUF` atomicity  
- `SIGPIPE`/`EPIPE`; how the shell builds `a | b` (`fork`, `dup2`, `exec`), `popen`; FIFOs  
- Filters, regular expressions, `grep`, `find`, `sed`, `awk`

### [Day 5: Archiving and Compression](day05/README.md)
- Archive vs compression; `tar` create, list, extract  
- Inside a `.tar`: the ustar header and 512-byte blocks  
- How compression works: redundancy, RLE, LZ77, Huffman; lossless vs lossy  
- `gzip`, `bzip2`, `xz`, `zstd`, `zip`; backups, checksums, archives over ssh

### [Day 6: Bash Scripting](day06/README.md)
- Shebang, running vs sourcing a script  
- Variables, quoting, parameter expansion, arithmetic, arrays  
- Arguments: `$1`, `"$@"`, `shift`, `getopts`  
- Conditions (`[ ]`, `[[ ]]`, `(( ))`, `case`) and loops (`for`, `while read`, `until`)  
- Functions; robust scripts: `set -euo pipefail`, `trap`, `shellcheck`

### [Day 7: Users, Ownership and Permissions](day07/README.md)
- UIDs, GIDs, root; `/etc/passwd`, `/etc/shadow`, `/etc/group`  
- Managing users and groups; `sudo` vs `su`  
- How the kernel checks permissions; `rwx` on files vs directories  
- `chmod`, `chown`, `umask`  
- setuid, setgid, sticky; real vs effective UID  
- Securing files; common security holes (buffer overruns, races, `system()`)

### [Day 8: Processes, Signals, Services and Packages](day08/README.md)
- What a process is: address space, PCB, states (R, S, D, T, Z)  
- Viewing processes: `ps`, `top`, `/proc/PID`; `nice`; job control  
- Signals: `kill`, `SIGTERM` vs `SIGKILL`, `sigaction`, safe handlers  
- systemd: `systemctl`, `journalctl`, writing a unit file  
- Packages: `apt`, `dpkg`

### [Day 9: Hardware and Drivers](day09/README.md)
- From `read()` to the device: driver, bus, DMA, interrupt  
- Finding the hardware: `lscpu`, `lsblk`, `lspci -k`, `lsusb`  
- Device files: character vs block, major/minor, `/dev`, udev rules  
- Special devices; kernel modules (`modprobe`)  
- `/proc` and `/sys`; `ioctl`

### [Day 10: Processes and Threads](day10/README.md)
- `fork`, `exec`, `waitpid`; exit status and `$?`; zombies and orphans  
- How a shell works; pipes and redirection from the inside (`pipe`, `dup2`)  
- Threads: `std::thread`, pthreads, `clone`  
- Race conditions, mutexes, deadlock, condition variables  
- Processes vs threads

### [Day 11: Networking and Sockets](day11/README.md)
- IP, interfaces, ports, TCP vs UDP, DNS; `ip`, `ss`, `ping`, `nc`, `nmap`  
- Socket API: `socket`, `bind`, `listen`, `accept`, `connect`, `send`, `recv`  
- Byte order (`htons`), TCP is a byte stream, partial `send`, `SIGPIPE`  
- Many clients: `fork`, threads, `poll`/`epoll`
