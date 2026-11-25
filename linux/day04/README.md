# Day 4: Pipes, Filters and Regular Expressions · Օր 4. Pipe-եր, ֆիլտրեր և ռեգուլյար արտահայտություններ

## Objectives · Նպատակներ
- Բացատրել, թե ինչ է pipe-ը kernel-ի տեսանկյունից, և կառուցել pipeline-ներ քայլ առ քայլ
- Օգտագործել ֆիլտրերը. `sort`, `uniq`, `wc`, `cut`, `tr`, `head`, `tail`, `tee`, `xargs`
- Գրել ռեգուլյար արտահայտություններ. anchor-ներ, bracket expression-ներ, quantifier-ներ, խմբեր, back-reference-ներ
- Տարբերել basic (BRE) և extended (ERE) ռեգուլյար արտահայտությունները
- Փնտրել ֆայլեր `find`-ով, տեքստ՝ `grep`-ով, փոխել տեքստը `sed`-ով, մշակել սյունակներ `awk`-ով
- Օգտագործել ռեգուլյար արտահայտությունները C/C++ ծրագրում (`regcomp`, `regexec`)

## Key Concepts · Հիմնական գաղափարներ
- Pipe, pipeline, filter
- `tee`, `xargs`, here-document
- Regular expression, metacharacter, anchor, bracket expression, POSIX character class
- Quantifier, group, alternation, back-reference
- BRE և ERE, `grep -E`, `grep -F`
- `grep`-ի exit status-ը
- `find`, `sed`, `awk`

## Definitions · Սահմանումներ

*Այսօր ներմուծվող տերմինները։ Նույն տեքստը կա [GLOSSARY.md](../GLOSSARY.md)-ում։*

**Pipe** — Kernel-ի buffer, որի մի ծայրից մի process-ը գրում է, մյուսից մյուս process-ը կարդում է։ Shell-ում `a | b`-ն `a`-ի stdout-ը (fd 1) միացնում է `b`-ի stdin-ին (fd 0)։ Երկու ծրագիրն աշխատում են **միաժամանակ**։

**Pipeline** — Pipe-երով միացված հրամանների շղթա. `cat log | grep error | sort | uniq -c`։

**Filter** — Ծրագիր, որը կարդում է stdin-ը, ինչ-որ բան է անում տվյալների հետ և գրում stdout. `grep`, `sort`, `uniq`, `wc`, `cut`, `tr`, `sed`, `awk`։

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

## Commands · Հրամաններ

*Այսօրվա հրամանները։ Բոլոր օրերի հրամանների ցանկը [COMMANDS.md](../COMMANDS.md)-ում է։*

```bash
# Pipeline-ներ
ls | head -3 | tail -1                     # երրորդ ֆայլը
ls -l /etc | tail -n +2 | sort -k5 -n      # առանց առաջին տողի, ըստ չափի
cmd | tee out.txt | wc -l                  # tee. պատճենել ֆայլ և շարունակել pipe-ը
find . -name '*.log' -print0 | xargs -0 rm # xargs. stdin-ի տողերը դարձնել argument-ներ

# Ֆիլտրեր
sort -n -r -k2 ; uniq -c ; wc -l -w -c
cut -d: -f1,7 /etc/passwd                  # 1-ին և 7-րդ դաշտերը, : բաժանիչով
tr 'a-z' 'A-Z' ; tr -d '\r' ; tr -s ' '    # փոխարինել / ջնջել / սեղմել նիշերը
sort | uniq -c | sort -rn | head           # ամենահաճախ հանդիպող տողերը

# grep
grep root /etc/passwd
grep -i -n -w -c -v -o -l -r 'pattern' dir # անտեսել case / համար / ամբողջ բառ / քանակ / շրջել / միայն համընկնումը / ֆայլերի անունները / ռեկուրսիվ
grep -E '^(root|daemon):' /etc/passwd      # ERE
grep -A2 -B1 error log.txt                 # 2 տող հետո, 1 տող առաջ
grep -F 'a.b*c' file                       # առանց regex. բառացի տող

# find
find ~ -name '*.cpp' -type f
find /var/log -size +1M -mtime -7          # 1 MB-ից մեծ, վերջին 7 օրում փոխված
find . -name '*.o' -delete
find . -type f -exec wc -l {} +            # հրաման բոլոր գտնվածների վրա

# sed
sed 's/rain/snow/' f.txt                   # առաջին համընկնումը յուրաքանչյուր տողում
sed 's/rain/snow/g' f.txt                  # բոլորը
sed -i.bak 's/old/new/g' f.txt             # փոխել ֆայլը տեղում, պահուստը f.txt.bak
sed -n '10,20p' f.txt ; sed '/^#/d' f.txt  # տպել 10–20 տողերը / ջնջել մեկնաբանությունները
sed -E 's/([0-9]+)-([0-9]+)/\2-\1/' f.txt  # խմբեր և back-reference

# awk
awk '{print $1}' f.txt                     # առաջին դաշտը
awk -F: '{print $1, $7}' /etc/passwd       # : բաժանիչով
awk '$5 > 1000000 {print $9}'              # պայմանով
ls -l /usr/lib | awk '{s += $5} END {print s / 1e6, "MB"}'
```

