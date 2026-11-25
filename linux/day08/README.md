# Day 8: Processes, Signals, Services and Packages · Օր 8. Process-ներ, signal-ներ, service-ներ և փաթեթներ

## Objectives · Նպատակներ
- Բացատրել, թե ինչ է process-ը OS-ի համար. հասցեային տարածք, PCB, վիճակներ
- Դիտել և կառավարել process-ները. `ps`, `top`, `/proc/PID`, `nice`, job control (`&`, Ctrl+Z, `fg`, `bg`)
- Բացատրել signal-ները, ուղարկել դրանք (`kill`) և մշակել ծրագրում (`sigaction`)
- Ստեղծել և կառավարել systemd service-ներ. `systemctl`, `journalctl`, unit ֆայլեր
- Տեղադրել, թարմացնել, ջնջել և փնտրել փաթեթներ `apt`-ով և `dpkg`-ով

## Key Concepts · Հիմնական գաղափարներ
- Process, program, PID, PPID
- Process Control Block (PCB), հասցեային տարածք. code, data, heap, stack
- Process-ի վիճակներ. R, S, D, T, Z
- Process-ի ստեղծման, ավարտի և կասեցման պատճառներ
- Nice value, priority
- Job control. foreground, background
- Signal, disposition, signal handler, `sig_atomic_t`
- Daemon, systemd, unit, journal
- Package, repository, dependency, `apt`, `dpkg`

## Definitions · Սահմանումներ

*Այսօր ներմուծվող տերմինները։ Նույն տեքստը կա [GLOSSARY.md](../GLOSSARY.md)-ում։*

**Program / process** — **Program**-ը ֆայլ է սկավառակի վրա։ **Process**-ը նրա աշխատող օրինակն է՝ իր հիշողությամբ, բաց ֆայլերով, վիճակով։ 16 օգտատեր, որոնք գործարկել են նույն `vim`-ը, ունեն 16 process, բայց մեկ ծրագրի կոդը հիշողության մեջ կիսվում է նրանց միջև։

**PID / PPID** — Process-ի համարը և նրա ծնողի (parent) համարը։ Բոլոր process-ները ծառ են կազմում, որի արմատը PID 1-ն է (`systemd`)։

**Process Control Block (PCB)** — Kernel-ի կառուցվածքը, որը նկարագրում է process-ը. PID, վիճակ, register-ներ (այդ թվում program counter), հիշողության տվյալներ, բաց ֆայլեր, priority, UID-եր։ Linux-ում այն կոչվում է `task_struct`։

**Address space** — Process-ի վիրտուալ հիշողությունը. **code** (ծրագրի հրամանները), **data** (գլոբալ փոփոխականներ), **heap** (`new`, `malloc`), **stack** (ֆունկցիաների տեղային փոփոխականներ)։ Ամեն process ունի իրենը, և մի process-ը չի կարող կարդալ մյուսինը։

**Process state** — Linux-ում. **R** (running կամ ready, սպասում է CPU-ի), **S** (sleeping, սպասում է իրադարձության, կարելի է ընդհատել), **D** (uninterruptible sleep, սովորաբար սկավառակի I/O), **T** (stopped), **Z** (zombie)։

**Zombie** — Ավարտված process, որի exit status-ը ծնողը դեռ չի կարդացել (`wait()`, Օր 10)։ Այն հիշողություն չի զբաղեցնում, միայն տող process-ների աղյուսակում։

**Nice value** — Process-ի «քաղաքավարությունը»՝ -20-ից 19։ Մեծ արժեքը նշանակում է ցածր priority։ Լռելյայն 0 է. բարձրացնել կարող է ամեն ոք, իջեցնել (բացասական դարձնել)՝ միայն root-ը։

**Job control** — Shell-ի հնարավորությունը կառավարել իր գործարկած process-ները. ֆոնում (`&`), կասեցնել (Ctrl+Z), վերադարձնել առաջին պլան (`fg`) կամ շարունակել ֆոնում (`bg`)։

