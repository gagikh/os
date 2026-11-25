# Commands · Հրամանների ցանկ

Դասընթացում օգտագործվող հրամանները՝ խմբերով։ Վերջին սյունակը ցույց է տալիս այն օրը, երբ հրամանը ներմուծվում է։ Ամեն հրամանի մասին ավելին՝ `man <հրաման>` կամ `<հրաման> --help`։

Տերմինների համար տե՛ս [GLOSSARY.md](GLOSSARY.md), C/C++ ֆունկցիաների համար՝ [API.md](API.md)։

## Միանալ բորդին

| Հրաման | Ինչ է անում | Օր |
|---|---|---|
| `ssh lab@192.168.1.50` | Shell բորդի վրա network-ով | 1 |
| `ssh-keygen -t ed25519` | Ստեղծել SSH key PC-ի վրա (մեկ անգամ) | 1 |
| `ssh-copy-id lab@192.168.1.50` | Մուտք առանց password-ի | 1 |
| `ssh-keygen -R 192.168.1.50` | Մոռանալ բորդի հին մատնահետքը (վերատեղադրումից հետո) | 1 |
| `scp file lab@IP:~/dir/` | Ֆայլը PC-ից բորդ | 1 |
| `scp lab@IP:~/file .` | Ֆայլը բորդից PC | 1 |
| `rsync -av dir/ lab@IP:~/dir/` | Directory-ն, միայն փոխված ֆայլերը | 1 |
| `picocom -b 115200 /dev/ttyUSB0` | Serial console (դուրս գալ՝ Ctrl+A, Ctrl+X) | 1 |
| `exit` կամ Ctrl+D | Փակել shell-ը | 1 |

## Օգնություն

| Հրաման | Ինչ է անում | Օր |
|---|---|---|
| `man ls` | Ամբողջական ձեռնարկ (Space՝ էջ, `/բառ`՝ փնտրել, `q`՝ դուրս) | 1 |
| `ls --help` | Կարճ օգնություն | 1 |
| `history` | Նախորդ հրամանները | 1 |

## Ֆայլեր և directory-ներ

| Հրաման | Ինչ է անում | Օր |
|---|---|---|
| `pwd` | Ընթացիկ directory-ն | 1 |
| `ls -la` | Պարունակությունը, մանրամասն, նաև թաքնված ֆայլերը | 1 |
| `cd dir`, `cd ..`, `cd ~`, `cd -` | Գնալ directory / վերև / home / նախորդ | 1 |
| `mkdir -p a/b/c` | Ստեղծել directory՝ parent-ներով | 1 |
| `touch f.txt` | Ստեղծել դատարկ ֆայլ | 1 |
| `cp a b`, `cp -r d1 d2` | Պատճենել ֆայլ / directory | 1 |
| `mv a b` | Տեղափոխել կամ վերանվանել | 1 |
| `rm f`, `rm -r dir` | Ջնջել (վերջնական, աղբարկղ չկա) | 1 |
| `find ~ -name "*.c"` | Փնտրել ֆայլ ըստ անվան | 1 |
| `file prog` | Ֆայլի տեսակը, binary-ի architecture-ը | 1 |

## Տեքստ

| Հրաման | Ինչ է անում | Օր |
|---|---|---|
| `cat f` | Տպել ամբողջ ֆայլը | 1 |
| `less f` | Կարդալ էջ առ էջ | 1 |
| `head -n 5 f`, `tail -n 5 f` | Առաջին / վերջին 5 տողը | 1 |
| `tail -f f` | Հետևել ֆայլի նոր տողերին | 1 |
| `nano f` | Խմբագրել (Ctrl+O՝ պահել, Ctrl+X՝ դուրս) | 1 |
| `echo "x" > f`, `echo "x" >> f` | Գրել ֆայլ / ավելացնել վերջում | 1 |
| `cmd 2> /dev/null` | Թաքցնել սխալները | 1 |
| `cmd1 \| cmd2` | Pipe. առաջինի ելքը՝ երկրորդի մուտք | 1 |
| `grep -i word f` | Տողեր, որոնք պարունակում են բառը | 1 |
| `cut -d= -f2` | Տողի երկրորդ դաշտը `=`-ով բաժանված | 1 |