## Functions · Ֆունկցիաներ

*Այսօրվա C ֆունկցիաները։ Ամբողջական ցանկը [API.md](../API.md)-ում է։*

```c
#include <regex.h>
// Կոմպիլյացնել regex-ը. cflags. REG_EXTENDED (ERE), REG_ICASE (անտեսել case-ը),
// REG_NOSUB (միայն «կա՞ համընկնում»), REG_NEWLINE. 0՝ հաջող, այլապես սխալի կոդ
int regcomp(regex_t *preg, const char *regex, int cflags);
// Փնտրել համընկնում տողում. 0՝ գտավ, REG_NOMATCH՝ չգտավ.
// pmatch[0]՝ ամբողջ համընկնման սկիզբը և վերջը, pmatch[1..]՝ խմբերինը
int regexec(const regex_t *preg, const char *string, size_t nmatch, regmatch_t pmatch[], int eflags);
// Սխալի կոդը դարձնել տեքստ. "Unmatched ( or \\(" և այլն
size_t regerror(int errcode, const regex_t *preg, char *errbuf, size_t errbuf_size);
// Ազատել regcomp-ի հատկացրած հիշողությունը
void regfree(regex_t *preg);
```

C++-ում կա նաև `std::regex` (`<regex>`), բայց այն դանդաղ է և ունի իր սեփական գրելաձևը։ `grep`, `sed` և այլ Unix գործիքների հետ համատեղելի լինելու համար օգտագործում ենք POSIX `regcomp`/`regexec`-ը։

---

## 1. Pipe-եր

### Visual
![Pipeline. ls, grep, wc երեք առանձին process են, որոնք աշխատում են միաժամանակ. ls-ի fd 1-ը գրում է kernel-ի pipe buffer-ի մեջ, grep-ի fd 0-ն կարդում է նրանից. stderr-ը (fd 2) չի անցնում pipe-ով և գնում է terminal. buffer-ի չափը 64 KB է. գրողը սպասում է, երբ buffer-ը լիքն է, կարդացողը՝ երբ դատարկ է](pipeline.svg)

Օր 2-ում տեսանք `>`-ը, `>>`-ը, `<`-ը և `2>`-ը։ **Pipe**-ը `|` մի հրամանի stdout-ը տալիս է մյուսի stdin-ին։ Ինչ է իրականում կատարվում.

1. Shell-ը kernel-ից խնդրում է **pipe**. buffer երկու ծայրով (գրելու և կարդալու)։
2. Ստեղծում է երկու process։ Առաջինի fd 1-ը միացնում է գրելու ծայրին, երկրորդի fd 0-ն՝ կարդալու։
3. Երկու ծրագիրը աշխատում են **միաժամանակ**։ Եթե buffer-ը լիքն է (Linux-ում 64 KB), գրողը սպասում է. եթե դատարկ է, կարդացողը սպասում է։
4. Երբ գրողն ավարտվում է, կարդացողը ստանում է EOF (Օր 3)։ Եթե կարդացողն է ավարտվում առաջինը, գրողը ստանում է `SIGPIPE` signal և նույնպես ավարտվում է։

