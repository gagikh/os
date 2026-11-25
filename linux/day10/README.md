# Day 10: Processes and Threads · Օր 10. Process-ներ և thread-եր

## Objectives · Նպատակներ
- Ստեղծել process-ներ `fork`-ով, գործարկել այլ ծրագիր `exec`-ով, սպասել և կարդալ exit status-ը `waitpid`-ով
- Բացատրել zombie-ները և orphan-ները, մաքրել զավակներին `SIGCHLD`-ով կամ `WNOHANG`-ով
- Միացնել երկու process pipe-ով (`pipe`, `dup2`), ինչպես shell-ը անում է `a | b`-ի համար
- Ստեղծել thread-եր (`pthread_create`, `std::thread`) և սպասել նրանց
- Ճանաչել race condition-ը և պաշտպանել critical section-ը mutex-ով
- Բացատրել process-ների և thread-երի տարբերությունը և երբ որն ընտրել

## Key Concepts · Հիմնական գաղափարներ
- `fork`, `exec*`, `wait`/`waitpid`, `exit` / `_exit`
- Exit status macros. `WIFEXITED`, `WEXITSTATUS`, `WIFSIGNALED`
- Zombie, orphan, `SIGCHLD`
- `pipe`, `dup2`. ինչպես են աշխատում pipe-երը և redirection-ը
- Thread, `pthread_create`, `pthread_join`, `std::thread`
- Race condition, critical section, mutex, deadlock, condition variable
- `clone`. thread-ը Linux-ում process է, որը կիսում է հիշողությունը

## Definitions · Սահմանումներ

*Այսօր ներմուծվող տերմինները։ Նույն տեքստը կա [GLOSSARY.md](../GLOSSARY.md)-ում։*

**`fork`** — System call, որը ստեղծում է ընթացիկ process-ի **պատճենը**։ Երկուսն էլ շարունակում են նույն տեղից. ծնողում `fork`-ը վերադարձնում է երեխայի PID-ը, երեխայում՝ 0։

**`exec`** — System call-երի ընտանիք (`execvp`, `execl` ...), որը ընթացիկ process-ի ծրագիրը **փոխարինում** է նորով։ PID-ը, բաց ֆայլերը և environment-ը մնում են, կոդը և հիշողությունը՝ նոր։ Հաջողության դեպքում `exec`-ը չի վերադառնում։

**`waitpid`** — Սպասել երեխա process-ի ավարտին և ստանալ նրա **status**-ը։ Միաժամանակ մաքրում է zombie-ն։

**Orphan** — Process, որի ծնողը ավարտվել է։ Նրան «որդեգրում» է PID 1-ը (կամ subreaper-ը), որը կմաքրի այն ավարտվելիս։

**Copy-on-write** — `fork`-ը իրականում չի պատճենում հիշողությունը. ծնողը և երեխան կիսում են նույն էջերը, մինչև նրանցից մեկը փոխի էջը. միայն այդ պահին էջը պատճենվում է։ Այդ պատճառով `fork`-ը արագ է նույնիսկ մեծ process-ների համար։

**`dup2`** — Պատճենել file descriptor-ը որոշակի համարի վրա. `dup2(fd, 1)`-ից հետո stdout-ը գնում է այնտեղ, ուր `fd`-ն։ Այսպես է shell-ը իրականացնում `>`-ը և `|`-ը (Օր 2, 4)։

**Thread** — Կատարման հոսք process-ի ներսում։ Մեկ process-ի thread-երը կիսում են հիշողությունը, բաց ֆայլերը և այլ ռեսուրսները, բայց ամեն մեկն ունի իր stack-ը և register-ները։

**Race condition** — Սխալ, որի արդյունքը կախված է նրանից, թե thread-երը (կամ process-ները) ինչ հերթականությամբ են աշխատում։ Օրինակ՝ երկու thread միաժամանակ անում են `counter++`. գործողությունը երեք քայլ է (կարդալ, ավելացնել, գրել), և մեկ ավելացումը կորչում է։

**Critical section** — Կոդի հատված, որը միաժամանակ պետք է կատարի միայն մեկ thread, քանի որ այն աշխատում է ընդհանուր տվյալների հետ։

