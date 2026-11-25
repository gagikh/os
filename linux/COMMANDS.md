# Commands · Հրամանների ցանկ

Դասընթացում օգտագործվող հրամանները՝ խմբերով։ Վերջին սյունակը ցույց է տալիս այն օրը, երբ հրամանը ներմուծվում է։ Ամեն հրամանի մասին ավելին՝ `man <հրաման>` կամ `<հրաման> --help`։

Տերմինների համար տե՛ս [GLOSSARY.md](GLOSSARY.md), C/C++ ֆունկցիաների համար՝ [API.md](API.md)։

## Համակարգի մասին

| Հրաման | Ինչ է անում | Օր |
|---|---|---|
| `whoami` | Ընթացիկ user-ը | 1 |
| `hostname` | Համակարգչի անունը | 1 |
| `date`, `date -u` | Ամսաթիվը և ժամը, UTC-ով | 1 |
| `cal`, `cal -j 3 1999` | Օրացույց | 1 |
| `free -h` | Memory | 1 |
| `w`, `uptime` | Ով է մուտք գործել, որքան է աշխատում համակարգը | 1 |
| `uname -a`, `uname -r` | Kernel-ը, տարբերակը, architecture-ը | 1 |
| `cat /proc/version` | Ինչով և երբ է հավաքվել kernel-ը | 1 |
| `cat /etc/os-release` | Distribution-ը | 1 |
| `cat /proc/sys/kernel/tainted` | Kernel-ի taint դրոշները (`0`՝ մաքուր) | 1 |
| `lsmod`, `modinfo name` | Բեռնված module-ները, module-ի մասին | 1 |
| `arch` | Architecture | 2 |
| `who`, `tty`, `last` | Ով է մուտք գործել, իմ terminal-ը, մուտքերի պատմությունը | 2 |
| `write user pts/N`, `mesg n` | Գրել ուրիշի terminal-ում / արգելել | 2 |
| `stat file` | Ֆայլի մանրամասները | 2 |
| `du -sh dir`, `df -h` | Directory-ի չափը, disk-ների ազատ տեղը | 2 |
| `vmstat 1 5` | Memory, CPU, I/O | 2 |
| `lsof -p PID` | Process-ի բաց ֆայլերը | 2 |
| `dmesg` | Kernel-ի հաղորդագրությունները | 2 |
| `strace -c cmd`, `strace -e trace=openat cmd` | Ծրագրի system call-երը | 2 |
| `ltrace -c cmd` | Գրադարանային ֆունկցիաների կանչերը | 2 |
| `kill -l` | Signal-ների ցանկը | 2 |

## Shell

| Հրաման | Ինչ է անում | Օր |
|---|---|---|
| `echo $SHELL` | Ընթացիկ shell-ը | 1 |
| `which cmd`, `type cmd` | Որտեղ է ծրագիրը, built-in է արդյոք | 1 |
| `history`, `!42`, `!!` | Նախորդ հրամանները, կրկնել | 1 |
| Ctrl+R, Ctrl+A/E, Ctrl+U/K, Ctrl+W | Փնտրել history-ում, տողի խմբագրում | 1 |
| `cmd1 \| cmd2` | Pipe. առաջինի ելքը՝ երկրորդի մուտք | 1 |
| `grep word` | Տողեր, որոնք պարունակում են բառը | 1 |
| `ps -e -o ruser,comm` | Process-ները և դրանց owner-ները | 1 |

## Redirection և exit status

| Հրաման | Ինչ է անում | Օր |
|---|---|---|
| `cmd > f`, `cmd >> f` | stdout ֆայլ / ավելացնել | 2 |
| `cmd 2> f` | stderr ֆայլ | 2 |
| `cmd > f 2>&1`, `cmd &> f` | Երկուսն էլ ֆայլ | 2 |
| `cmd 2>&1 \| less` | Երկուսն էլ pipe | 2 |
| `cmd < f` | stdin ֆայլից | 2 |
| `echo $?`, `${PIPESTATUS[@]}` | Exit status | 2 |
| `a && b`, `a \|\| b`, `! a` | Պայմանական կատարում | 2 |
| `true`, `false` | Միշտ 0 / միշտ 1 | 2 |

## Variable-ներ