Վերջին կետը բացատրում է, թե ինչու `yes | head -3`-ը ավարտվում է ակնթարթորեն, թեև `yes`-ը անվերջ է տպում։

Ծրագիրը չգիտի, որ իր stdout-ը pipe է. նա պարզապես գրում է fd 1-ում։ Սա Unix-ի փիլիսոփայության երկրորդ կանոնն է (Օր 1). ամեն ծրագրի ելքը կարող է դառնալ ուրիշի մուտք։

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

## 2. Ֆիլտրեր

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

## 3. Ռեգուլյար արտահայտություններ

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

## 4. `find`. ֆայլերի որոնում

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

## 5. `sed`

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

## 6. `awk`

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

## 7. Ամեն ինչ միասին

```bash
# Ո՞ր IP հասցեներից են ամենաշատ հարցումները web սերվերի log-ում
awk '{print $1}' access.log | sort | uniq -c | sort -rn | head -5

# Քանի՞ անգամ է հանդիպում յուրաքանչյուր բառը ֆայլում
tr -cs '[:alpha:]' '\n' < book.txt | tr 'A-Z' 'a-z' | sort | uniq -c | sort -rn | head

# Որ ֆայլերում կա TODO, և քանի հատ
grep -r -c TODO src/ | grep -v ':0$' | sort -t: -k2 -rn
```

---