**Mutex (mutual exclusion)** — Կողպեք critical section-ի համար. `lock()`-ը սպասում է, մինչև կողպեքը ազատվի, `unlock()`-ը ազատում է այն։ Միաժամանակ այն պահում է միայն մեկ thread։

**Deadlock** — Վիճակ, երբ երկու կամ ավելի thread սպասում են միմյանց կողպեքներին, և ոչ մեկը չի կարող շարունակել։ Դասական օրինակը՝ «ճաշող փիլիսոփաները»։

**Condition variable** — Մեխանիզմ, որով thread-ը քնում է, մինչև մեկ այլ thread ազդարարի, որ պայմանը փոխվել է (օրինակ՝ հերթում տվյալներ հայտնվեցին)։ Միշտ օգտագործվում է mutex-ի հետ։

**`clone`** — Linux-ի system call, որը ստեղծում է նոր process՝ ընտրելով, թե ինչ է կիսում ծնողի հետ (հիշողություն, ֆայլեր, signal handler-ներ)։ `fork`-ը և `pthread_create`-ը երկուսն էլ իրականացված են `clone`-ով։

---

## 1. `fork`, `exec`, `wait`

### Visual
![fork-ը ստեղծում է process-ի պատճեն. ծնողում fork-ը վերադարձնում է երեխայի PID-ը, երեխայում՝ 0. երեխան կանչում է execvp, և նրա ծրագիրը փոխարինվում է ls-ով (PID-ը նույնն է). ծնողը կանչում է waitpid և ստանում exit status-ը. եթե ծնողը չի կանչում waitpid, ավարտված երեխան դառնում է zombie](fork_exec.svg)

Windows-ում նոր ծրագիր գործարկվում է մեկ ֆունկցիայով (`CreateProcess`)։ Unix-ում դա երկու քայլ է.

1. **`fork`**-ը ստեղծում է ընթացիկ process-ի ճշգրիտ պատճենը։ Երկու process-ն էլ շարունակում են `fork`-ից հետո։ Ինչպես տարբերել. `fork`-ը ծնողին վերադարձնում է երեխայի PID-ը, իսկ երեխային՝ 0։
2. **`exec`**-ը երեխայի ծրագիրը փոխարինում է նորով։

```cpp
#include <sys/wait.h>
#include <unistd.h>
#include <cstdio>

int main()
{
    auto const pid = fork();
    if (-1 == pid) { std::perror("fork"); return 1; }

    if (0 == pid) {                                   // երեխա
        char *argv[] = {(char *)"ls", (char *)"-l", (char *)"/", nullptr};
        execvp("ls", argv);                           // հաջողության դեպքում չի վերադառնում
        std::perror("execvp");                        // եթե այստեղ ենք, exec-ը ձախողվեց
        _exit(127);                                   // «command not found», ինչպես shell-ում (Օր 2)
    }

    auto status = 0;                                  // ծնող
    waitpid(pid, &status, 0);
    if (WIFEXITED(status))
        std::printf("child %d exited with %d\n", pid, WEXITSTATUS(status));
    else if (WIFSIGNALED(status))
        std::printf("child %d killed by signal %d\n", pid, WTERMSIG(status));
}
```

Ինչու է երկու քայլը օգտակար. `fork`-ի և `exec`-ի **միջև** երեխան կարող է պատրաստել միջավայրը նոր ծրագրի համար՝ փոխել թղթապանակը, environment-ը (Օր 2), ուղղորդել file descriptor-ները (§3), իջեցնել իրավունքները (Օր 7)։ Նոր ծրագիրը կժառանգի այդ ամենը։

`fork`-ը արագ է նույնիսկ մեծ process-ների համար **copy-on-write**-ի շնորհիվ. kernel-ը չի պատճենում հիշողությունը, այլ երկու process-ին էլ տալիս է նույն էջերը «միայն կարդալու» համար, և էջը պատճենվում է միայն այն պահին, երբ նրանցից մեկը փորձում է փոխել այն։

**Կարևոր թակարդ**. `fork`-ից առաջ `printf`-ով տպված, բայց դեռ buffer-ում մնացած տեքստը (Օր 2) պատճենվում է երկու process-ների մեջ և կտպվի **երկու անգամ**։ Լուծումը՝ `fflush(stdout)` `fork`-ից առաջ, և երեխայում `_exit`, ոչ թե `exit`։