| Հրաման | Ինչ է անում | Օր |
|---|---|---|
| `x=10`, `echo $x` | Shell variable | 2 |
| `export x`, `export X=1` | Environment variable | 2 |
| `env`, `printenv NAME` | Environment-ը | 2 |
| `set` | Բոլոր variable-ները | 2 |
| `unset x` | Ջնջել | 2 |
| `X=1 cmd` | Variable միայն մեկ հրամանի համար | 2 |
| `source ~/.bashrc` | Կատարել ֆայլը ընթացիկ shell-ում | 2 |
| `mktemp`, `mktemp -d` | Ժամանակավոր ֆայլ / directory | 2 |

## Օգնություն

| Հրաման | Ինչ է անում | Օր |
|---|---|---|
| `cmd --help` | Կարճ օգնություն | 1 |
| `man cmd`, `man 2 name` | Ձեռնարկ, ընտրված բաժնից | 1 |
| `man -k word`, `apropos word` | Փնտրել բոլոր man page-երում | 1 |
| `info cmd` | GNU-ի ձեռնարկը | 1 |

## Network

| Հրաման | Ինչ է անում | Օր |
|---|---|---|
| `ssh user@host` | Shell հեռավոր համակարգում | 1 |
| `ping -c 3 host` | Հասանելի՞ է host-ը | 1 |
| `nc -vz host 22` | Բա՞ց է port-ը | 1 |
| `printf 'GET / ...' \| nc host 80` | Ձեռքով HTTP հարցում | 1 |

## Ֆայլեր և directory-ներ

| Հրաման | Ինչ է անում | Օր |
|---|---|---|
| `pwd`, `cd dir`, `cd ..`, `cd -` | Որտեղ եմ, շարժվել | 3 |
| `ls -a`, `-l`, `-h`, `-S`, `-t`, `-d`, `-F`, `-i`, `-R` | Ցանկ. թաքնված, մանրամասն, չափեր, դասավորում, տեսակ, inode | 3 |
| `tree -L 2 -d` | Ծառը | 3 |
| `mkdir -p a/b`, `rmdir dir` | Ստեղծել / ջնջել դատարկ directory | 3 |
| `touch f` | Ստեղծել դատարկ ֆայլ, թարմացնել ժամը | 3 |
| `cp a b`, `cp -r`, `cp -a` | Պատճենել | 3 |
| `mv a b` | Տեղափոխել / վերանվանել | 3 |
| `rm f`, `rm -r dir`, `rm -i` | Ջնջել | 3 |
| `file f` | Տեսակը ըստ պարունակության | 3 |
| `stat f` | Inode-ի դաշտերը | 3 |
| `ln a b`, `ln -s a b` | Hard / symbolic link | 3 |
| `readlink -f link` | Ուր է ցույց տալիս link-ը | 3 |
| `find . -samefile f`, `find / -inum N` | Hard link-երը, ֆայլը ըստ inode-ի | 3 |
| `mkfifo p` | Named pipe | 3 |
| `truncate -s 1G f` | Փոխել չափը, sparse ֆայլ | 3 |

## Filesystem-ներ

| Հրաման | Ինչ է անում | Օր |
|---|---|---|
| `df -h`, `df -i` | Ազատ տեղ, ազատ inode-ներ | 3 |
| `du -h f` | Disk-ի վրա զբաղեցրած տեղը | 3 |
| `lsblk -f` | Disk-ներ, partition-ներ, filesystem-ներ | 3 |
| `mount dev dir`, `umount dir` | Միացնել / անջատել filesystem | 3 |
| `findmnt`, `cat /etc/fstab` | Ինչ է mount արված / ինչ է mount արվում boot-ին | 3 |
| `cat /proc/filesystems` | Kernel-ի աջակցած filesystem-ները | 3 |
| `mkfs.ext4 img` | Ստեղծել ext4 | 3 |
| `dumpe2fs -h img`, `debugfs -R 'stat <2>' img` | Superblock-ը, inode-ը | 3 |
| `e2fsck -b 32768 dev` | Ստուգել, պահուստային superblock-ով | 3 |

## Pipe-եր և ֆիլտրեր

