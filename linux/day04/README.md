# Day 4: Pipes, Filters and Regular Expressions · Օր 4. Pipe-եր, ֆիլտրեր և ռեգուլյար արտահայտություններ

## Objectives · Նպատակներ
- Հասկանալ, թե ինչ է pipe-ը kernel-ի ներսում. buffer երկու ծայրով, որոնք file descriptor-ներ են
- Ստեղծել pipe C-ով, փոխանցել տվյալներ երկու process-ի միջև և ճիշտ փակել ծայրերը
- Իմանալ `read`-ի և `write`-ի կանոնները pipe-ի վրա. երբ են սպասում, երբ է գալիս EOF-ը, երբ՝ `SIGPIPE`-ը
- Չափել pipe-ի buffer-ը, փոխել այն և բացատրել, ինչու է չափը կարևոր (backpressure, deadlock)
- Իմանալ `PIPE_BUF`-ը և երբ են մի քանի գրողների գրառումները չխառնվում
- Տեսնել, թե ինչպես է shell-ը կառուցում `a | b`-ն, օգտագործել անվանված pipe (FIFO)
- Կառուցել pipeline-ներ ֆիլտրերով, գրել ռեգուլյար արտահայտություններ `grep`-ի, `sed`-ի, `awk`-ի համար

## Key Concepts · Հիմնական գաղափարներ
- Pipe = kernel buffer + երկու file descriptor (`fds[0]` կարդալու, `fds[1]` գրելու)
- Չօգտագործվող ծայրերի փակում և EOF
- Blocking. դատարկ pipe-ից կարդացողը սպասում է, լիքը pipe-ում գրողը սպասում է
- Buffer-ի չափ. 16 էջ (64 KiB), `F_GETPIPE_SZ`, `F_SETPIPE_SZ`
- `PIPE_BUF` (4096). ատոմար գրառում
- `SIGPIPE` / `EPIPE`. գրել pipe-ում, որը ոչ ոք չի կարդում
- `pipe` + `fork` + `dup2` + `exec` = shell-ի `|`
- FIFO (named pipe), `mkfifo`
- Filter, pipeline, regular expression, `grep`, `sed`, `awk`

## Definitions · Սահմանումներ

*Այսօր ներմուծվող տերմինները։ Նույն տեքստը կա [GLOSSARY.md](../GLOSSARY.md)-ում։*

**Pipe** — Kernel-ի օբյեկտ. հիշողության buffer, որն ունի երկու ծայր։ Մեկ ծայրից գրում են, մյուսից կարդում, նույն հերթականությամբ (FIFO)։ Ծրագրի համար երկու ծայրերն էլ սովորական file descriptor-ներ են. աշխատում են `read`-ը, `write`-ը, `close`-ը։ Pipe-ը չունի անուն և ֆայլ սկավառակի վրա։

**Pipe buffer (capacity)** — Որքան տվյալ կարող է պահել pipe-ը, քանի դեռ ոչ ոք չի կարդացել։ Linux-ում լռելյայն 16 էջ է (64 KiB 4 KiB էջերով)։ Երբ buffer-ը լիքն է, գրողը սպասում է։

**EOF on a pipe** — Pipe-ից կարդացող `read`-ը վերադարձնում է 0, երբ buffer-ը դատարկ է **և** գրելու ծայրը փակված է **բոլոր** process-ներում։ Քանի դեռ գոնե մեկ process պահում է գրելու ծայրը բաց, `read`-ը սպասում է։

**`SIGPIPE` / `EPIPE`** — Եթե process-ը գրում է pipe-ում, որի կարդալու ծայրը փակված է բոլոր process-ներում, kernel-ը նրան ուղարկում է `SIGPIPE` signal (լռելյայն՝ ավարտում է process-ը)։ Եթե signal-ը անտեսված է, `write`-ը վերադարձնում է -1, և `errno = EPIPE`։

**`PIPE_BUF`** — Առավելագույն չափը (Linux-ում 4096 բայթ), որով `write`-ը pipe-ում **ատոմար** է. եթե մի քանի process միաժամանակ գրում են մինչև `PIPE_BUF` բայթ, նրանց գրառումները չեն խառնվում իրար։ Ավելի մեծ գրառումները կարող են խառնվել։

**Backpressure** — Երբ կարդացողը դանդաղ է, buffer-ը լցվում է, և գրողը ավտոմատ կանգնում է։ Արագ ծրագիրը չի կարող «խեղդել» դանդաղին. pipeline-ը աշխատում է ամենադանդաղ մասի արագությամբ։

**FIFO (named pipe)** — Pipe, որն ունի անուն ֆայլային համակարգում (`mkfifo`)։ `ls -l`-ում նրա տեսակը `p` է (Օր 3)։ Թույլ է տալիս միացնել process-ներ, որոնք ծնող-երեխա չեն։

**Filter** — Ծրագիր, որը կարդում է stdin-ը, ինչ-որ բան է անում տվյալների հետ և գրում stdout. `grep`, `sort`, `uniq`, `wc`, `cut`, `tr`, `sed`, `awk`։

**Pipeline** — Pipe-երով միացված հրամանների շղթա. `cat log | grep error | sort | uniq -c`։

**Regular expression (regex)** — Ձևանմուշ, որը նկարագրում է տողերի բազմություն։ Մեծ ձևանմուշները կառուցվում են փոքրերից օպերատորներով, ինչպես թվաբանական արտահայտությունները։