### Exit status-ը և `$?`

`waitpid`-ի `status`-ը թիվ է, որի մեջ կոդավորված է, թե ինչպես է ավարտվել երեխան։ Shell-ը նույն տեղեկությունից է կազմում `$?`-ը (Օր 2).

| Ինչպես ավարտվեց | Macro | Shell-ի `$?` |
|---|---|---|
| `exit(3)` կամ `return 3` | `WIFEXITED`, `WEXITSTATUS` = 3 | 3 |
| Սպանվեց `SIGKILL`-ով (9) | `WIFSIGNALED`, `WTERMSIG` = 9 | 137 = 128 + 9 |
| `exec`-ը ձախողվեց | `WEXITSTATUS` = 127 (մեր ընտրությամբ) | 127 |

### Zombie-ներ և orphan-ներ

Ավարտված երեխան մնում է **zombie** (`Z`, Օր 8), մինչև ծնողը կարդա նրա status-ը `wait`-ով։ Եթե ծնողը երբեք չի կանչում `wait`, zombie-ները կուտակվում են։ Երկու լուծում.

```cpp
while (0 < waitpid(-1, &status, WNOHANG)) { /* մաքրել բոլոր ավարտվածներին, առանց սպասելու */ }
```

կամ `SIGCHLD`-ի handler, որը նույնն է անում (Օր 8)։

Եթե **ծնողն** է ավարտվում առաջինը, երեխան դառնում է **orphan**, և նրան որդեգրում է PID 1-ը (`systemd`), որը կմաքրի նրան։

## 2. Ինչ է անում shell-ը

Օր 1-ի նկարը (shell-ի ցիկլը) հիմա կարող ենք կարդալ ամբողջությամբ. shell-ը մի ցիկլ է.

```
while (կարդալ տող):
    բաժանել բառերի               (Օր 2)
    եթե built-in է (cd, exit)  → կատարել ինքը
    pid = fork()
    երեխա. execvp(բառեր)        → եթե չստացվեց՝ _exit(127)
    ծնող. եթե & չկա՝ waitpid(pid) → $?
```

Լաբորատորիայում հենց այս ցիկլը կգրես։ Տես, թե ինչ է անում իրական `bash`-ը.

```bash
strace -f -e trace=clone,clone3,execve,wait4,pipe2,dup2 bash -c 'ls / | wc -l'
```

## 3. Pipe-եր և redirection ներսից

Օր 4-ում տեսանք, որ `a | b`-ն երկու միաժամանակ աշխատող process է։ Ահա թե ինչպես է shell-ը միացնում դրանք.

```cpp
int fds[2];
pipe(fds);                                   // fds[0]՝ կարդալու, fds[1]՝ գրելու ծայր

if (0 == fork()) {                           // առաջին երեխա. a
    dup2(fds[1], STDOUT_FILENO);             // stdout-ը → pipe
    close(fds[0]); close(fds[1]);
    execvp(a[0], a);
    _exit(127);
}
if (0 == fork()) {                           // երկրորդ երեխա. b
    dup2(fds[0], STDIN_FILENO);              // stdin-ը ← pipe
    close(fds[0]); close(fds[1]);
    execvp(b[0], b);
    _exit(127);
}
close(fds[0]); close(fds[1]);                // ծնողը պետք է փակի երկու ծայրն էլ
waitpid(-1, nullptr, 0); waitpid(-1, nullptr, 0);
```

Եթե ծնողը չփակի `fds[1]`-ը, `b`-ն երբեք չի ստանա EOF (pipe-ի գրելու ծայրը դեռ բաց է), և pipeline-ը կկախվի։ Սա pipe-երով աշխատելու ամենատարածված սխալն է։

`cmd > file`-ը նույն ձևով է. երեխան `exec`-ից առաջ անում է `open("file", O_WRONLY | O_CREAT | O_TRUNC, 0644)` և `dup2(fd, 1)`։

## 4. Thread-եր

### Visual
![Երկու process ունեն առանձին հիշողություն, իսկ մեկ process-ի երկու thread կիսում են code, data, heap, բաց ֆայլերը, բայց ամեն մեկն ունի իր stack-ը և register-ները. ներքևում race condition-ը. երկու thread կարդում են counter = 5, երկուսն էլ ավելացնում են, երկուսն էլ գրում են 6, մեկ ավելացում կորավ. mutex-ով երկրորդը սպասում է](threads_race.svg)