**Signal** — Kernel-ի կամ այլ process-ի ասինխրոն հաղորդագրություն process-ին. մեկ թիվ, օրինակ `SIGTERM` (15)։ Process-ը այն ստանում է անմիջապես՝ ընթացիկ ֆունկցիայի մեջտեղում։

**Disposition** — Ինչ է անում process-ը signal ստանալիս. լռելյայն գործողությունը (`SIG_DFL`. սովորաբար ավարտ), անտեսել (`SIG_IGN`), կամ կանչել **signal handler** ֆունկցիա։ `SIGKILL`-ի և `SIGSTOP`-ի disposition-ը փոխել հնարավոր չէ։

**`sig_atomic_t`** — Ամբողջ թվի տիպ, որի մեջ գրելը կատարվում է մեկ անբաժանելի հրամանով։ Signal handler-ը պետք է միայն դրոշ դնի այդպիսի `volatile` փոփոխականում, իսկ հիմնական ծրագիրը ստուգում է դրոշը։

**Daemon** — Ֆոնային process, որը կապված չէ terminal-ի հետ և սպասարկում է համակարգը կամ ցանցը. `sshd`, `cron`, `systemd-journald`։ Անունները հաճախ ավարտվում են `d`-ով։

**systemd** — Linux-ի առաջին process-ը (PID 1). գործարկում է բոլոր service-ները boot-ի ժամանակ, վերագործարկում է ձախողվածները, հավաքում է log-երը (**journal**)։

**Unit** — systemd-ի կառավարվող օբյեկտը, նկարագրված տեքստային ֆայլով. `.service` (ծրագիր), `.timer` (ժամանակացույց), `.mount`, `.socket`։

**Package** — Ծրագիր, պատրաստված տեղադրման համար. ֆայլեր, մետատվյալներ (տարբերակ, նկարագրություն) և **dependency**-ների ցանկ (այլ փաթեթներ, որոնք պետք են)։ Debian/Ubuntu-ում՝ `.deb`։

**Repository** — Սերվեր, որտեղ պահվում են փաթեթները և նրանց ցանկը։ `apt update`-ը ներբեռնում է ցանկը, `apt install`-ը՝ փաթեթը և նրա dependency-ները։

## Commands · Հրամաններ

*Այսօրվա հրամանները։ Բոլոր օրերի հրամանների ցանկը [COMMANDS.md](../COMMANDS.md)-ում է։*

```bash
# Process-ներ
ps ; ps aux ; ps -ef --forest ; ps -o pid,ppid,stat,ni,%cpu,%mem,cmd -p $$
pstree -p ; top ; htop
pgrep -a ssh ; pidof sshd
cat /proc/$$/status ; ls -l /proc/$$/fd ; cat /proc/$$/maps
nice -n 10 ./long_job ; renice -n 5 -p 1234

# Job control
sleep 100 &                  # ֆոնում
jobs ; fg %1 ; bg %1         # Ctrl+Z կասեցնում է առաջին պլանի process-ը
nohup ./long_job > out.log 2>&1 &   # չի մեռնի terminal-ը փակելիս
disown %1

# Signal-ներ
kill -l                      # ցանկը
kill 1234                    # SIGTERM. խնդրել ավարտվել
kill -9 1234                 # SIGKILL. ստիպել
kill -HUP 1234 ; kill -USR1 1234 ; kill -STOP 1234 ; kill -CONT 1234
pkill -f myserver ; killall sleep
trap 'echo caught' INT       # bash-ում

# systemd
systemctl status ssh ; systemctl list-units --type=service --state=running
sudo systemctl start|stop|restart|reload nginx
sudo systemctl enable --now myapp ; sudo systemctl disable myapp
systemctl cat ssh ; sudo systemctl edit myapp ; sudo systemctl daemon-reload
journalctl -u ssh -f ; journalctl -b -p err ; journalctl --since "10 min ago"
systemd-analyze blame

# Փաթեթներ
sudo apt update ; apt list --upgradable ; sudo apt upgrade
sudo apt install htop ; sudo apt remove htop ; sudo apt purge htop ; sudo apt autoremove
apt search json ; apt show htop ; apt policy htop
dpkg -l | grep ssh ; dpkg -L htop ; dpkg -S /usr/bin/top
sudo apt install ./package.deb
```