**Metacharacter** — Regex-ում հատուկ իմաստով նիշ. `.`, `*`, `^`, `$`, `[`, `]`, `\`, իսկ ERE-ում նաև `+`, `?`, `|`, `(`, `)`, `{`, `}`։ Բառացի համընկնելու համար այն պետք է «escape» անել `\`-ով։

**Anchor** — Regex-ի տարր, որը համընկնում է դիրքի, ոչ թե նիշի հետ. `^`՝ տողի սկիզբ, `$`՝ տողի վերջ, `\<` և `\>`՝ բառի սկիզբ և վերջ, `\b`՝ բառի սահման։

**Bracket expression** — `[...]`-ը համընկնում է **մեկ** նիշի հետ թվարկվածներից. `[aeiou]`, `[0-9]`։ `[^...]`-ը համընկնում է մեկ նիշի հետ, որը **չկա** թվարկվածներում։

**POSIX character class** — Bracket expression-ի ներսում օգտագործվող անվանված բազմություն. `[[:digit:]]`, `[[:alpha:]]`, `[[:space:]]`, `[[:upper:]]`, `[[:alnum:]]`, `[[:punct:]]`։ Աշխատում են ցանկացած լեզվով (locale-ով)։

**Quantifier** — Ցույց է տալիս, քանի անգամ կրկնվի նախորդ տարրը. `*` (0 կամ ավելի), `+` (1 կամ ավելի), `?` (0 կամ 1), `{n}`, `{n,}`, `{n,m}`։

**Back-reference** — `\1`, `\2`. համընկնում է նույն տեքստի հետ, որը համընկել էր 1-ին, 2-րդ խմբի `(...)` հետ։ Օրինակ՝ `\(..\)\1`-ը գտնում է կրկնվող զույգեր, ինչպես `abab`։

**BRE / ERE** — Basic և Extended regular expression. GNU `grep`-ում երկուսն ունեն նույն հնարավորությունները, տարբերվում է միայն գրելաձևը. BRE-ում `?`, `+`, `{`, `|`, `(`, `)` նիշերը սովորական են, և օպերատոր դառնում են միայն `\`-ով։ `grep`-ը լռելյայն BRE է, `grep -E`-ն՝ ERE։

**`sed` (stream editor)** — Ֆիլտր, որը տող առ տող խմբագրում է տեքստը հրամաններով. ամենահաճախը՝ `s/regex/replacement/`։

**`awk`** — Փոքր ծրագրավորման լեզու տեքստը սյունակներով մշակելու համար. ամեն տող բաժանվում է դաշտերի `$1`, `$2`, ..., և ամեն տողի համար կատարվում է `{ action }`-ը։

---

## 1. Ինչ է pipe-ը

### Visual
![Pipeline. ls, grep, wc երեք առանձին process են, որոնք աշխատում են միաժամանակ. ls-ի fd 1-ը գրում է kernel-ի pipe buffer-ի մեջ, grep-ի fd 0-ն կարդում է նրանից. stderr-ը (fd 2) չի անցնում pipe-ով և գնում է terminal. buffer-ի չափը 64 KB է. գրողը սպասում է, երբ buffer-ը լիքն է, կարդացողը՝ երբ դատարկ է](pipeline.svg)

Օր 2-ում տեսանք, որ ամեն process ունի file descriptor-ներ. 0՝ stdin, 1՝ stdout, 2՝ stderr, և որ `>`-ը ու `<`-ը պարզապես փոխում են, թե ուր են դրանք ցույց տալիս։ Իսկ Օր 3-ում՝ որ file descriptor-ը kernel-ի օբյեկտի «բռնակ» է, որը կարող է լինել ֆայլ, terminal կամ սարք։

**Pipe**-ը ևս մեկ այդպիսի օբյեկտ է. kernel-ի հիշողության մեջ գտնվող buffer, որն ունի երկու ծայր.

- **գրելու ծայր**. ինչ գրվում է այստեղ, ավելանում է buffer-ի վերջում
- **կարդալու ծայր**. `read`-ը վերցնում է տվյալները buffer-ի սկզբից, և դրանք անհետանում են buffer-ից

Pipe-ը ֆայլ չէ. տվյալները չեն հասնում սկավառակին, և `lseek`-ը չի աշխատում։ Կարդալ կարելի է միայն մեկ անգամ և միայն հերթականությամբ։

Pipe ստեղծվում է մեկ system call-ով.

```c
#include <unistd.h>

int fds[2];
if (-1 == pipe(fds)) { perror("pipe"); return 1; }
// fds[0]՝ կարդալու ծայր, fds[1]՝ գրելու ծայր
```

Հիշելու հնարք. 0-ն stdin-ն է (կարդալ), 1-ը stdout-ն է (գրել). նույնն է `fds[0]`-ի և `fds[1]`-ի համար։

Ամենապարզ օրինակը. գրել և կարդալ նույն process-ում։

```c
int fds[2];
pipe(fds);                                  // օրինակ՝ fds[0] = 3, fds[1] = 4

write(fds[1], "hello\n", 6);                // 6 բայթ հիմա kernel-ի buffer-ում են
close(fds[1]);                              // այլևս ոչ ոք չի գրի

char buf[64];
ssize_t n = read(fds[0], buf, sizeof buf);  // n = 6, buf = "hello\n"
n = read(fds[0], buf, sizeof buf);          // n = 0. EOF, քանի որ գրելու ծայրը փակ է
close(fds[0]);
```

Ինչու են համարները 3 և 4. 0, 1, 2-ը արդեն զբաղված են, իսկ kernel-ը միշտ տալիս է ամենափոքր ազատ համարը։

Pipe-երը կարելի է տեսնել աշխատող process-ի մոտ.

```bash
sleep 300 | sleep 300 &
ls -l /proc/$(pgrep -n sleep)/fd
# 0 -> pipe:[48213]       ← երկրորդ sleep-ի stdin-ը pipe-ի կարդալու ծայրն է
# 1 -> /dev/pts/0
```

`pipe:[48213]`-ի թիվը pipe-ի inode-ն է. նույն թիվը կտեսնես առաջին `sleep`-ի fd 1-ում։

## 2. Pipe երկու process-ի միջև

Մեկ process-ում pipe-ը անիմաստ է։ Այն օգտակար է, երբ մի process-ը գրում է, մյուսը կարդում։ Դրա համար օգտագործվում է `fork()`. այն ստեղծում է ընթացիկ process-ի պատճենը (մանրամասն՝ Օր 10)։ Կարևորն այստեղ այն է, որ երեխան **ժառանգում է ծնողի բոլոր բաց file descriptor-ները**, այդ թվում pipe-ի երկու ծայրերը։ `fork`-ից հետո երկու process-ն էլ ունեն և՛ `fds[0]`, և՛ `fds[1]`։

### Visual
![fork-ից հետո ծնողը և երեխան ունեն pipe-ի երկու ծայրերն էլ. երեխան փակում է կարդալու ծայրը, ծնողը՝ գրելու ծայրը. մնում է մեկ ուղղությամբ կապուղի երեխայից ծնող. աղյուսակ. read-ը դատարկ pipe-ից, երբ գրող կա՝ սպասում է, երբ գրող չկա՝ վերադարձնում է 0 (EOF). write-ը լիքը pipe-ում՝ սպասում է, երբ կարդացող չկա՝ SIGPIPE](pipe_fork.svg)

```c
int fds[2];
pipe(fds);

pid_t pid = fork();
if (0 == pid) {                          // երեխա. գրող
    close(fds[0]);                       // կարդալու ծայրը երեխային պետք չէ
    const char *msg = "data from child\n";
    write(fds[1], msg, strlen(msg));
    close(fds[1]);                       // վերջ. ծնողը կստանա EOF
    _exit(0);
}

close(fds[1]);                           // ծնող. կարդացող. ԳՐԵԼՈՒ ԾԱՅՐԸ ՓԱԿԵԼ
char buf[256];
ssize_t n;
while (0 < (n = read(fds[0], buf, sizeof buf)))
    fwrite(buf, 1, n, stdout);
close(fds[0]);
waitpid(pid, NULL, 0);
```

**Ամենակարևոր կանոնը. ամեն process փակում է այն ծայրը, որը չի օգտագործում։**

Ինչու. EOF-ը գալիս է միայն այն ժամանակ, երբ գրելու ծայրը փակված է **բոլոր** process-ներում։ Եթե ծնողը մոռանա `close(fds[1])`-ը, ապա երեխան կավարտվի, բայց ծնողը ինքն է պահում գրելու ծայրը բաց, և նրա `read`-ը կսպասի հավերժ։ Ծրագիրը «կախվում է» առանց որևէ սխալի հաղորդագրության։ Սա pipe-երով աշխատելիս ամենատարածված սխալն է։ Փորձիր (Self-Learning 1)։

## 3. `read`-ի և `write`-ի կանոնները

Pipe-ի ամբողջ վարքը նկարագրվում է չորս իրավիճակով.

| Իրավիճակ | Ինչ է անում kernel-ը |
|---|---|
| `read`. buffer-ում տվյալ կա | Վերադարձնում է **այն, ինչ կա** (կարող է լինել քիչ, քան խնդրել ես) |
| `read`. buffer-ը դատարկ է, գրող կա | **Սպասում է** (process-ը քնում է, `S` վիճակ, Օր 8) |
| `read`. buffer-ը դատարկ է, գրող չկա | Վերադարձնում է **0** (EOF) |
| `write`. buffer-ում տեղ կա | Գրում է և վերադառնում |
| `write`. buffer-ը լիքն է | **Սպասում է**, մինչև կարդացողը ազատի տեղ |
| `write`. կարդացող չկա | **`SIGPIPE`** (process-ը մեռնում է), կամ `-1` և `errno = EPIPE` |

Երկու կարևոր հետևանք.

1. **`read`-ը կարող է վերադարձնել ավելի քիչ, քան սպասում ես։** Եթե գրողը գրել է 1000 բայթ, կարդացողը կարող է ստանալ 300 + 700։ Pipe-ը բայթերի հոսք է, ոչ թե հաղորդագրություններ. ճիշտ ծրագիրը միշտ կարդում է ցիկլով, մինչև 0 (այդպես է աշխատում `read_all`-ը template-ում)։ Նույնը կտեսնենք TCP-ում (Օր 11)։
2. **Մեծ `write`-ը կարող է սպասել։** 1 MB գրելը 64 KiB pipe-ում արգելափակում է գրողին, մինչև կարդացողը վերցնի տվյալները։ Դա սխալ չէ, այլ հենց այն մեխանիզմը, որ թույլ է տալիս pipe-ով փոխանցել ցանկացած չափի տվյալ։

`O_NONBLOCK` դրոշով (`fcntl(fd, F_SETFL, O_NONBLOCK)`) «սպասում է»-ի փոխարեն `read`-ը և `write`-ը անմիջապես վերադարձնում են -1 և `errno = EAGAIN`։ Template-ում այսպես ենք չափում buffer-ը։

## 4. Buffer-ի չափը

Որքան տվյալ է տեղավորվում pipe-ում, քանի դեռ ոչ ոք չի կարդում։ Պարզ փորձ. գրել առանց կարդալու և հաշվել, թե երբ `write`-ը կասի «այլևս տեղ չկա».

```c
int fds[2];
pipe(fds);
fcntl(fds[1], F_SETFL, O_NONBLOCK);       // չսպասել, այլ վերադարձնել EAGAIN

char block[1024] = {0};
long total = 0;
ssize_t n;
while (0 < (n = write(fds[1], block, sizeof block)))
    total += n;
printf("capacity = %ld\n", total);        // 65536
```

Linux-ում լռելյայն չափը **16 էջ** է։ x86-ում և Jetson-ի վրա էջը 4 KiB է, ուստի ստանում ենք 65536 բայթ։ Raspberry Pi 5-ի kernel-ը օգտագործում է 16 KiB էջեր, և այնտեղ նույն ծրագիրը կտպի 262144։ Ստուգիր `getconf PAGESIZE`-ով։ `man 7 pipe`-ը հատուկ զգուշացնում է, որ ծրագրերը չպետք է կախված լինեն կոնկրետ չափից։

Չափը կարելի է իմանալ և փոխել `fcntl`-ով.

```c
long size = fcntl(fds[1], F_GETPIPE_SZ);          // 65536
fcntl(fds[1], F_SETPIPE_SZ, 1 << 20);             // խնդրել 1 MiB
size = fcntl(fds[1], F_GETPIPE_SZ);               // 1048576
```

- Kernel-ը կլորացնում է խնդրված չափը մինչև էջերի 2-ի աստիճան. 100000 → 131072։
- Սովորական օգտատերը չի կարող խնդրել ավելի, քան `/proc/sys/fs/pipe-max-size` (լռելյայն 1 MiB)։ root-ը կարող է։

```bash
getconf PIPE_BUF /                    # 4096
cat /proc/sys/fs/pipe-max-size        # 1048576
getconf PAGESIZE                      # 4096 (կամ 16384 Raspberry Pi 5-ի վրա)
```

**Ինչու է չափը կարևոր.**

- **Backpressure**. `fast_producer | slow_consumer` pipeline-ում buffer-ը լցվում է, և արագ ծրագիրը ավտոմատ կանգնում է։ Հիշողությունը չի աճում անվերջ։ Embedded համակարգում (օրինակ՝ տեսախցիկ → մշակում) սա պաշտպանում է RAM-ը։
- **Deadlock**. եթե ծրագիրը երկու pipe-ով կապված է մեկ այլ ծրագրի հետ (գրում է նրա stdin-ը, կարդում նրա stdout-ը), և երկուսն էլ գրում են առանց կարդալու, երկու buffer-ն էլ լցվում են, և երկուսն էլ սպասում են հավերժ։ Template-ի TODO 5-ը և Self-Learning 4-ը սրա մասին են։
- **Արագություն**. մեծ buffer-ը նշանակում է ավելի քիչ context switch գրողի և կարդացողի միջև։ Մեծ տվյալների համար (video, sensor stream) `F_SETPIPE_SZ`-ը կարող է նկատելի արագացում տալ։

## 5. `PIPE_BUF`. ատոմար գրառումներ

Եթե մի քանի process գրում են **նույն** pipe-ում (օրինակ՝ մի քանի worker ուղարկում են log-եր մեկ կարդացողի), կարող են նրանց տվյալները խառնվել։ POSIX-ը խոստանում է.

- **`n ≤ PIPE_BUF`** (Linux-ում 4096). մեկ `write`-ի բոլոր `n` բայթերը գրվում են **միասին**, ուրիշի տվյալները դրանց մեջտեղում չեն հայտնվի։
- **`n > PIPE_BUF`**. գրառումը կարող է բաժանվել մասերի, և այլ գրողների տվյալները կարող են հայտնվել մեջտեղում։

Գործնական կանոն. եթե մի քանի գրող կա, ամեն հաղորդագրություն ուղարկիր **մեկ** `write`-ով, և այն պահիր 4096 բայթից փոքր։ `printf`-ը կարող է բաժանել տողը մի քանի `write`-ի (stdio buffer-ը, Օր 2), ուստի այսպիսի դեպքերում ավելի ապահով է նախ ձևավորել տողը `snprintf`-ով, ապա ուղարկել մեկ `write`-ով։

## 6. `SIGPIPE`. երբ կարդացողը գնաց

```bash
yes | head -3
```

`yes`-ը անվերջ տպում է `y`։ Ինչու է այս pipeline-ը ավարտվում ակնթարթորեն.

1. `head`-ը կարդում է 3 տող, տպում է և ավարտվում։ Pipe-ի կարդալու ծայրը փակվում է։
2. `yes`-ի հաջորդ `write`-ը գրում է pipe-ում, որը ոչ ոք չի կարդում։
3. Kernel-ը `yes`-ին ուղարկում է `SIGPIPE`, որի լռելյայն գործողությունը process-ի ավարտն է։

Տես սա.

```bash
yes | head -3; echo "${PIPESTATUS[@]}"     # 141 0 → yes-ը մեռավ signal 13-ով (128 + 13)
strace -f -e trace=write -e signal=SIGPIPE bash -c 'yes | head -1' 2>&1 | grep PIPE
# [pid 1416] <... write resumed>) = -1 EPIPE (Broken pipe)
# [pid 1416] --- SIGPIPE {si_signo=SIGPIPE, ...} ---
# [pid 1416] +++ killed by SIGPIPE +++
```

Ծրագրում, որը չպետք է մեռնի այս դեպքում (օրինակ՝ server, Օր 11), `SIGPIPE`-ը անտեսում են (`signal(SIGPIPE, SIG_IGN)`), և ստուգում են `write`-ի արդյունքը. `-1` և `errno == EPIPE`։

## 7. Ինչպես է shell-ը կառուցում `a | b`

Հիմա կարող ենք բացատրել `ls | wc -l`-ը ամբողջությամբ.

```c
int fds[2];
pipe(fds);

if (0 == fork()) {                  // առաջին երեխա. ls
    dup2(fds[1], 1);                // fd 1-ը (stdout) հիմա pipe-ի գրելու ծայրն է
    close(fds[0]); close(fds[1]);   // բնօրինակներն այլևս պետք չեն
    execlp("ls", "ls", NULL);       // ls-ը գրում է fd 1-ում և չգիտի, որ դա pipe է
}
if (0 == fork()) {                  // երկրորդ երեխա. wc
    dup2(fds[0], 0);                // fd 0-ն (stdin) հիմա կարդալու ծայրն է
    close(fds[0]); close(fds[1]);
    execlp("wc", "wc", "-l", NULL);
}
close(fds[0]); close(fds[1]);       // shell-ը ինքը pipe-ը չի օգտագործում
wait(NULL); wait(NULL);
```

- `dup2(old, new)`-ը `new` համարը դարձնում է `old`-ի պատճենը։ Այսպես `ls`-ի stdout-ը դառնում է pipe։
- `exec`-ը փոխարինում է ծրագիրը, բայց file descriptor-ները մնում են. `ls`-ը ժառանգում է արդեն ուղղորդված fd 1-ը։
- Եթե shell-ը մոռանա փակել `fds[1]`-ը, `wc`-ը երբեք չի ստանա EOF, և հրամանը կկախվի։

`ls`-ը և `wc`-ը աշխատում են **միաժամանակ**։ `wc`-ը սկսում է հաշվել, երբ `ls`-ը դեռ տպում է։ Ծրագիրը չգիտի, որ իր stdout-ը pipe է. սա Unix-ի փիլիսոփայության երկրորդ կանոնն է (Օր 1). ամեն ծրագրի ելքը կարող է դառնալ ուրիշի մուտք։ Օր 10-ում այս կոդը կդառնա մեր shell-ի մաս։

Մեկ ուղղությամբ դեպքի համար C գրադարանն ունի պատրաստի ֆունկցիա. `popen`-ը անում է `pipe` + `fork` + `exec` մեկ կանչով.

```c
FILE *p = popen("ls /etc | wc -l", "r");    // "r"՝ կարդալ հրամանի stdout-ը, "w"՝ գրել նրա stdin-ը
char line[64];
fgets(line, sizeof line, p);
int status = pclose(p);                     // սպասում է և վերադարձնում exit status-ը
```

## 8. Անվանված pipe (FIFO)

Սովորական pipe-ը հասանելի է միայն ծնողին և նրա երեխաներին (`fork`-ով ժառանգված fd-ներ)։ Իսկ եթե պետք է միացնել երկու անկախ ծրագիր, օրինակ՝ երկու terminal-ում. **FIFO**-ն pipe է, որն ունի անուն ֆայլային համակարգում.

```bash
mkfifo /tmp/chan
ls -l /tmp/chan          # prw-r--r-- ... /tmp/chan     ← տեսակը p (Օր 3)

# terminal 1
cat /tmp/chan            # սպասում է, մինչև ինչ-որ մեկը բացի գրելու համար

# terminal 2
echo "hello" > /tmp/chan # terminal 1-ում կհայտնվի hello, և cat-ը կավարտվի (EOF)
```

- `open`-ը FIFO-ի վրա **սպասում է**, մինչև բացվի մյուս ծայրը։ Կարդացողը սպասում է գրողին, և հակառակը։
- Բացվելուց հետո FIFO-ն աշխատում է ճիշտ ինչպես pipe-ը. նույն buffer-ը, նույն կանոնները։
- Տվյալները չեն գրվում սկավառակին. ֆայլային համակարգում պահվում է միայն անունը և իրավունքները (Օր 7)։

C-ում.

```c
mkfifo("/tmp/chan", 0644);
int fd = open("/tmp/chan", O_WRONLY);   // կսպասի կարդացողին
write(fd, "hello\n", 6);
close(fd);
```

Embedded համակարգերում FIFO-ն պարզ ձև է կապելու, օրինակ, sensor կարդացող daemon-ը և մշակող ծրագիրը առանց ցանցի։

## 9. Pipeline-ներ shell-ում

Հիմա, երբ գիտենք, թե ինչ է կատարվում ներսում, անցնենք գործնական կողմին։ Pipeline-ները կառուցում են ոչ թե միանգամից, այլ **քայլ առ քայլ**. ավելացրու մեկ ֆիլտր, նայիր արդյունքը, ապա ավելացրու հաջորդը։

**Կառուցիր pipeline քայլ առ քայլ** և ամեն քայլից հետո նայիր արդյունքը.

```bash
ls -l /etc                               # 1. ինչ ունենք
ls -l /etc | tail -n +2                  # 2. առանց "total ..." տողի
ls -l /etc | tail -n +2 | awk '{print $3}'           # 3. միայն owner-ը
ls -l /etc | tail -n +2 | awk '{print $3}' | sort | uniq -c   # 4. քանի ֆայլ ունի ամեն owner
```

Pipe-ը կարելի է համատեղել redirection-ի հետ.

```bash
grep -i error /var/log/syslog | sort | uniq -c > summary.txt 2> errors.txt
```

Օգտակար գործիքներ pipeline-ների համար.

```bash
make 2>&1 | tee build.log            # tee. տպել էկրանին և միաժամանակ պահել ֆայլում
ls *.txt | xargs wc -l               # xargs. stdin-ի բառերը դարձնել argument-ներ
find . -name '*.tmp' -print0 | xargs -0 rm   # -print0 / -0՝ բացատներով անունների համար
wc -l <<< "one line"                 # here-string. տող stdin-ին
cat <<EOF > config.ini               # here-document. մի քանի տող stdin-ին
[server]
port = 8080
EOF
diff <(ls dir1) <(ls dir2)           # process substitution. ելքը որպես «ֆայլ»
```

## 10. Ֆիլտրեր

**Ֆիլտրը** ծրագիր է, որը կարդում է stdin-ը, փոխում կամ ընտրում է տվյալները և գրում stdout։ Ամեն ֆիլտր անում է **մեկ** պարզ բան, իսկ բարդ առաջադրանքները լուծվում են դրանք pipe-երով միացնելով (Օր 1, Unix-ի փիլիսոփայությունը)։ Ֆիլտրը չգիտի, որ աշխատում է pipeline-ում. նա պարզապես կարդում է fd 0-ն և գրում fd 1-ը։

Ամենահաճախ օգտագործվողները ստորև են։ Դրանք անգիր անել պետք չէ. `man sort`-ը և `sort --help`-ը միշտ ձեռքի տակ են։ Կարևորը հասկանալն է, որ ամեն մեկը մեկ քայլ է, և փորձել դրանք pipeline-ներում.

| Հրաման | Ինչ է անում | Օրինակ |
|---|---|---|
| `head -n N`, `tail -n N` | Առաջին / վերջին N տողերը. `tail -n +N`՝ N-րդից սկսած | `ls \| head -3` |
| `wc -l -w -c` | Տողեր, բառեր, բայթեր | `wc -l < /etc/passwd` |
| `sort` | Դասավորել. `-n` թվային, `-r` հակառակ, `-k N` ըստ դաշտի, `-t:` բաժանիչ, `-u` եզակի | `sort -t: -k3 -n /etc/passwd` |
| `uniq` | Հեռացնել **հարևան** կրկնվող տողերը. `-c` հաշվել, `-d` միայն կրկնվողները | `sort \| uniq -c` |
| `cut` | Կտրել դաշտեր (`-d: -f1`) կամ նիշեր (`-c1-10`) | `cut -d: -f1 /etc/passwd` |
| `tr` | Փոխարինել (`tr a-z A-Z`), ջնջել (`-d`), սեղմել (`-s`) նիշերը | `tr -s ' ' < f` |
| `paste` | Միացնել ֆայլերը սյունակներով | `paste names.txt ages.txt` |
| `tee` | Պատճենել stdin-ը ֆայլ և stdout | `cmd \| tee log` |
| `rev`, `nl` | Շրջել տողը, համարակալել տողերը | `nl script.sh` |

`uniq`-ը հեռացնում է միայն **հարևան** կրկնությունները, այդ պատճառով նրանից առաջ գրեթե միշտ գալիս է `sort`-ը։ «Ամենահաճախը» գտնելու դասական pipeline-ը.

```bash
# Ամենահաճախ օգտագործվող 10 հրամանները քո history-ում
history | awk '{print $2}' | sort | uniq -c | sort -rn | head
```

## 11. Ռեգուլյար արտահայտություններ

### Visual
![Ռեգուլյար արտահայտության վերլուծություն. ^([0-9]{1,3}\.){3}[0-9]{1,3}$ IP հասցեի ձևանմուշը բաժանված մասերի. ^ տողի սկիզբ, [0-9] մեկ թվանշան, {1,3} մեկից երեք անգամ, \. բառացի կետ, ( ){3} խումբը երեք անգամ, $ տողի վերջ. ներքևում՝ որ տողերն են համընկնում և որոնք՝ ոչ](regex_anatomy.svg)

**Regular expression**-ը ձևանմուշ է, որը նկարագրում է տողերի բազմություն։ Ամենապարզ regex-ը սովորական բառ է. `grep root`-ը գտնում է `root` պարունակող տողերը։ Հզորությունը **metacharacter**-ների մեջ է.

| Նիշ | Իմաստ | Օրինակ | Համընկնում է |
|---|---|---|---|
| `.` | Ցանկացած մեկ նիշ | `t.t` | `tat`, `t3t`, `t t` |
| `*` | Նախորդ տարրը 0 կամ ավելի անգամ | `twe*t` | `twt`, `twet`, `tweeet` |
| `^` | Տողի սկիզբ | `^#` | մեկնաբանության տողեր |
| `$` | Տողի վերջ | `sh$` | `.sh`-ով ավարտվող տողեր |
| `^$` | | | դատարկ տողեր |
| `[abc]` | Մեկ նիշ թվարկվածներից | `a[0-9]x` | `a0x`, `a9x` |
| `[^abc]` | Մեկ նիշ, որը թվարկվածներից չէ | `[^0-9]` | ցանկացած ոչ-թվանշան |
| `\` | Հաջորդ նիշը բառացի է | `\.` | կետ, ոչ թե «ցանկացած նիշ» |
| `.*` | Ցանկացած տեքստ (նաև դատարկ) | `a.*x` | `ax`, `abcx`, `a---x` |

`ls -l`-ի ելքի վրա սրանք շատ օգտակար են.

```bash
ls -l | grep '^d'          # միայն թղթապանակները
ls -l | grep '^-'          # միայն սովորական ֆայլերը
ls -l ~ | grep '^.....w'   # ֆայլեր, որոնց group-ը գրելու իրավունք ունի (6-րդ նիշը w է)
grep -v '^#' /etc/fstab | grep -v '^$'   # առանց մեկնաբանությունների և դատարկ տողերի
```

**Bracket expression**-ներ և POSIX դասեր.

| Գրելաձև | Իմաստ |
|---|---|
| `[0-9]`, `[a-z]`, `[A-Za-z0-9_]` | Միջակայքեր |
| `[[:digit:]]` | Թվանշան |
| `[[:alpha:]]`, `[[:alnum:]]` | Տառ / տառ կամ թվանշան |
| `[[:upper:]]`, `[[:lower:]]` | Մեծատառ / փոքրատառ |
| `[[:space:]]` | Բացատ, tab, նոր տող |
| `[[:punct:]]` | Կետադրական նշաններ |

Ուշադրություն. `[[:digit:]]`-ում արտաքին `[ ]`-ը bracket expression-ն է, ներքինը՝ դասի անունը։ `[:digit:]`-ը առանց արտաքին փակագծերի նշանակում է «`:`, `d`, `i`, `g`, `t` նիշերից մեկը»։

**Quantifier-ներ, խմբեր և alternation** (ERE գրելաձևով).

| ERE | BRE | Իմաստ |
|---|---|---|
| `a+` | `a\+` | 1 կամ ավելի |
| `a?` | `a\?` | 0 կամ 1 |
| `a{3}`, `a{2,5}`, `a{2,}` | `a\{3\}` ... | Ճիշտ 3, 2-ից 5, 2 կամ ավելի |
| `(ab)+` | `\(ab\)\+` | Խումբ |
| <code>cat&#124;dog</code> | <code>cat\&#124;dog</code> | Alternation. մեկը կամ մյուսը |
| `(..)\1` | `\(..\)\1` | Back-reference. նույն տեքստը կրկին |

GNU `grep`-ում BRE-ն և ERE-ն ունեն նույն հնարավորությունները. BRE-ում `?`, `+`, `{`, `|`, `(`, `)` նիշերը սովորական են, և օպերատոր դառնալու համար պահանջում են `\`։ Գործնականում `grep -E`-ով գրելն ավելի ընթեռնելի է։

**Բառի սահմաններ** (GNU ընդլայնում). `\<` և `\>`՝ բառի սկիզբ և վերջ, `\b`՝ ցանկացած սահման, `\w`՝ բառի նիշ (`[[:alnum:]_]`).

```bash
grep '\<the\>' text.txt     # "the", բայց ոչ "there" կամ "other"
grep -w the text.txt        # նույնը, option-ով
```

**Միշտ դիր regex-ը միակ չակերտների մեջ.** Այլապես shell-ը կբացի `*`, `$`, `\` նիշերը մինչև `grep`-ին հասնելը (Օր 2)։

### `grep`

```bash
grep -n root /etc/passwd             # տողի համարով
grep -i error log.txt                # անտեսել մեծատառ/փոքրատառ
grep -v '^#' config                  # տողեր, որոնք ՉԵՆ համընկնում
grep -c bash /etc/passwd             # միայն քանակը
grep -o -E '[0-9]+' f.txt            # միայն համընկնած մասը, ամեն մեկը առանձին տողում
grep -r -l TODO src/                 # ռեկուրսիվ, միայն ֆայլերի անունները
grep -A3 -B1 'Segmentation' log      # համատեքստ. 3 տող հետո, 1 տող առաջ
grep -q root /etc/passwd && echo yes # լուռ. միայն exit status
```

`grep`-ի **exit status**-ը. `0`՝ գտավ գոնե մեկ տող, `1`՝ ոչինչ չգտավ, `2`՝ սխալ (ֆայլը չկա, regex-ը սխալ է)։ Script-երում `grep -q`-ն պայմանի համար է։

## 12. `find`. ֆայլերի որոնում

`grep`-ը փնտրում է ֆայլերի **ներսում**, `find`-ը՝ ֆայլերը **ծառում**՝ ըստ անվան, տեսակի, չափի, ժամի, owner-ի.

```bash
find ~ -name '*.cpp'                       # ըստ անվան (glob, ոչ regex)
find /etc -type d -name 'ssh*'             # միայն թղթապանակներ
find / -size +100M 2>/dev/null             # 100 MB-ից մեծ
find . -mtime -1                           # վերջին 24 ժամում փոխված
find . -user student -perm -o+w            # owner-ը student է, բոլորը կարող են գրել
find . -name '*.o' -delete                 # ջնջել
find . -name '*.cpp' -exec grep -l main {} +   # ֆայլեր, որոնցում կա main
```

`-exec cmd {} \;`-ը գործարկում է հրամանը ամեն ֆայլի համար առանձին, `-exec cmd {} +`-ը՝ մեկ անգամ բոլորի համար (արագ է)։

## 13. `sed`

`sed`-ը տող առ տող խմբագրում է տեքստը։ Ամենաշատ օգտագործվող հրամանը `s/regex/replacement/flags`-ն է.

```bash
echo "The rain in Spain stays mainly in the plain." > easy_sed.txt
sed 's/rain/snow/' easy_sed.txt                              # The snow in Spain ...
sed -i.bak 's/rain/snow/;s/Spain/Sweden/;s/plain/mountains/' easy_sed.txt
cat easy_sed.txt                                              # The snow in Sweden stays mainly in the mountains.
cat easy_sed.txt.bak                                          # սկզբնականը
```

| Գրելաձև | Իմաստ |
|---|---|
| `s/a/b/` | Առաջին `a`-ն ամեն տողում → `b` |
| `s/a/b/g` | Բոլորը |
| `s/a/b/2` | Միայն երկրորդը |
| `s/a/b/I` | Անտեսել case-ը (GNU) |
| `-n '5,10p'` | Տպել միայն 5–10 տողերը |
| `'/^#/d'` | Ջնջել `#`-ով սկսվող տողերը |
| `'/start/,/end/d'` | Ջնջել `start`-ից մինչև `end` |
| `-E 's/(\w+) (\w+)/\2 \1/'` | Փոխել երկու բառերի տեղերը |
| `-i` / `-i.bak` | Փոխել ֆայլը տեղում / պահուստով |

`/`-ի փոխարեն կարելի է օգտագործել ցանկացած նիշ, ինչը հարմար է ճանապարհների համար. `sed 's|/usr/local|/opt|g'`։

## 14. `awk`

`awk`-ը ամեն տող բաժանում է **դաշտերի** (լռելյայն՝ բացատներով). `$1`, `$2`, ..., `$NF` (վերջինը), `$0` (ամբողջ տողը)։ `NR`-ը տողի համարն է։ Ծրագրի կառուցվածքը.

```
BEGIN { սկզբում, մեկ անգամ }
պայման { ամեն տողի համար, որը բավարարում է պայմանին }
END { վերջում, մեկ անգամ }
```

```bash
ls -l /usr | awk '{print $9 "\t" $5}'                  # անուն և չափ
ls -lh /lib | awk '{printf "%20s\t%s\n", $9, $5}'      # հավասարեցված սյունակներ
ls -l /usr/lib | awk 'BEGIN {sum = 0} {sum += $5} END {printf "Total: %.1f MB\n", sum / 1e6}'
awk -F: '$3 >= 1000 {print $1, $6}' /etc/passwd       # սովորական օգտատերեր և նրանց home-երը
awk 'length($0) > 80 {print FILENAME ": " NR}' *.cpp   # 80 նիշից երկար տողեր
```

`sed`-ը և `awk`-ը միասին.

```bash
ls -l /lib | awk '{printf "%s\t%s\n", $9, $5}' | sed -E 's/\.so(\.[0-9]+)*//' | less
```

## 15. Ամեն ինչ միասին

```bash
# Ո՞ր IP հասցեներից են ամենաշատ հարցումները web սերվերի log-ում
awk '{print $1}' access.log | sort | uniq -c | sort -rn | head -5

# Քանի՞ անգամ է հանդիպում յուրաքանչյուր բառը ֆայլում
tr -cs '[:alpha:]' '\n' < book.txt | tr 'A-Z' 'a-z' | sort | uniq -c | sort -rn | head

# Որ ֆայլերում կա TODO, և քանի հատ
grep -r -c TODO src/ | grep -v ':0$' | sort -t: -k2 -rn
```

---

## Commands · Հրամաններ

*Ամփոփում. այս օրվա հրամանները, որոնք բացատրված են վերևում։ Անգիր անել պետք չէ. վերադարձիր այստեղ, երբ մոռանաս գրելաձևը։ Բոլոր օրերի հրամանների ցանկը [COMMANDS.md](../COMMANDS.md)-ում է։*

| Թեմա | Հրամաններ | Բաժին |
|---|---|---|
| Pipe-երը համակարգում | `ls -l /proc/PID/fd`, `getconf PIPE_BUF /`, `getconf PAGESIZE`, `cat /proc/sys/fs/pipe-max-size` | §1, §4 |
| `SIGPIPE` | `yes \| head -3; echo "${PIPESTATUS[@]}"`, `strace -f -e trace=write` | §6 |
| FIFO | `mkfifo`, `ls -l` (տեսակ `p`) | §8 |
| Pipeline-ի գործիքներ | `tee`, `xargs`, `<<<`, `<<EOF`, `<( )` | §9 |
| Ֆիլտրեր | `head`, `tail`, `wc`, `sort`, `uniq`, `cut`, `tr`, `paste`, `nl` | §10 |
| Որոնում | `grep` (`-i -n -v -c -o -r -l -w -E -F -q -A -B`), `find` | §11, §12 |
| Խմբագրում | `sed`, `awk` | §13, §14 |

## Functions · Ֆունկցիաներ

*Ամփոփում. այս օրվա C ֆունկցիաները։ Ամբողջական ցանկը [API.md](../API.md)-ում է։*

```c
#include <unistd.h>
int pipe(int fds[2]);                    // fds[0]՝ կարդալու, fds[1]՝ գրելու ծայր. 0՝ հաջող, -1՝ սխալ
ssize_t read(int fd, void *buf, size_t n);         // 0 → EOF (գրողներ չկան)
ssize_t write(int fd, const void *buf, size_t n);  // կարող է սպասել. -1/EPIPE, եթե կարդացող չկա
int close(int fd);
int dup2(int oldfd, int newfd);          // newfd-ն դառնում է oldfd-ի պատճեն (§7)
pid_t fork(void);                        // երեխան ժառանգում է բոլոր fd-ները (մանրամասն՝ Օր 10)

#include <fcntl.h>
int fcntl(int fd, int cmd, ...);         // F_GETFL/F_SETFL + O_NONBLOCK, F_GETPIPE_SZ, F_SETPIPE_SZ

#include <sys/stat.h>
int mkfifo(const char *path, mode_t mode);         // անվանված pipe

#include <stdio.h>
FILE *popen(const char *command, const char *type); // "r" կամ "w"
int pclose(FILE *stream);                // սպասում է, վերադարձնում exit status-ը

#include <limits.h>
PIPE_BUF                                 // 4096. մինչև այսքան բայթ write-ը ատոմար է

#include <regex.h>
int regcomp(regex_t *preg, const char *regex, int cflags);   // REG_EXTENDED, REG_ICASE, REG_NOSUB
int regexec(const regex_t *preg, const char *string, size_t nmatch, regmatch_t pmatch[], int eflags);
size_t regerror(int errcode, const regex_t *preg, char *errbuf, size_t errbuf_size);
void regfree(regex_t *preg);
```

## Resources · Աղբյուրներ
- *Advanced Linux Programming*, Chapter 5.4 «Pipes» (pipe-ի ստեղծում, ծնող-երեխա կապ, `dup2`, `popen`, FIFO-ներ). [`docs/linux_book.pdf`](../docs/linux_book.pdf)
- [`pipe(7)`](https://man7.org/linux/man-pages/man7/pipe.7.html). buffer-ի չափը և նրա պատմությունը, `F_GETPIPE_SZ`/`F_SETPIPE_SZ`, `pipe-max-size`, `PIPE_BUF`-ի ատոմարությունը, `SIGPIPE`/`EPIPE`, `O_NONBLOCK`
- [`pipe(2)`](https://man7.org/linux/man-pages/man2/pipe.2.html), [`fifo(7)`](https://man7.org/linux/man-pages/man7/fifo.7.html), [`mkfifo(3)`](https://man7.org/linux/man-pages/man3/mkfifo.3.html), [`popen(3)`](https://man7.org/linux/man-pages/man3/popen.3.html), [`fcntl(2)`](https://man7.org/linux/man-pages/man2/fcntl.2.html)
- [Piping and Redirection](http://ryanstutorials.net/linuxtutorial/piping.php), Ryan's Tutorials. հոսքեր, redirection, pipeline-ներ քայլ առ քայլ
- [GNU grep manual: Regular Expressions](https://www.gnu.org/software/grep/manual/html_node/Regular-Expressions.html), [Basic vs Extended](https://www.gnu.org/software/grep/manual/html_node/Basic-vs-Extended.html), [Exit Status](https://www.gnu.org/software/grep/manual/html_node/Exit-Status.html)
- [Regular expressions in Linux](http://www.linuxnix.com/regular-expressions-linux-i/), LinuxNix
- [Using grep and regular expressions](https://www.digitalocean.com/community/tutorials/using-grep-regular-expressions-to-search-for-text-patterns-in-linux), DigitalOcean
- [Introduction to Linux](http://www.bu.edu/tech/files/2012/09/Introduction-to-Linux.pptx), BU IS&T, սլայդներ 30, 45–49. pipe, `awk`, `sed`
- [`regex(7)`](https://man7.org/linux/man-pages/man7/regex.7.html), [`regcomp(3)`](https://man7.org/linux/man-pages/man3/regcomp.3.html), [GNU sed manual](https://www.gnu.org/software/sed/manual/sed.html), [GNU awk manual](https://www.gnu.org/software/gawk/manual/gawk.html)
- Հին դասի հղումներ, այժմ հասանելի չեն. [Regular expressions](http://www.robelle.com/smugbook/regexpr.html) (Robelle), [Streams, pipes and redirects](http://www.hydro.washington.edu/~nijssen/computing_workshops/streams_pipes_redirects.pdf)

## Hands-On Task · Գործնական առաջադրանք

Pipe-երի լաբորատորիա. [`template.cpp`](template.cpp)։ Ամեն TODO փորձ է, որ ինքդ չափում ես pipe-ի մեկ հատկություն.

| TODO | Փորձ | Ինչ պետք է տեսնես |
|---|---|---|
| 1 | Գրել `O_NONBLOCK`-ով մինչև `EAGAIN` | Buffer-ը 16 էջ է (65536) |
| 2 | `F_SETPIPE_SZ` 1 MiB և 100000 | 1048576 և 131072 |
| 3 | Երեխան գրում է 1 MB, ծնողը կարդում է | Բոլոր բայթերը հասնում են 64 KiB buffer-ով |
| 4 | Գրել, երբ կարդացող չկա | `EPIPE` |
| 5 | `sort`, `wc -l`, `tr`-ին տալ input և կարդալ ելքը | Երկու pipe, `dup2`, `exec` |

```bash
g++ -std=c++17 -Wall -Wextra -O2 template.cpp -o day04 && ./day04
```

Գործարկիր նույն ծրագիրը laptop-ի, Jetson-ի և Raspberry Pi-ի վրա։ Համեմատիր TODO 1-ի արդյունքը և բացատրիր տարբերությունը (`getconf PAGESIZE`)։

## Self-Learning · Ինքնուրույն աշխատանք
1. TODO 3-ում հեռացրու ծնողի `close(fds[1])`-ը։ Ինչ է պատահում։ Ծրագիրը կախված ժամանակ մեկ այլ terminal-ում գտիր նրա PID-ը, նայիր `ls -l /proc/PID/fd`-ը և `cat /proc/PID/wchan`-ը, և բացատրիր, թե ինչու է այն սպասում։
2. TODO 3-ում երեխան թող գրի 1 MB առանց `write_all`-ի, մեկ `write`-ով։ Ինչ է վերադարձնում `write`-ը։ Իսկ եթե pipe-ը `O_NONBLOCK` է։
3. Գրիր ծրագիր, որտեղ 4 երեխա միաժամանակ գրում են նույն pipe-ում 1000-ական տող, իսկ ծնողը ստուգում է, որ ամեն տող ամբողջական է։ Փորձիր տողի երկարությունը 100, 4096 և 5000 բայթ։ Երբ են տողերը սկսում խառնվել։ Կապիր `PIPE_BUF`-ի հետ։
4. TODO 5-ում փոխանցիր `cat`-ին 1 MB input։ Ծրագիրը կախվում է (deadlock)։ Բացատրիր, թե ինչու `sort`-ի հետ նույն չափը կարող է աշխատել, իսկ `cat`-ի հետ՝ ոչ։ Ուղղիր երկու ձևով. (ա) գրողը թող լինի առանձին thread կամ երեխա, (բ) `poll()`-ով սպասիր, թե որ ծայրն է պատրաստ (Օր 11)։
5. Չափիր 1 GB-ի փոխանցման ժամանակը pipe-ով (`dd if=/dev/zero bs=1M count=1024 | cat > /dev/null`), ապա քո ծրագրով 64 KiB և 1 MiB buffer-ով։ Կա տարբերություն։
6. Ստեղծիր FIFO և կազմակերպիր «չաթ» երկու terminal-ի միջև. մեկ FIFO յուրաքանչյուր ուղղության համար։ Ինչու է պետք երկու FIFO, և ինչ կլինի, եթե երկու կողմն էլ նախ բացեն գրելու համար։
7. Գրիր `grep`-ի փոքր տարբերակ POSIX regex-ով (`regcomp`, `regexec`) `-i`, `-v`, `-n`, `-c`, `-E` option-ներով, և համեմատիր իրական `grep`-ի հետ. `diff <(./mygrep -n -i root /etc/passwd) <(grep -n -i root /etc/passwd)`։ Exit status-ը պետք է լինի նույնը. 0, 1, 2։
8. Տպիր միայն `/etc`-ի վերջից 20-րդ ֆայլը՝ մեկ pipeline-ով։
9. `/etc/passwd`-ից տպիր օգտատերերի անունները և shell-երը, դասավորված ըստ shell-ի, և հաշվիր, քանի օգտատեր ունի յուրաքանչյուր shell։
10. Գրիր regex, որը համընկնում է միայն վավեր IPv4 հասցեների հետ (յուրաքանչյուր թիվ 0–255)։ Ստուգիր `grep -E -x`-ով `0.0.0.0`, `192.168.1.1`, `256.1.1.1`, `1.2.3`, `01.2.3.4` տողերի վրա։
11. `sed`-ով CSV ֆայլում փոխիր ամսաթվերը `DD.MM.YYYY`-ից `YYYY-MM-DD` ձևաչափի։
12. Գրիր `awk` ծրագիր, որը `ps aux`-ի ելքից հաշվում է հիշողության ընդհանուր օգտագործումը (`%MEM` սյունակ) ըստ օգտատիրոջ։

## Self-Check · Ինքնաստուգում
Պատասխաններ չկան։ Մտածիր ինքնուրույն։

1. Ինչով է pipe-ը տարբերվում ֆայլից։ Կարելի՞ է pipe-ից նույն տվյալները կարդալ երկու անգամ։
2. `pipe(fds)`-ից հետո ո՞ր ծայրն է `fds[0]`-ն, և ինչու է հեշտ հիշել։
3. `fork`-ից հետո քանի՞ file descriptor է ցույց տալիս pipe-ի գրելու ծայրը։ Ինչ պետք է անեն ծնողը և երեխան։
4. Ծնողը կարդում է pipe-ից, երեխան ավարտվել է, բայց `read`-ը չի վերադառնում։ Որն է ամենահավանական պատճառը։
5. `read`-ը խնդրել է 4096 բայթ և ստացել 17։ Սա սխալ է։ Ինչ պետք է անի ծրագիրը։
6. Ինչ է պատահում, երբ գրողը արագ է, իսկ կարդացողը դանդաղ։ Ինչու դա լավ է։
7. Ինչու է `yes | head -3`-ը ավարտվում։ Ինչ exit status ունի `yes`-ը և ինչու։
8. Ինչ է երաշխավորում `PIPE_BUF`-ը։ Ե՞րբ դա կարևոր չէ։
9. Ինչու է նույն ծրագիրը Raspberry Pi 5-ի վրա ցույց տալիս այլ buffer-ի չափ, քան laptop-ի վրա։
10. Ծրագիրը գրում է մեկ այլ ծրագրի stdin-ը և կարդում նրա stdout-ը։ Փոքր տվյալներով աշխատում է, մեծերով կախվում է։ Ինչու։
11. Ինչ է անում `dup2(fds[1], 1)`-ը, և ինչու `exec`-ից հետո նոր ծրագիրը շարունակում է գրել pipe-ում։
12. Ինչով է FIFO-ն տարբերվում սովորական pipe-ից։ Ինչու է `open`-ը FIFO-ի վրա սպասում։
13. Ինչու՞ է `sort`-ը գրեթե միշտ `uniq`-ից առաջ։
14. Ինչ տարբերություն կա `*`-ի իմաստի մեջ shell-ում (`ls *.txt`) և regex-ում (`grep 'a*'`)։
15. `grep '1.5'`-ը կգտնի՞ `1x5`-ը։ Ինչպես գտնել միայն `1.5`-ը։
16. `find . -name *.txt`-ը երբեմն աշխատում է, երբեմն՝ ոչ։ Ինչու՞, և ինչպես ճիշտ գրել։

## Code Template · Կոդի ձևանմուշ
Տե՛ս [`template.cpp`](template.cpp)-ը։

```bash
g++ -std=c++17 -Wall -Wextra -O2 template.cpp -o day04
./day04
```