**Thread**-ը կատարման հոսք է process-ի ներսում։ Thread-երը կիսում են process-ի ամբողջ հիշողությունը (գլոբալ փոփոխականներ, heap) և բաց ֆայլերը, բայց ամեն մեկն ունի իր **stack**-ը և **register**-ները։

```cpp
#include <iostream>
#include <thread>
#include <vector>

void work(int id) { std::cout << "thread " << id << '\n'; }

int main()
{
    auto threads = std::vector<std::thread>{};
    for (auto i = 0; i < 4; ++i) threads.emplace_back(work, i);
    for (auto &t : threads) t.join();       // սպասել բոլորին. առանց join-ի ծրագիրը կավարտվի std::terminate-ով
}
```

```bash
g++ -std=c++17 -pthread threads.cpp -o threads && ./threads
```

Տողերը կտպվեն ամեն անգամ տարբեր հերթականությամբ, երբեմն նույնիսկ խառնված. thread-երը աշխատում են միաժամանակ, և ոչ ոք չի խոստանում հերթականություն։

C-ում նույնը անում է POSIX threads գրադարանը (`pthread_create`, `pthread_join`), որի վրա կառուցված է `std::thread`-ը։

### Linux-ում thread-ը process է

Linux kernel-ի համար thread-ը առանձին տեսակ չէ. և՛ `fork`-ը, և՛ `pthread_create`-ը կանչում են **`clone`** system call-ը, պարզապես տարբեր դրոշներով. thread-ի դեպքում նոր «process»-ը կիսում է ծնողի հիշողությունը (`CLONE_VM`), ֆայլերը (`CLONE_FILES`), signal handler-ները։ Ամեն thread ունի իր ID-ն (TID), որը երևում է `ps -eLf`-ում և `/proc/PID/task/`-ում։

Տես ինքդ.

```bash
ps -eLf | head               # LWP սյունակը thread-ի ID-ն է, NLWP-ն՝ thread-ների քանակը process-ում
top -H -p PID                # -H. ցույց տալ thread-երը առանձին տողերով
ls /proc/PID/task            # ամեն thread-ի համար թղթապանակ
cat /proc/sys/kernel/threads-max ; ulimit -u    # քանի thread/process է թույլատրված համակարգում և քեզ
```

## 5. Race condition և mutex

```cpp
long counter = 0;

void add(int n) { for (auto i = 0; i < n; ++i) ++counter; }

int main()
{
    std::thread a(add, 1000000), b(add, 1000000);
    a.join(); b.join();
    std::cout << counter << '\n';     // սպասում ենք 2000000, ստանում ենք 1134072, 1287531, ...
}
```

`++counter`-ը մեկ հրաման չէ, այլ երեք. **կարդալ** հիշողությունից register, **ավելացնել**, **գրել** հետ։ Եթե երկու thread-ը կարդում են նույն արժեքը մինչև մյուսի գրելը, մեկ ավելացումը կորչում է։ Սա **race condition** է. արդյունքը կախված է պատահական հերթականությունից, ամեն անգամ տարբեր է, և սխալը կարող է չերևալ փորձարկման ժամանակ։

Լուծումը՝ **mutex**-ով պաշտպանված **critical section**.

```cpp
std::mutex m;

void add(int n)
{
    for (auto i = 0; i < n; ++i) {
        std::lock_guard<std::mutex> lock(m);   // lock. unlock-ը ավտոմատ է scope-ի վերջում
        ++counter;
    }
}
```

Հիմա արդյունքը միշտ 2000000 է, բայց ծրագիրը շատ ավելի դանդաղ է. thread-երը մշտապես սպասում են միմյանց։ Ավելի լավ է՝ յուրաքանչյուր thread հաշվի իր տեղային գումարը և միայն վերջում ավելացնի ընդհանուրին (մեկ lock), կամ պարզ դեպքերի համար՝ `std::atomic<long>`։

Race condition-ներ գտնելու գործիքներ. `g++ -fsanitize=thread` (ThreadSanitizer) և `valgrind --tool=helgrind`։

### Deadlock և ճաշող փիլիսոփաները