| Հրաման | Ինչ է անում | Օր |
|---|---|---|
| `ls -l /proc/PID/fd` | Process-ի fd-ները. pipe-երը երևում են որպես `pipe:[inode]` | 4 |
| `getconf PIPE_BUF /`, `getconf PAGESIZE`, `cat /proc/sys/fs/pipe-max-size` | Pipe-ի սահմանները | 4 |
| `echo "${PIPESTATUS[@]}"` | Pipeline-ի բոլոր հրամանների exit status-ները | 4 |
| `mkfifo name` | Անվանված pipe (FIFO) | 4 |
| `a \| b \| c` | Pipeline | 4 |
| `tee f` | Պատճենել stdin-ը ֆայլ և stdout | 4 |
| `xargs cmd`, `find -print0 \| xargs -0` | stdin-ի բառերը դարձնել argument-ներ | 4 |
| `cmd <<< "text"`, `cmd <<EOF` | Here-string, here-document | 4 |
| `diff <(a) <(b)` | Process substitution | 4 |
| `head -n N`, `tail -n N`, `tail -n +N` | Առաջին / վերջին տողերը | 4 |
| `wc -l -w -c` | Տողեր, բառեր, բայթեր | 4 |
| `sort -n -r -k N -t: -u` | Դասավորել | 4 |
| `uniq -c`, `uniq -d` | Հարևան կրկնությունները | 4 |
| `cut -d: -f1,7`, `cut -c1-10` | Դաշտեր / նիշեր | 4 |
| `tr a-z A-Z`, `tr -d`, `tr -s` | Փոխարինել / ջնջել / սեղմել նիշերը | 4 |
| `paste`, `nl`, `rev` | Միացնել սյունակներ, համարակալել, շրջել | 4 |

## Որոնում և տեքստի մշակում

| Հրաման | Ինչ է անում | Օր |
|---|---|---|
| `grep -i -n -w -c -v -o -l -r -q` | Փնտրել տողեր regex-ով | 4 |
| `grep -E`, `grep -F` | Extended regex / բառացի տող | 4 |
| `grep -A N -B N -C N` | Համատեքստ | 4 |
| `find dir -name -type -size -mtime -user -perm` | Փնտրել ֆայլեր | 4 |
| `find ... -exec cmd {} +`, `-delete` | Գործողություն գտնվածների վրա | 4 |
| `sed 's/a/b/g'`, `sed -i.bak`, `sed -n '5,10p'`, `sed '/re/d'` | Stream editor | 4 |
| `awk '{print $1}'`, `awk -F:`, `BEGIN{} {} END{}` | Սյունակներով մշակում | 4 |

## Archive-ներ և compression

| Հրաման | Ինչ է անում | Օր |
|---|---|---|
| `tar -cvf a.tar dir` | Ստեղծել archive | 5 |
| `tar -tvf a.tar` | Դիտել պարունակությունը | 5 |
| `tar -xvf a.tar -C dir` | Բացել | 5 |
| `tar -czf`, `-cjf`, `-cJf`, `--zstd -cf` | Archive + gzip / bzip2 / xz / zstd | 5 |
| `tar --exclude='*.o'` | Բաց թողնել ֆայլեր | 5 |
| `gzip -k -9 f`, `gunzip f.gz`, `gzip -l f.gz` | gzip | 5 |
| `bzip2`, `xz -T0`, `zstd` | Այլ սեղմիչներ | 5 |
| `zcat`, `zless`, `zgrep` | Կարդալ սեղմված ֆայլը առանց բացելու | 5 |
| `zip -r a.zip dir`, `unzip -l a.zip`, `unzip a.zip -d dir` | zip | 5 |
| `sha256sum f`, `sha256sum -c f.sha256` | Checksum, ստուգում | 5 |
| `split -b 100M f part_` | Բաժանել մասերի | 5 |
| `tar -czf - dir \| ssh host 'tar -xzf - -C dest'` | Պատճենել network-ով | 5 |
| `xxd f \| head` | Ֆայլի բայթերը | 5 |

## Bash script-եր