## Functions · Ֆունկցիաներ

*Այսօրվա C ֆունկցիաները։ Ամբողջական ցանկը [API.md](../API.md)-ում է։*

```c
#include <unistd.h>
pid_t getpid(void);                // իմ PID-ը
pid_t getppid(void);               // ծնողի PID-ը
int nice(int inc);                 // ավելացնել nice-ին inc (դրական՝ ցածր priority)
int pause(void);                   // քնել մինչև signal

#include <signal.h>
// Սահմանել signal-ի disposition-ը. act.sa_handler = SIG_DFL / SIG_IGN / ֆունկցիա
int sigaction(int sig, const struct sigaction *act, struct sigaction *old);
int kill(pid_t pid, int sig);      // ուղարկել signal (pid-ին)
int raise(int sig);                // ուղարկել ինքն իրեն
volatile sig_atomic_t flag;        // միակ անվտանգ բանը, որ handler-ը կարող է փոխել

#include <time.h>
// Քնել. signal-ը արթնացնում է շուտ, և ֆունկցիան վերադարձնում է -1 (errno = EINTR)
int nanosleep(const struct timespec *req, struct timespec *rem);
```

---

## 1. Ի՞նչ է process-ը

### Visual
![Process-ի վիճակները. new → ready → running → terminated. running-ից՝ ready (ժամանակը վերջացավ, preemption) կամ waiting (սպասում է I/O կամ իրադարձության). waiting → ready, երբ իրադարձությունը տեղի ունեցավ. Linux-ի տառերը. R ready/running, S և D waiting, T stopped, Z zombie](process_states.svg)

**Program**-ը ֆայլ է, **process**-ը նրա աշխատող օրինակն է։ Process-ը բաղկացած է.

1. **Կոդից**՝ ծրագրի հրամանները (մի քանի process-ի միջև կիսվում է)
2. **Տվյալներից**՝ գլոբալ փոփոխականներ, heap (`new`), stack (տեղային փոփոխականներ)
3. **Կատարման համատեքստից**՝ register-ներ, program counter, stack pointer, բաց ֆայլեր, ընթացիկ թղթապանակ, UID-եր (Օր 7), environment (Օր 2)

Kernel-ը յուրաքանչյուր process-ի մասին պահում է **Process Control Block** (Linux-ում՝ `task_struct`). PID, վիճակ, register-ների պատճեն, հիշողության քարտեզ, բաց ֆայլերի ցանկ, priority, ծախսած ժամանակ։ Երբ kernel-ը CPU-ն տալիս է մեկ այլ process-ի (**context switch**), այն պահում է ընթացիկ register-ները PCB-ում և վերականգնում հաջորդինը։

Process-ի ամեն ինչ երևում է `/proc/PID/`-ում (Օր 3).

```bash
cat /proc/$$/status | head -12     # Name, State, Pid, PPid, Uid, VmRSS (հիշողություն) ...
ls -l /proc/$$/fd                  # բաց ֆայլերը. 0, 1, 2 → /dev/pts/0 (Օր 2)
cat /proc/$$/maps | head           # հասցեային տարածքը. code, heap, stack, գրադարաններ
tr '\0' ' ' < /proc/$$/cmdline     # հրամանի տողը (argv-ն, '\0'-ներով բաժանված)
```

### Վիճակներ

Process-ը իր կյանքի ընթացքում անցնում է վիճակներով.

| Ընդհանուր մոդել | Linux (`ps`-ի STAT) | Իմաստ |
|---|---|---|
| Ready | `R` | Պատրաստ է, սպասում է CPU-ի |
| Running | `R` | Աշխատում է CPU-ի վրա |
| Waiting | `S` | Սպասում է իրադարձության (ստեղնաշար, ցանց, timer). signal-ը կարթնացնի |
| Waiting | `D` | Սպասում է սարքին (սկավառակ). չի ընդհատվում նույնիսկ `SIGKILL`-ով |
| Suspended | `T` | Կանգնեցված է (Ctrl+Z, `SIGSTOP`, debugger) |
| Terminated | `Z` | Ավարտված է, բայց ծնողը դեռ չի կարդացել exit status-ը |