## User-ներ և permission-ներ

| Հրաման | Ինչ է անում | Օր |
|---|---|---|
| `whoami`, `id`, `groups` | Ով եմ, որ group-երում եմ | 1 |
| `sudo cmd` | Կատարել `root`-ի permission-ներով | 1 |
| `chmod +x f`, `chmod 750 f` | Փոխել permission-ները | 1 |
| `sudo usermod -aG dialout,i2c $USER` | Ավելանալ group-ին (հետո նորից մուտք գործել) | 1 |

## Process-ներ

| Հրաման | Ինչ է անում | Օր |
|---|---|---|
| `ps aux` | Բոլոր process-ները | 1 |
| `top`, `htop` | Process-ները իրական ժամանակում | 1 |
| `cmd &`, `jobs` | Գործարկել ֆոնում / ֆոնի աշխատանքները | 1 |
| `kill PID`, `kill -9 PID` | Կանգնեցնել / ստիպել կանգնել | 1 |
| `pkill -f name` | Կանգնեցնել ըստ անվան | 1 |
| `echo $?` | Վերջին հրամանի exit status-ը | 1 |
| `tegrastats` | Jetson. CPU, GPU, ջերմաստիճան, հզորություն | 1 |
| `vcgencmd measure_temp` | Raspberry Pi. ջերմաստիճան | 1 |

## Package-ներ

| Հրաման | Ինչ է անում | Օր |
|---|---|---|
| `sudo apt update` | Թարմացնել package-ների ցանկը | 1 |
| `apt search name` | Փնտրել package | 1 |
| `sudo apt install -y name` | Տեղադրել | 1 |
| `python3 -m venv ~/venv` | Ստեղծել Python-ի վիրտուալ environment | 1 |
| `source ~/venv/bin/activate` | Միացնել այն | 1 |

## Համակարգի տեղեկություն

| Հրաման | Ինչ է անում | Օր |
|---|---|---|
| `uname -a`, `uname -r`, `uname -m` | Kernel-ը, տարբերակը, architecture-ը | 1 |
| `cat /etc/os-release` | Distribution-ը | 1 |
| `tr -d '\0' < /proc/device-tree/model` | Բորդի անունը | 1 |
| `nproc`, `lscpu` | CPU core-եր | 1 |
| `free -h` | Memory | 1 |
| `df -h`, `lsblk` | Disk-ներ | 1 |
| `ip a`, `hostname -I` | Network ինտերֆեյսներ, IP հասցե | 1 |
| `uptime -p` | Որքան ժամանակ է աշխատում | 1 |
| `dmesg \| tail` | Kernel-ի վերջին հաղորդագրությունները | 1 |
| `cat /sys/class/thermal/thermal_zone0/temp` | CPU-ի ջերմաստիճանը (millidegree) | 1 |
| `sudo reboot`, `sudo poweroff` | Վերագործարկել / անջատել | 1 |

## Հավաքում

| Հրաման | Ինչ է անում | Օր |
|---|---|---|
| `gcc -Wall -O2 a.c -o a` | Հավաքել C | 1 |
| `g++ -std=c++17 -Wall -O2 a.cpp -o a` | Հավաքել C++ | 1 |
| `aarch64-linux-gnu-g++ ...` | Cross build PC-ի վրա բորդի համար | 1 |
| `qemu-aarch64 -L /usr/aarch64-linux-gnu ./a` | Գործարկել aarch64 binary-ն PC-ի վրա | 1 |
| `cmake -S . -B build && cmake --build build` | Հավաքել CMake-ով | 1 |
| `python3 a.py` | Գործարկել Python ծրագիր | 1 |