Եթե thread A-ն վերցրել է mutex 1-ը և սպասում է 2-ին, իսկ B-ն վերցրել է 2-ը և սպասում է 1-ին, երկուսն էլ կսպասեն հավերժ (**deadlock**)։ Դասական խնդիրը՝ հինգ փիլիսոփա կլոր սեղանի շուրջ, հինգ պատառաքաղ, ամեն մեկին պետք է երկուսը։ Եթե բոլորը միաժամանակ վերցնեն ձախը, ոչ ոք չի կարող վերցնել աջը։ (Windows դասընթացում այն արդեն իրականացրել ենք. [`windows/day2`](../../windows/day2/), [`windows/day3`](../../windows/day3/)։)

Լուծումներ. միշտ վերցնել կողպեքները **նույն հերթականությամբ** (օրինակ՝ նախ փոքր համարով պատառաքաղը), կամ `std::scoped_lock(m1, m2)`, որը վերցնում է մի քանի mutex առանց deadlock-ի։

### Condition variable

Երբ thread-ը պետք է սպասի, մինչև ինչ-որ բան տեղի ունենա (օրինակ՝ հերթում աշխատանք հայտնվի), ցիկլով ստուգելը վատնում է CPU-ն։ **Condition variable**-ը թույլ է տալիս քնել մինչև ազդանշանը.

```cpp
std::mutex m;
std::condition_variable cv;
std::queue<int> jobs;

// աշխատող
std::unique_lock<std::mutex> lock(m);
cv.wait(lock, [] { return !jobs.empty(); });   // քնում է, mutex-ը ազատված է
auto job = jobs.front(); jobs.pop();

// արտադրող
{ std::lock_guard<std::mutex> lock(m); jobs.push(42); }
cv.notify_one();
```

## 6. Process, թե thread

| | Process-ներ | Thread-եր |
|---|---|---|
| Հիշողություն | Առանձին. մեկի սխալը չի վնասում մյուսին | Ընդհանուր. մեկ thread-ի սխալը ավարտում է ամբողջ process-ը |
| Տվյալների փոխանակում | Pipe, socket, shared memory, ֆայլեր | Ուղիղ, ընդհանուր փոփոխականներով (mutex-ով) |
| Ստեղծման արժեք | Ավելի մեծ | Ավելի փոքր |
| Անվտանգություն | Կարելի է տալ տարբեր իրավունքներ (Օր 7) | Նույն իրավունքները |
| Տարբեր ծրագրեր | Այո (`exec`) | Ոչ |
| Օրինակներ | Shell, Chrome-ի tab-եր, `sshd`, web server-ի worker-ներ | Խաղի շարժիչ, պատկերի մշակում, GUI + ֆոնային աշխատանք |

Կանոն. եթե առաջադրանքները անկախ են կամ պետք է մեկուսացված լինեն՝ process-ներ, եթե պետք է կիսել շատ տվյալներ և արագ՝ thread-եր։

---

## Commands · Հրամաններ

*Ամփոփում. այս օրվա հրամանները, որոնք բացատրված են վերևում։ Անգիր անել պետք չէ. վերադարձիր այստեղ, երբ մոռանաս գրելաձևը։ Բոլոր օրերի հրամանների ցանկը [COMMANDS.md](../COMMANDS.md)-ում է։*

```bash
ps -o pid,ppid,stat,cmd --forest          # ծառը. ծնող-երեխա
ps -eLf | head ; ps -o pid,nlwp,cmd -p PID # thread-երը. LWP, NLWP՝ thread-ների քանակը
top -H -p PID                              # process-ի thread-երը top-ում
ls /proc/PID/task                          # ամեն thread-ի համար թղթապանակ
strace -f -e trace=clone,clone3,execve,wait4 bash -c 'ls | wc -l'   # ինչ է անում shell-ը
cat /proc/sys/kernel/threads-max ; ulimit -u
g++ -std=c++17 -pthread prog.cpp -o prog   # thread-երով ծրագիր
valgrind --tool=helgrind ./prog            # գտնել race condition-ները
g++ -fsanitize=thread -g prog.cpp -o prog  # ThreadSanitizer
```

## Functions · Ֆունկցիաներ

*Ամփոփում. այս օրվա C/C++ ֆունկցիաները։ Ամբողջական ցանկը [API.md](../API.md)-ում է։*