`ps`-ի STAT սյունակում կարող են լինել լրացուցիչ նշաններ. `s` (session leader), `+` (առաջին պլանում), `<` (բարձր priority), `N` (ցածր priority), `l` (multi-threaded)։

### Ինչու են process-ները ստեղծվում, ավարտվում և կասեցվում

**Ստեղծում.** օգտատերը մուտք է գործում (`login` → shell), օգտատերը գործարկում է ծրագիր (shell → `fork` + `exec`, Օր 10), OS-ը ստեղծում է service (systemd → `sshd`), ծրագիրը ստեղծում է օգնականներ զուգահեռության համար։

**Ավարտ.** նորմալ (`exit`, `return` `main`-ից), սխալ ծրագրում (անթույլատրելի հասցե → `SIGSEGV`, բաժանում զրոյի → `SIGFPE`, անթույլատրելի հրաման → `SIGILL`), հիշողության պակաս (OOM killer), ժամանակի սահմանը անցավ, իրավունքների սխալ, ծնողը կամ օպերատորը ավարտեց այն (`kill`)։

**Կասեցում.** օգտատիրոջ խնդրանքով (Ctrl+Z, debugger), ծնողի խնդրանքով, OS-ի պատճառով (հիշողության պակաս. swap), պարբերական աշխատանք (սպասում է հաջորդ ժամանակահատվածին)։

## 2. Process-ների դիտում

```bash
ps                         # իմ terminal-ի process-ները
ps aux                     # բոլորը. USER PID %CPU %MEM VSZ RSS TTY STAT START TIME COMMAND
ps -ef --forest            # ծառի տեսքով
ps -o pid,ppid,stat,ni,cmd -p $$
pstree -p                  # ամբողջ ծառը. systemd(1)─┬─sshd(812)───sshd(1020)───bash(1021)
pgrep -a python            # PID-ներ ըստ անվան
top                        # իրական ժամանակում. P՝ ըստ CPU, M՝ ըստ հիշողության, k՝ kill, q՝ դուրս
```

`top`-ի վերևում **load average**-ն է. R և D վիճակում process-ների միջին քանակը վերջին 1, 5, 15 րոպեում։ Եթե այն մշտապես մեծ է CPU core-երի քանակից (`nproc`), համակարգը ծանրաբեռնված է։

### Priority և nice

Kernel-ը CPU-ի ժամանակը բաշխում է process-ների միջև (**scheduler**)։ Կարելի է խնդրել, որ որոշ process-ներ ստանան ավելի քիչ.

```bash
nice -n 10 sort huge.txt > sorted.txt    # ցածր priority. չի խանգարի մյուսներին
renice -n 15 -p 1234                      # փոխել աշխատող process-ինը
sudo renice -n -5 -p 1234                 # բարձրացնել priority-ն. միայն root
```

Real-time scheduling-ը (`SCHED_FIFO`) և ճշգրիտ ժամանակը [embedded դասընթացի Օր 3](../../embedded/README.md)-ի թեման է։

### Job control

```bash
sleep 300 &                  # [1] 4567. ֆոնում
jobs                         # [1]+ Running   sleep 300 &
fg %1                        # վերադարձնել առաջին պլան
# Ctrl+Z                     # [1]+ Stopped. SIGTSTP
bg %1                        # շարունակել ֆոնում
kill %1                      # job-ը համարով
```

Terminal-ը փակելիս shell-ը իր job-երին ուղարկում է `SIGHUP` («hang up»), և դրանք ավարտվում են։ Որ աշխատանքը շարունակվի. `nohup cmd &`, `disown`, կամ ավելի լավ՝ `tmux`/`screen`, կամ systemd service (§4)։

## 3. Signal-ներ

