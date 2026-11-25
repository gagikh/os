# API Index · Ֆունկցիաների ցանկ

Դասընթացում օգտագործվող C/C++ ֆունկցիաները և system call-երը՝ ըստ այն օրվա, երբ դրանք ներմուծվում են։ Ամեն օրվա `README.md`-ի **Functions · Ֆունկցիաներ** բաժնում կան signature-ները՝ մեկական տող մեկնաբանությամբ. այս էջը միայն քարտեզն է։

Signature-ները համապատասխանում են [Linux man pages](https://man7.org/linux/man-pages/)-ին. բաժին 2՝ system call-եր, բաժին 3՝ գրադարանային ֆունկցիաներ։ Տերմինների համար տե՛ս [GLOSSARY.md](GLOSSARY.md), հրամանների համար՝ [COMMANDS.md](COMMANDS.md)։

---

## By day

| Day | Functions introduced |
|---|---|
| [1](day01/README.md#functions--ֆունկցիաներ) | `uname` · `strerror` · `SYS_CHECK` ([common/sys_check.h](common/sys_check.h)) |
| [2](day02/README.md#functions--ֆունկցիաներ) | `main(argc, argv)` · `getopt_long` · `getenv` · `setenv` · `unsetenv` · `environ` · `exit` · `fprintf(stderr)` · `fflush` · `mkstemp` · `unlink` |
| [3](day03/README.md#functions--ֆունկցիաներ) | `stat` · `lstat` · `S_ISREG`/`S_ISDIR`/`S_ISLNK`... · `mkdir` · `opendir` · `readdir` · `closedir` · `link` · `symlink` · `readlink` · `unlink` · `rmdir` · `truncate` · `rename` |
| [4](day04/README.md#functions--ֆունկցիաներ) | `regcomp` · `regexec` · `regerror` · `regfree` · `getopt` |
| [5](day05/README.md#functions--ֆունկցիաներ) | `stat` · `getpwuid` · `getgrgid` · `snprintf` · `strtoul` |
| [6](day06/README.md#functions--ֆունկցիաներ) | bash. `getopts` · `shift` · `local` · `return` · `exit` · `trap` · `shopt -s nullglob` · `declare -A` · `printf` |
| [7](day07/README.md#functions--ֆունկցիաներ) | `getuid` · `geteuid` · `getgid` · `getegid` · `getgroups` · `access` · `setreuid` · `chown` · `chmod` · `umask` · `getpwnam` · `getgrgid` |
| [8](day08/README.md#functions--ֆունկցիաներ) | `getpid` · `getppid` · `nice` · `pause` · `sigaction` · `kill` · `raise` · `sig_atomic_t` · `nanosleep` |
| [9](day09/README.md#functions--ֆունկցիաներ) | `open` · `read` · `write` · `close` · `ioctl` · `major` · `minor` · `std::filesystem::directory_iterator` |