| Հրաման | Ինչ է անում | Օր |
|---|---|---|
| `chmod +x s.sh && ./s.sh` | Գործարկել script | 6 |
| `source s.sh`, `. s.sh` | Կատարել ընթացիկ shell-ում | 6 |
| `bash -n s.sh`, `bash -x s.sh` | Ստուգել syntax-ը / տպել ամեն հրամանը | 6 |
| `shellcheck s.sh` | Գտնել տիպիկ սխալները | 6 |
| `${v:-def}`, `${#v}`, `${v,,}`, `${f##*.}`, `${f%.*}`, `${v/a/b}` | Parameter expansion | 6 |
| `$(cmd)`, `$(( expr ))` | Command substitution, թվաբանություն | 6 |
| `arr=(a b)`, `"${arr[@]}"`, `declare -A m` | Զանգվածներ | 6 |
| `$0 $1 $# "$@"`, `shift`, `getopts` | Argument-ներ | 6 |
| `if`, `[ ]`, `[[ ]]`, `(( ))`, `case` | Պայմաններ | 6 |
| `for`, `while read -r`, `until`, `break`, `continue` | Ցիկլեր | 6 |
| `f() { local x=$1; ... }` | Ֆունկցիա | 6 |
| `set -euo pipefail`, `trap '...' EXIT` | Հուսալի script | 6 |
| `read -r -p "prompt" var` | Կարդալ մուտքը | 6 |

## User-ներ և permission-ներ (մանրամասն)

| Հրաման | Ինչ է անում | Օր |
|---|---|---|
| `id user`, `getent passwd user`, `getent group g` | User-ի և group-ի տվյալները | 7 |
| `sudo adduser u`, `sudo useradd -m -s /bin/bash u` | Ստեղծել user | 7 |
| `sudo passwd u`, `sudo usermod -L u`, `sudo deluser --remove-home u` | Password, արգելափակել, ջնջել | 7 |
| `sudo groupadd g`, `sudo usermod -aG g u`, `sudo gpasswd -d u g` | Group-եր | 7 |
| `sudo -i`, `sudo -u u cmd`, `su - u`, `sudo -l`, `sudo visudo` | Ուրիշի permission-ներով | 7 |
| `chmod u+x,g-w,o-r f`, `chmod 640 f`, `chmod -R g+rX d` | Permission-ներ | 7 |
| `chmod u+s`, `g+s`, `+t`, `chmod 2775 d` | Հատուկ բիթեր | 7 |
| `sudo chown u:g f`, `chgrp g f` | Owner, group | 7 |
| `umask`, `umask 027` | Նոր ֆայլերի permission-ները | 7 |
| `stat -c '%A %a %U %G' f` | Permission-ները երկու ձևով | 7 |
| `find / -perm -4000 -type f`, `find -perm -o+w` | Setuid / բոլորի համար գրելի ֆայլեր | 7 |
| `getfacl f`, `setfacl -m u:anna:r f` | ACL | 7 |
| `getcap /usr/bin/ping` | Capabilities | 7 |
| `last`, `lastlog`, `/var/log/auth.log` | Մուտքերի պատմություն | 7 |

## Process-ներ, signal-ներ, service-ներ, package-ներ

| Հրաման | Ինչ է անում | Օր |
|---|---|---|
| `ps aux`, `ps -ef --forest`, `ps -o pid,ppid,stat,ni,cmd` | Process-ների ցանկ | 8 |
| `pstree -p`, `pgrep -a name`, `pidof name` | Ծառ, PID ըստ անվան | 8 |
| `cat /proc/PID/status`, `ls -l /proc/PID/fd` | Process-ի տվյալները | 8 |
| `nice -n 10 cmd`, `renice -n 5 -p PID` | Priority | 8 |
| `cmd &`, Ctrl+Z, `jobs`, `fg %1`, `bg %1` | Job control | 8 |
| `nohup cmd &`, `disown` | Չավարտվել terminal-ը փակելիս | 8 |
| `kill -TERM PID`, `kill -9 PID`, `kill -HUP`, `-USR1`, `-STOP`, `-CONT` | Signal-ներ | 8 |
| `pkill -f pattern`, `killall name` | Signal ըստ անվան | 8 |
| `systemctl status\|start\|stop\|restart\|reload u` | Service-ի կառավարում | 8 |
| `systemctl enable --now u`, `disable` | Boot-ի ժամանակ | 8 |
| `systemctl list-units --type=service`, `--state=failed` | Service-ների ցանկ | 8 |
| `systemctl cat u`, `systemctl edit u`, `systemctl daemon-reload` | Unit ֆայլեր | 8 |
| `journalctl -u u -f`, `journalctl -b -p err` | Log-եր | 8 |
| `systemd-analyze blame` | Boot-ի ժամանակը | 8 |
| `apt update`, `apt upgrade`, `apt list --upgradable` | Թարմացում | 8 |
| `apt install`, `remove`, `purge`, `autoremove` | Տեղադրել / ջնջել | 8 |
| `apt search`, `apt show`, `apt policy` | Փնտրել, տեղեկություն | 8 |
| `dpkg -l`, `dpkg -L pkg`, `dpkg -S file` | Տեղադրված package-ներ և ֆայլեր | 8 |