### Visual
![Signal-ի ճանապարհը. kill -TERM 1234 → kernel-ը նշում է signal-ը որպես սպասող process 1234-ի համար → երբ process-ը հաջորդ անգամ աշխատում է, kernel-ը ընդհատում է նրա կոդը ցանկացած տեղում և կանչում է handler-ը → handler-ը դնում է դրոշ և վերադառնում → ծրագիրը շարունակում է այնտեղից, որտեղ ընդհատվել էր, և ստուգում է դրոշը. SIGKILL-ը և SIGSTOP-ը չեն հասնում handler-ին](signals.svg)

**Signal**-ը կարճ հաղորդագրություն է process-ին՝ ընդամենը թիվ։ Այն ասինխրոն է. process-ը ստանում է այն անմիջապես, ընթացիկ ֆունկցիայի կամ նույնիսկ տողի մեջտեղում։

| Signal | № | Ով է ուղարկում | Լռելյայն գործողություն |
|---|---|---|---|
| `SIGHUP` | 1 | Terminal-ը փակվեց. daemon-ների համար՝ «վերակարդա կարգավորումները» | Ավարտ |
| `SIGINT` | 2 | Ctrl+C | Ավարտ |
| `SIGQUIT` | 3 | Ctrl+\\ | Ավարտ + core dump |
| `SIGKILL` | 9 | `kill -9` | Ավարտ. **չի կարելի որսալ կամ անտեսել** |
| `SIGUSR1`, `SIGUSR2` | 10, 12 | Ծրագրի սեփական հրամանների համար | Ավարտ |
| `SIGSEGV` | 11 | Kernel-ը. անթույլատրելի հասցե (segmentation fault) | Ավարտ + core dump |
| `SIGPIPE` | 13 | Գրել pipe-ի մեջ, որը ոչ ոք չի կարդում (Օր 4) | Ավարտ |
| `SIGTERM` | 15 | `kill`, `systemctl stop`. «խնդրում եմ ավարտվիր» | Ավարտ |
| `SIGCHLD` | 17 | Երեխա process-ը ավարտվեց (Օր 10) | Անտեսել |
| `SIGCONT` | 18 | `fg`, `bg`, `kill -CONT` | Շարունակել |
| `SIGSTOP` | 19 | `kill -STOP`. **չի կարելի որսալ** | Կանգնել |
| `SIGTSTP` | 20 | Ctrl+Z | Կանգնել |

Համարները x86 և ARM Linux-ի համար են. ծրագրում միշտ օգտագործիր անունները։ Լրիվ ցանկը՝ `kill -l` և `man 7 signal`։

**`SIGTERM` և `SIGKILL`**. `SIGTERM`-ը խնդրանք է. ծրագիրը կարող է որսալ այն, պահել տվյալները, փակել ֆայլերը և ավարտվել կոկիկ։ `SIGKILL`-ը kernel-ն է կատարում առանց ծրագրին հարցնելու. ծրագիրը ոչինչ չի հասցնում։ Այդ պատճառով միշտ սկզբում `kill`, և միայն եթե չօգնեց՝ `kill -9`։

### Signal-ի մշակում C/C++-ում

Signal handler-ը կարող է կանչվել ծրագրի **ցանկացած** կետում, նույնիսկ `printf`-ի կամ `malloc`-ի մեջտեղում։ Այդ պատճառով handler-ում թույլատրված է շատ քիչ. ամենաանվտանգը՝ **միայն դրոշ դնել**, իսկ իրական աշխատանքն անել հիմնական ցիկլում.

```cpp
#include <csignal>

volatile sig_atomic_t g_stop = 0;              // միակ բանը, որ handler-ը փոխում է

void on_signal(int) { g_stop = 1; }            // ոչ printf, ոչ new, ոչ mutex

int main()
{
    struct sigaction sa = {};
    sa.sa_handler = on_signal;
    sigaction(SIGTERM, &sa, nullptr);
    sigaction(SIGINT, &sa, nullptr);

    while (!g_stop) {
        do_work();
        sleep(1);                               // signal-ը ընդհատում է sleep-ը շուտ
    }
    save_state();                               // կոկիկ ավարտ
    return 0;
}
```