```c
#include <unistd.h>
pid_t fork(void);                         // ծնողում՝ երեխայի PID, երեխայում՝ 0, սխալի դեպքում՝ -1
int execvp(const char *file, char *const argv[]);   // փնտրում է PATH-ում (Օր 2). argv-ի վերջում NULL
void _exit(int status);                   // ավարտ առանց stdio buffer-ների դատարկման. երեխայում exec-ի սխալից հետո
int pipe(int fds[2]);                     // fds[0]՝ կարդալու ծայր, fds[1]՝ գրելու ծայր
int dup2(int oldfd, int newfd);           // newfd-ն դառնում է oldfd-ի պատճեն
int chdir(const char *path);              // փոխել ընթացիկ թղթապանակը. cd-ն built-in է (Օր 2)

#include <sys/wait.h>
// Սպասել երեխային. pid = -1՝ ցանկացածին. options = WNOHANG՝ չսպասել, եթե ոչ մեկը չի ավարտվել
pid_t waitpid(pid_t pid, int *status, int options);
// status-ի վերծանում. WIFEXITED(s) → WEXITSTATUS(s), WIFSIGNALED(s) → WTERMSIG(s)

#include <pthread.h>
int pthread_create(pthread_t *t, const pthread_attr_t *attr, void *(*fn)(void *), void *arg);
int pthread_join(pthread_t t, void **result);
int pthread_mutex_lock(pthread_mutex_t *m);
int pthread_mutex_unlock(pthread_mutex_t *m);
```

```cpp
#include <thread>
#include <mutex>
std::thread t(function, args...);  t.join();          // C++11 թղթակիցը
std::mutex m;  std::lock_guard<std::mutex> lock(m);    // unlock-ը ավտոմատ է scope-ի վերջում
```