## Resources · Աղբյուրներ
- [Piping and Redirection](http://ryanstutorials.net/linuxtutorial/piping.php), Ryan's Tutorials. հոսքեր, redirection, pipeline-ներ քայլ առ քայլ
- [GNU grep manual: Regular Expressions](https://www.gnu.org/software/grep/manual/html_node/Regular-Expressions.html), [Basic vs Extended](https://www.gnu.org/software/grep/manual/html_node/Basic-vs-Extended.html), [Exit Status](https://www.gnu.org/software/grep/manual/html_node/Exit-Status.html)
- [Regular expressions in Linux](http://www.linuxnix.com/regular-expressions-linux-i/), LinuxNix
- [Using grep and regular expressions](https://www.digitalocean.com/community/tutorials/using-grep-regular-expressions-to-search-for-text-patterns-in-linux), DigitalOcean
- [Introduction to Linux](http://www.bu.edu/tech/files/2012/09/Introduction-to-Linux.pptx), BU IS&T, սլայդներ 30, 45–49. pipe, `awk`, `sed`
- [`regex(7)`](https://man7.org/linux/man-pages/man7/regex.7.html), [`regcomp(3)`](https://man7.org/linux/man-pages/man3/regcomp.3.html), [`pipe(7)`](https://man7.org/linux/man-pages/man7/pipe.7.html), [GNU sed manual](https://www.gnu.org/software/sed/manual/sed.html), [GNU awk manual](https://www.gnu.org/software/gawk/manual/gawk.html)
- Հին դասի հղումներ, այժմ հասանելի չեն. [Regular expressions](http://www.robelle.com/smugbook/regexpr.html) (Robelle), [Streams, pipes and redirects](http://www.hydro.washington.edu/~nijssen/computing_workshops/streams_pipes_redirects.pdf)

## Hands-On Task · Գործնական առաջադրանք

Գրիր `grep`-ի փոքր տարբերակը. [`template.cpp`](template.cpp)։

```bash
./day04 root /etc/passwd                 # տողեր, որոնք պարունակում են root
./day04 -n -i ROOT /etc/passwd           # տողի համարով, անտեսելով case-ը
./day04 -E -c '^(root|daemon):' /etc/passwd
cat /etc/passwd | ./day04 -v nologin     # stdin-ից, տողեր առանց nologin-ի
./day04 bash /etc/passwd /etc/shells     # մի քանի ֆայլ. ամեն տողի առաջ՝ ֆայլի անունը
./day04 'a(' /etc/passwd ; echo $?       # սխալ regex → stderr, exit status 2
```

Լրացրու TODO-ները. option-ները, `regcomp`-ը, տողերի ցիկլը `regexec`-ով, ֆայլերի ցիկլը։ Exit status-ը պետք է լինի նույնը, ինչ `grep`-ինը. 0, 1, 2։ Ստուգիր իրական `grep`-ի հետ.

```bash
diff <(./day04 -n -i root /etc/passwd) <(grep -n -i root /etc/passwd) && echo same
```

## Self-Learning · Ինքնուրույն աշխատանք
1. Տպիր միայն `/etc`-ի վերջից 20-րդ ֆայլը՝ մեկ pipeline-ով։
2. Հաշվիր, քանի ֆայլ և թղթապանակ կա քո home-ում, որոնք դու կարող ես կատարել (`x` իրավունք owner-ի համար)։
3. `/etc/passwd`-ից տպիր օգտատերերի անունները և shell-երը, դասավորված ըստ shell-ի, և հաշվիր, քանի օգտատեր ունի յուրաքանչյուր shell։
4. Գրիր regex, որը համընկնում է միայն վավեր IPv4 հասցեների հետ (յուրաքանչյուր թիվ 0–255)։ Ստուգիր `grep -E -x`-ով `0.0.0.0`, `192.168.1.1`, `256.1.1.1`, `1.2.3`, `01.2.3.4` տողերի վրա։
5. `sed`-ով CSV ֆայլում փոխիր ամսաթվերը `DD.MM.YYYY`-ից `YYYY-MM-DD` ձևաչափի։
6. Գրիր `awk` ծրագիր, որը `ps aux`-ի ելքից հաշվում է հիշողության ընդհանուր օգտագործումը (`%MEM` սյունակ) ըստ օգտատիրոջ։
7. Համեմատիր `yes | head -1` և `yes > /dev/null & sleep 1; kill %1`. ինչու է առաջինը ավարտվում ինքնուրույն։ `strace -f -e trace=write,kill bash -c 'yes | head -1'`-ով գտիր `SIGPIPE`-ը։
8. Ընդլայնիր template-ը `-o` option-ով. տպել միայն համընկած մասերը (`regmatch_t`, ցիկլով նույն տողում)։

## Self-Check · Ինքնաստուգում
Պատասխաններ չկան։ Մտածիր ինքնուրույն։

1. `cmd1 | cmd2`-ում հրամանները աշխատում են հերթով, թե միաժամանակ։ Ինչ է կատարվում, երբ `cmd2`-ը կարդում է ավելի դանդաղ, քան `cmd1`-ը գրում է։
2. Ինչու՞ `yes | head -3`-ը ավարտվում է, եթե `yes`-ը երբեք չի կանգնում։
3. `cmd | grep error`-ը չի գտնում սխալները, թեև դրանք երևում են էկրանին։ Ինչու՞, և ինչպես ուղղել։
4. Ինչու՞ է `sort`-ը գրեթե միշտ `uniq`-ից առաջ։
5. Ինչ տարբերություն կա `*`-ի իմաստի մեջ shell-ում (`ls *.txt`) և regex-ում (`grep 'a*'`)։
6. `ls | grep '[^abc]'`-ը ինչ անուններ կտպի։ Կբացառի՞ արդյոք `a`, `b` կամ `c` պարունակող անունները։ Ինչպես գրել regex, որը իրականում բացառում է դրանք։
7. `grep '1.5'`-ը կգտնի՞ `1x5`-ը։ Ինչպես գտնել միայն `1.5`-ը։
8. `grep 'a+'` և `grep -E 'a+'`. ինչով են տարբերվում։
9. Ինչ կտպի `grep -c ''` ֆայլի վրա։ Իսկ `grep -c '^'`-ը։
10. Script-ում `if grep -q user /etc/passwd; then ...`. ինչպես է `if`-ը իմանում արդյունքը, եթե `grep -q`-ն ոչինչ չի տպում։
11. `find . -name *.txt`-ը երբեմն աշխատում է, երբեմն՝ ոչ։ Ինչու՞, և ինչպես ճիշտ գրել։
12. `sed -i`-ը փոխում է ֆայլը «տեղում»։ Իրականում ինչ է անում այն inode-ի հետ (Օր 3)։ Ստուգիր `ls -i`-ով առաջ և հետո։

## Code Template · Կոդի ձևանմուշ
Տե՛ս [`template.cpp`](template.cpp)-ը։

```bash
g++ -std=c++17 -Wall -Wextra -O2 template.cpp -o day04
./day04 -n root /etc/passwd
```