`signal()` ֆունկցիան էլ կա, բայց նրա վարքը տարբեր համակարգերում տարբեր է. նոր կոդում օգտագործիր `sigaction`-ը։

Bash script-ում նույնը անում է `trap`-ը (Օր 6).

```bash
trap 'echo "saving..."; exit 0' TERM INT
```

## 4. systemd և service-ներ

### Visual
![systemd-ը PID 1-ն է. boot-ի ժամանակ գործարկում է unit-ները ըստ կախվածությունների. systemctl start → ExecStart ծրագիրը, stdout և stderr → journal. systemctl reload → SIGHUP. systemctl stop → SIGTERM, 90 վայրկյան հետո SIGKILL. Restart=on-failure → վերագործարկում ձախողվելիս](systemd_service.svg)

**systemd**-ը առաջին process-ն է (PID 1)։ Այն գործարկում է բոլոր service-ները boot-ի ժամանակ, հետևում է դրանց, վերագործարկում է ձախողվածները և հավաքում է դրանց ելքը **journal**-ում։

```bash
systemctl status ssh                      # վիճակ, PID, վերջին log-ի տողերը
sudo systemctl stop ssh                   # SIGTERM, ապա (եթե չավարտվեց) SIGKILL
sudo systemctl start ssh
sudo systemctl restart ssh
sudo systemctl reload ssh                 # սովորաբար SIGHUP. վերակարդալ կարգավորումները
sudo systemctl enable --now ssh           # գործարկել հիմա և ամեն boot-ի ժամանակ
systemctl list-units --type=service --state=failed
journalctl -u ssh -f                      # service-ի log-ը, իրական ժամանակում
journalctl -b -p err                      # այս boot-ի սխալները
systemd-analyze blame                     # որ service-ն է դանդաղեցնում boot-ը
```

**Քո ծրագիրը որպես service.** Բավական է unit ֆայլ.

```ini
# /etc/systemd/system/day08.service
[Unit]
Description=Day 8 demo service
After=network.target

[Service]
ExecStart=/opt/day08/day08 --config /etc/day08.conf
ExecReload=/bin/kill -HUP $MAINPID
Restart=on-failure
User=student

[Install]
WantedBy=multi-user.target
```

```bash
sudo systemctl daemon-reload              # կարդալ նոր unit ֆայլը
sudo systemctl enable --now day08
journalctl -u day08 -f
```

Ծրագիրը կարիք չունի «դառնալու daemon» (fork, setsid, PID ֆայլ). systemd-ը անում է դա։ Այն պարզապես գրում է stderr (journal-ը կհավաքի), մշակում է `SIGTERM`-ը և `SIGHUP`-ը։ Հենց այդպիսի ծրագիր կգրես լաբորատորիայում։ Պատրաստի unit ֆայլը՝ [`day08.service`](day08.service)։

## 5. Փաթեթներ

Ubuntu-ում և Debian-ում ծրագրերը տեղադրվում են **փաթեթներով** (`.deb`) **repository**-ներից։ `apt`-ը լուծում է **dependency**-ները. եթե `vlc`-ին պետք են 40 գրադարաններ, դրանք կտեղադրվեն ավտոմատ։

```bash
sudo apt update                    # ներբեռնել փաթեթների ցանկը (ոչինչ չի տեղադրում)
apt list --upgradable              # ինչ կարելի է թարմացնել
sudo apt upgrade                   # թարմացնել տեղադրվածները
sudo apt install htop tree         # տեղադրել
sudo apt remove htop               # ջնջել, կարգավորումները մնում են
sudo apt purge htop                # ջնջել կարգավորումների հետ
sudo apt autoremove                # ջնջել այլևս չօգտագործվող dependency-ները
apt search "json parser"           # փնտրել
apt show htop                      # նկարագրություն, տարբերակ, dependency-ներ
apt policy htop                    # որ տարբերակն է տեղադրված և որտեղից
```