## Resources · Աղբյուրներ
- *Advanced Linux Programming*, Chapter 3 «Processes» (3.2 fork և exec, 3.4 wait, zombie-ներ) և Chapter 4 «Threads» (ստեղծում, race condition-ներ, mutex-ներ, condition variable-ներ, 4.5 thread-երը Linux-ում, 4.6 process-ներ և thread-եր). [`docs/linux_book.pdf`](../docs/linux_book.pdf)
- [`fork(2)`](https://man7.org/linux/man-pages/man2/fork.2.html), [`execve(2)`](https://man7.org/linux/man-pages/man2/execve.2.html), [`waitpid(2)`](https://man7.org/linux/man-pages/man2/waitpid.2.html), [`pipe(2)`](https://man7.org/linux/man-pages/man2/pipe.2.html), [`dup2(2)`](https://man7.org/linux/man-pages/man2/dup2.2.html), [`clone(2)`](https://man7.org/linux/man-pages/man2/clone.2.html), [`pthreads(7)`](https://man7.org/linux/man-pages/man7/pthreads.7.html)
- [cppreference: Thread support library](https://en.cppreference.com/w/cpp/thread)
- Windows դասընթացի օրինակները նույն խնդիրների համար. [`windows/day6`](../../windows/day6/) (thread-եր, Fibonacci), [`windows/day2`](../../windows/day2/) (փիլիսոփաներ)

## Hands-On Task · Գործնական առաջադրանք

Գրիր փոքր shell. [`template.cpp`](template.cpp)։

```bash
./day10
day10:/home/student$ ls -l /etc/hostname
-rw-r--r-- 1 root root 7 ... /etc/hostname
day10:/home/student$ false
day10:/home/student$ status            # built-in. վերջին հրամանի exit status-ը
1
day10:/home/student$ nosuchcmd
nosuchcmd: No such file or directory
day10:/home/student$ cd /tmp           # built-in
day10:/tmp$ sleep 2 &
[4812]
day10:/tmp$ echo a b c | wc -w         # pipe
3
[4812] done, status 0
day10:/tmp$ exit
```

Լրացրու TODO-ները. տողը բառերի բաժանել, built-in-ներ (`cd`, `exit`, `status`), `fork` + `execvp` + `waitpid` (status-ը՝ ինչպես bash-ում, 128 + signal), ֆոնային հրամաններ `&`-ով և zombie-ների մաքրում `WNOHANG`-ով, մեկ pipe `a | b`։ Ստուգիր `strace -f`-ով, որ քո shell-ը անում է նույն system call-երը, ինչ `bash`-ը։

## Self-Learning · Ինքնուրույն աշխատանք
1. Գրիր ծրագիր, որը `printf("hello")` (առանց `\n`), ապա `fork()`։ Քանի անգամ է տպվում hello։ Ինչու։ Ուղղիր `fflush`-ով։
2. Ստեղծիր zombie C++-ում և տես այն `ps`-ով։ Ապա ավելացրու `SIGCHLD` handler, որը մաքրում է երեխաներին (Օր 8)։
3. Հաշվիր Fibonacci-ի թվերը կամ պարզ թվերը մինչև 10⁷ մեկ, երկու, չորս և ութ thread-ով։ Չափիր ժամանակը (`std::chrono`)։ Ինչու արագացումը `nproc`-ից հետո չի աճում։ (Համեմատիր [`windows/day6`](../../windows/day6/)-ի Fibonacci-ի հետ։)
4. Գործարկիր `counter++` օրինակը առանց mutex-ի 10 անգամ և գրիր արդյունքները։ Ապա `-fsanitize=thread`-ով։ Ինչ է հաղորդում ThreadSanitizer-ը։
5. Համեմատիր երեք տարբերակի արագությունը. mutex ամեն `++`-ի համար, `std::atomic<long>`, տեղային գումար + մեկ lock։
6. Իրականացրու ճաշող փիլիսոփաներին `std::thread`-ով և `std::mutex`-ով։ Նախ ստացիր deadlock, ապա ուղղիր այն կողպեքների հերթականությամբ։
7. Գրիր պարզ heap manager (հին `docs/tasks.txt`-ի #4 առաջադրանքը). `my_malloc(size)` և `my_free(ptr)` 1 MB ստատիկ զանգվածի վրա՝ ազատ block-երի ցուցակով։ Ինչ է fragmentation-ը, և ինչպես է այն երևում քո ալոկատորում։
8. Ընդլայնիր template-ի shell-ը `>` և `<` redirection-ով (`open` + `dup2`)։

## Self-Check · Ինքնաստուգում
Պատասխաններ չկան։ Մտածիր ինքնուրույն։

1. `fork()`-ը վերադառնում է երկու անգամ։ Ինչ է նշանակում սա, և ինչպես է ծրագիրը իմանում, որ process-ում է։
2. Ինչու Unix-ը բաժանում է նոր ծրագրի գործարկումը երկու քայլի (`fork` և `exec`)։ Ինչ կարելի է անել նրանց միջև։
3. `execvp`-ից հետո ինչ է մնում նույնը process-ում, և ինչ է փոխվում։
4. Ինչու է երեխայում `exec`-ի սխալից հետո պետք կանչել `_exit`, ոչ թե `exit`։
5. Ինչ է zombie-ն, և ով է պատասխանատու այն մաքրելու համար։ Ինչ է պատահում orphan-ի հետ։
6. `a | b` pipeline-ը կախվում է և երբեք չի ավարտվում։ Որն է ամենահավանական պատճառը։
7. Ինչու է `counter++`-ը երկու thread-ից անվտանգ չէ, եթե այն մեկ տող է։
8. Ինչու mutex-ը ամեն `++`-ի համար ճիշտ է, բայց դանդաղ։ Ինչպես կարելի է ավելի լավ։
9. Ինչ պայմաններում է առաջանում deadlock։ Ինչպես է կողպեքների ֆիքսված հերթականությունը կանխում այն։
10. Linux-ում ինչ տարբերություն կա process-ի և thread-ի միջև kernel-ի տեսանկյունից։
11. Web server-ը պետք է սպասարկի 1000 միաժամանակյա հաճախորդ։ Ինչ առավելություններ և թերություններ ունի յուրաքանչյուր հաճախորդի համար առանձին process-ը և առանձին thread-ը։

## Code Template · Կոդի ձևանմուշ
Տե՛ս [`template.cpp`](template.cpp)-ը։

```bash
g++ -std=c++17 -Wall -Wextra -O2 template.cpp -o day10 && ./day10
printf 'echo hello\nfalse\nstatus\necho a b c | wc -w\n' | ./day10    # առանց prompt-ի, երբ stdin-ը terminal չէ
```