## Hardware և driver-ներ

| Հրաման | Ինչ է անում | Օր |
|---|---|---|
| `lscpu`, `cat /proc/cpuinfo` | CPU | 9 |
| `lsblk -o NAME,SIZE,TYPE,MOUNTPOINT,MODEL`, `cat /proc/partitions` | Disk-ներ | 9 |
| `lspci -k` | PCI սարքեր և driver-ներ | 9 |
| `lsusb`, `lsusb -t` | USB սարքեր | 9 |
| `sudo lshw -short`, `sudo dmidecode`, `inxi -F` | Ամբողջ hardware-ը | 9 |
| `sensors` | Ջերմաստիճաններ | 9 |
| `cat /proc/interrupts`, `cat /proc/devices` | Interrupt-ներ, driver-ների major-ները | 9 |
| `sudo mknod f c 1 3` | Ստեղծել սարքի ֆայլ | 9 |
| `udevadm monitor`, `udevadm info -a -n /dev/X` | udev իրադարձություններ, սարքի հատկություններ | 9 |
| `sudo udevadm control --reload && sudo udevadm trigger` | Կիրառել udev կանոնները | 9 |
| `sudo losetup -f --show img` | Ֆայլը որպես block սարք | 9 |
| `lsmod`, `modinfo m`, `sudo modprobe m`, `sudo modprobe -r m` | Kernel module-ներ | 9 |
| `sudo dmesg -w` | Kernel-ի հաղորդագրությունները իրական ժամանակում | 9 |
| `ls /sys/class`, `cat /sys/class/net/eth0/address` | sysfs | 9 |
| `sudo sysctl -w key=value` | Kernel-ի պարամետր | 9 |

## Process-ներ և thread-եր

| Հրաման | Ինչ է անում | Օր |
|---|---|---|
| `ps -o pid,ppid,stat,cmd --forest` | Process-ների ծառը | 10 |
| `ps -eLf`, `ps -o pid,nlwp,cmd -p PID` | Thread-երը (LWP) և նրանց քանակը | 10 |
| `top -H -p PID`, `ls /proc/PID/task` | Process-ի thread-երը | 10 |
| `strace -f -e trace=clone,execve,wait4 cmd` | Ինչ process-ներ է ստեղծում ծրագիրը | 10 |
| `g++ -pthread`, `g++ -fsanitize=thread` | Thread-երով ծրագիր, ThreadSanitizer | 10 |
| `valgrind --tool=helgrind ./prog` | Race condition-ների որոնում | 10 |

## Network

| Հրաման | Ինչ է անում | Օր |
|---|---|---|
| `ip -br addr`, `ip link`, `ip route` | Ինտերֆեյսներ, IP-ներ, gateway | 11 |
| `ping -c 3 host` | Հասանելիություն | 11 |
| `cat /etc/hosts /etc/resolv.conf`, `dig +short name` | DNS | 11 |
| `ss -tlnp`, `ss -tnp`, `ss -tan` | Լսող port-եր, կապեր, վիճակներ (`TIME_WAIT`) | 11 |
| `nc -l 5555`, `nc host 5555`, `nc -u` | TCP/UDP սերվեր և հաճախորդ terminal-ից | 11 |
| `nmap -p 1-1024 host` | Բաց port-եր (միայն քո սարքերի վրա) | 11 |
| `curl -v url` | HTTP հարցում | 11 |
| `sudo tcpdump -i lo port 5555 -A` | Package-ների դիտում | 11 |