Ցածր մակարդակում աշխատում է `dpkg`-ը.

```bash
dpkg -l | grep ssh                 # տեղադրված փաթեթներ
dpkg -L htop                       # ինչ ֆայլեր է տեղադրել փաթեթը
dpkg -S /usr/bin/top               # որ փաթեթին է պատկանում ֆայլը
sudo apt install ./program.deb     # տեղադրել ներբեռնված .deb ֆայլը (dependency-ներով)
```

Repository-ների ցանկը `/etc/apt/sources.list`-ում և `/etc/apt/sources.list.d/`-ում է։ Երրորդ կողմի repository ավելացնելը նշանակում է վստահել նրա փաթեթներին root-ի իրավունքներով. ավելացրու միայն հայտնի աղբյուրներ։

Այլ distribution-ներում նույն գաղափարն է՝ այլ անուններով. `dnf` (Fedora, RHEL), `pacman` (Arch), `apk` (Alpine)։ Կան նաև distribution-ից անկախ ձևաչափեր՝ `snap`, `flatpak`, `AppImage`, իսկ Python-ի համար՝ `pip` `venv`-ում (Օր 1)։

---

## Resources · Աղբյուրներ
- *Advanced Linux Programming*, Chapter 3 «Processes» (3.1–3.3) և Appendix C «Table of Signals». [`docs/linux_book.pdf`](../docs/linux_book.pdf)
- [Operating Systems: Processes](http://www.eecg.toronto.edu/~jacobsen/os/2007s/process.pdf), ECE 344, University of Toronto. process-ի կառուցվածքը, PCB, վիճակներ, հերթեր
- [Process description and control: tables](https://www.cs.helsinki.fi/u/kerola/kj2/stal-01_slides_figs/T3-Vertical.pdf), Stallings, University of Helsinki. ստեղծման, ավարտի և կասեցման պատճառները, PCB-ի տարրերը
- [`signal(7)`](https://man7.org/linux/man-pages/man7/signal.7.html), [`sigaction(2)`](https://man7.org/linux/man-pages/man2/sigaction.2.html), [`proc(5)`](https://man7.org/linux/man-pages/man5/proc.5.html), [`ps(1)`](https://man7.org/linux/man-pages/man1/ps.1.html), [`systemd.service(5)`](https://www.freedesktop.org/software/systemd/man/latest/systemd.service.html), [`apt(8)`](https://manpages.ubuntu.com/manpages/noble/man8/apt.8.html)
- Հին դասի հղումներ, այժմ հասանելի չեն. [OS Chp4 Processes](http://www.just.edu.jo/~basel/os/os_slides/OS%20Chp4%20Processes.pdf) (JUST), [INF583](http://www.enseignement.polytechnique.fr/informatique/INF583/INF583_5.pdf) (École Polytechnique)

## Hands-On Task · Գործնական առաջադրանք

Գրիր փոքր service, որը կոկիկ է ավարտվում, վերակարդում է կարգավորումները և պատմում է իր վիճակը signal-ների միջոցով. [`template.cpp`](template.cpp)։

```bash
printf 'interval_ms=500\nmessage=working\n' > day08.conf
./day08 --config day08.conf &        # stderr-ում ամեն 0.5 վ. "[12:00:01] working (tick 1)"
kill -USR1 $!                        # "[...] stats: 7 ticks, 3.5 s, interval 500 ms"
sed -i 's/500/200/' day08.conf ; kill -HUP $!     # "[...] config reloaded: interval 200 ms"
kill $!                              # "[...] SIGTERM: stopped cleanly after 25 ticks"
wait $! ; echo $?                    # 0
```

Լրացրու TODO-ները. handler-ը (միայն դրոշներ), `sigaction`-ով տեղադրումը, կարգավորումների ֆայլի ընթերցումը, հիմնական ցիկլի արձագանքը դրոշներին։ Հետո գործարկիր որպես իսկական service [`day08.service`](day08.service)-ով և փորձիր `systemctl status`, `reload`, `stop`, `journalctl -u day08`։

## Self-Learning · Ինքնուրույն աշխատանք
1. Գտիր քո shell-ի բոլոր նախնիները մինչև PID 1-ը՝ `/proc/PID/status`-ի `PPid` դաշտով (bash ցիկլ)։ Համեմատիր `pstree -s $$`-ի հետ։
2. Ստեղծիր zombie. `bash -c 'sleep 1 & exec sleep 30'`։ Գտիր այն `ps -o pid,ppid,stat,cmd`-ով։ Ինչու է այն հայտնվում և անհետանում։
3. Գործարկիր երկու `yes > /dev/null &`, մեկը `nice -n 19`-ով։ `top`-ով համեմատիր, թե որքան CPU է ստանում յուրաքանչյուրը մեկ core-ի վրա (`taskset -c 0`)։
4. Գրիր bash script, որը `trap`-ով որսում է `INT`-ը և `TERM`-ը և ավարտվելուց առաջ ջնջում է իր ժամանակավոր ֆայլը (Օր 6)։ Ստուգիր Ctrl+C-ով և `kill`-ով։ Ինչ է պատահում `kill -9`-ի դեպքում։
5. Template-ում handler-ի մեջ ավելացրու `printf`։ Ինչու է սա վտանգավոր, եթե ծրագիրը շատ է տպում, և signal-ները հաճախ են գալիս։ Կարդա `man 7 signal-safety`։
6. Գրիր systemd **timer**, որը ամեն 5 րոպեն մեկ գործարկում է Օր 5-ի backup script-ը։ Ստուգիր `systemctl list-timers`-ով։
7. Template-ի service-ում դիտավորյալ սխալ ավելացրու (`abort()` 10 tick-ից հետո)։ Ինչ է անում `Restart=on-failure`-ը։ Գտիր սա `journalctl`-ում։
8. Գտիր, թե որ փաթեթն է տեղադրել `/usr/bin/ssh`-ը, ինչ dependency-ներ ունի այն (`apt-cache depends`), և ինչ ֆայլեր է տեղադրել։

## Self-Check · Ինքնաստուգում
Պատասխաններ չկան։ Մտածիր ինքնուրույն։

1. Ինչ տարբերություն կա program-ի և process-ի միջև։ Ինչ է կիսվում նույն ծրագրի երկու process-ների միջև, ինչը՝ ոչ։
2. Ինչ է պահվում PCB-ում, և երբ է kernel-ը օգտագործում այն։
3. Process-ը `D` վիճակում է և չի ավարտվում նույնիսկ `kill -9`-ից։ Ինչու։
4. Ինչ է zombie-ն։ Ինչու չի կարելի այն «սպանել», և ինչպես է այն անհետանում։
5. Ինչ տարբերություն կա `SIGTERM`-ի և `SIGKILL`-ի միջև։ Ինչու է `systemctl stop`-ը սկզբում ուղարկում առաջինը։
6. Ինչու է handler-ում `printf` կանչելը վտանգավոր։ Ինչու դրոշը պետք է լինի `volatile sig_atomic_t`։
7. Ինչու է template-ի ցիկլը արձագանքում `kill`-ին անմիջապես, թեև նա քնած է `nanosleep`-ում։
8. Terminal-ը փակելուց հետո `./server &`-ը ավարտվեց։ Ինչու, և ինչ տարբերակներ ունես, որ այն շարունակի աշխատել։
9. Ինչ տարբերություն կա `apt remove`-ի և `apt purge`-ի միջև։ Ինչ է անում `apt update`-ը և ինչ՝ `apt upgrade`-ը։
10. Ինչու չպետք է service-ը աշխատի root-ով, եթե դա պետք չէ (Օր 7)։

## Code Template · Կոդի ձևանմուշ
Տե՛ս [`template.cpp`](template.cpp)-ը և [`day08.service`](day08.service) unit ֆայլը։

```bash
g++ -std=c++17 -Wall -Wextra -O2 template.cpp -o day08
printf 'interval_ms=500\nmessage=working\n' > day08.conf && ./day08 --config day08.conf
```
