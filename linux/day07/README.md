# Day 7: Users, Ownership and Permissions · Օր 7. Օգտատերեր, սեփականություն և իրավունքներ

## Objectives · Նպատակներ
- Բացատրել UID-ը, GID-ը, `root`-ը և կարդալ `/etc/passwd`-ը, `/etc/group`-ը, `/etc/shadow`-ը
- Ստեղծել և կառավարել օգտատերեր և group-եր, օգտագործել `sudo`-ն և `su`-ն
- Բացատրել, թե ինչպես է kernel-ը որոշում՝ թույլ տալ դիմումը ֆայլին, թե ոչ
- Տարբերել `r`, `w`, `x` իրավունքների իմաստը ֆայլերի և թղթապանակների համար
- Փոխել իրավունքները և owner-ին. `chmod` (symbolic և octal), `chown`, `chgrp`, `umask`
- Բացատրել հատուկ բիթերը. setuid, setgid, sticky, և real/effective UID-ը
- Ճանաչել տիպիկ անվտանգության խնդիրները և ապահովել ֆայլերն ու թղթապանակները

## Key Concepts · Հիմնական գաղափարներ
- UID, GID, superuser (`root`, UID 0)
- `/etc/passwd`, `/etc/shadow`, `/etc/group`
- `useradd`, `usermod`, `passwd`, `sudo`, `su`
- Owner / group / other, `rwx`, octal նշում
- Իրավունքները թղթապանակների վրա
- `umask`
- setuid, setgid, sticky bit
- Real և effective UID
- Least privilege, անվտանգության անցքեր

## Definitions · Սահմանումներ

*Այսօր ներմուծվող տերմինները։ Նույն տեքստը կա [GLOSSARY.md](../GLOSSARY.md)-ում։*

**UID (user ID)** — Օգտատիրոջ համարը։ Համակարգը աշխատում է միայն համարներով. անունը (`student`) միայն մարդկանց համար է։ Երկու անուն նույն UID-ով համակարգի համար նույն օգտատերն են։

**GID (group ID)** — Group-ի համարը։ Group-ը պարունակում է օգտատերեր (բայց ոչ այլ group-եր), իսկ մեկ օգտատերը կարող է լինել շատ group-երում։

**root (superuser)** — UID 0 ունեցող օգտատերը։ Նրա process-ները կարող են կարդալ և փոխել ցանկացած ֆայլ, ստեղծել օգտատերեր, անջատել ցանցը. kernel-ը նրա համար իրավունքներ չի ստուգում։

**`/etc/passwd`** — Օգտատերերի ցանկը. անուն, UID, GID, home թղթապանակ, shell։ Բոլորը կարող են կարդալ։ Գաղտնաբառերն այստեղ չեն։

**`/etc/shadow`** — Գաղտնաբառերի **hash**-երը։ Կարդալ կարող է միայն `root`-ը։

**`sudo`** — Կատարել մեկ հրաման այլ օգտատիրոջ (լռելյայն՝ `root`-ի) իրավունքներով, եթե `/etc/sudoers`-ը դա թույլ է տալիս։ Գաղտնաբառը՝ քո սեփականը։

**`su`** — Դառնալ այլ օգտատեր (լռելյայն՝ `root`)՝ նրա գաղտնաբառով։

**`umask`** — Բիթերի դիմակ, որը հանվում է նոր ֆայլերի իրավունքներից։ `umask 022`-ի դեպքում նոր ֆայլը ստանում է `644` (`666 & ~022`), նոր թղթապանակը՝ `755`։

**setuid bit** — Կատարվող ֆայլի բիթ. ծրագիրը աշխատում է **ֆայլի owner-ի** իրավունքներով, ոչ թե գործարկողի։ `ls -l`-ում owner-ի `x`-ի տեղում՝ `s`։ Օրինակ՝ `/usr/bin/passwd`-ը, որը պետք է փոխի `root`-ին պատկանող `/etc/shadow`-ը։

**setgid bit** — Ֆայլի վրա. ծրագիրը աշխատում է ֆայլի group-ի իրավունքներով։ Թղթապանակի վրա. նրանում ստեղծված ֆայլերը ժառանգում են թղթապանակի group-ը։ `ls -l`-ում group-ի `x`-ի տեղում՝ `s`։

**Sticky bit** — Թղթապանակի բիթ. ֆայլը ջնջել կամ վերանվանել կարող է միայն նրա owner-ը (կամ թղթապանակի owner-ը, կամ `root`-ը), թեև թղթապանակում գրել կարող են բոլորը։ `/tmp`-ն ունի այն. `ls -l`-ում other-ի `x`-ի տեղում՝ `t`։

**Real / effective UID** — Յուրաքանչյուր process ունի երկու UID. **real**՝ ով գործարկեց, **effective**՝ որի իրավունքներով են ստուգվում դիմումները։ Սովորաբար նույնն են. setuid ծրագրում effective-ը ֆայլի owner-ն է։

**Least privilege** — Անվտանգության սկզբունք. ծրագիրը և օգտատերը պետք է ունենան միայն այն իրավունքները, որոնք անհրաժեշտ են աշխատանքի համար, և ոչ ավելին։

---

## 1. Օգտատերեր և group-եր

Linux-ը multiuser համակարգ է (Օր 1)։ Յուրաքանչյուր օգտատեր ունի եզակի **UID**, յուրաքանչյուր group՝ **GID**։ Համակարգը աշխատում է միայն համարներով. անունները մարդկանց համար են.

```bash
id
# uid=1000(student) gid=1000(student) groups=1000(student),4(adm),27(sudo),20(dialout)
```

`whoami`-ն տպում է միայն անունը, `id anna`-ն՝ այլ օգտատիրոջ տվյալները։

`gid`-ը process-ի ընթացիկ (հիմնական) group-ն է, `groups`-ը՝ բոլոր group-երը, որոնց անդամ է օգտատերը։

Օգտատերերը նկարագրված են երեք տեքստային ֆայլում.

```bash
grep student /etc/passwd
# student:x:1000:1000:Student,,,:/home/student:/bin/bash
#  անուն  │  UID  GID  մեկնաբ.     home          shell
#         └ x = գաղտնաբառը /etc/shadow-ում է

sudo grep student /etc/shadow
# student:$y$j9T$...hash...:20050:0:99999:7:::        ← միայն root-ը կարող է կարդալ

grep lab /etc/group
# lab:x:1001:anna,aram                                ← group-ի անդամները
```

Ֆայլերը կարդալու փոխարեն ավելի ճիշտ է հարցնել `getent`-ին. `getent passwd student`, `getent group sudo`։ Այն գտնում է օգտատիրոջը, նույնիսկ եթե նա `/etc/passwd`-ում չէ, այլ, օրինակ, համալսարանի LDAP սերվերում։

| UID | Ով |
|---|---|
| 0 | `root` |
| 1–999 | Համակարգային օգտատերեր ծառայությունների համար. `www-data`, `sshd`, `systemd-network`. նրանք չունեն մուտքի գաղտնաբառ և shell |
| 1000+ | Սովորական օգտատերեր |

**root**-ը կարող է ամեն ինչ։ Սա և ուժն է, և վտանգը. ցանկացած ծրագիր, որը աշխատում է `root`-ով և սխալ ունի, կարող է վնասել ամբողջ համակարգը։ Այդ պատճառով ժամանակակից distribution-ներում `root`-ով ուղիղ մուտք չեն գործում, այլ օգտագործում են `sudo`-ն մեկ հրամանի համար։

## 2. Օգտատերերի կառավարում

```bash
sudo adduser anna                    # ստեղծում է օգտատեր, group, home թղթապանակ, հարցնում է գաղտնաբառը
sudo groupadd lab                    # նոր group
sudo usermod -aG lab anna            # ավելացնել anna-ին lab group-ին. -a-ն պարտադիր է
groups anna
sudo passwd anna                     # փոխել գաղտնաբառը
sudo usermod -L anna                 # արգելափակել
sudo deluser --remove-home anna      # ջնջել
```

`adduser`-ը Debian/Ubuntu-ի հարմար script-ն է։ Նրա տակ աշխատում է ցածր մակարդակի `useradd`-ը, որը կա բոլոր distribution-ներում, բայց առանց option-ների չի ստեղծում home թղթապանակ. `sudo useradd -m -s /bin/bash aram`։ Group-ից հեռացնել՝ `sudo gpasswd -d anna lab`։

Group-ի փոփոխությունը կիրառվում է **հաջորդ մուտքից**. արդեն աշխատող process-ները պահում են հին group-երը (կամ `newgrp lab` նույն terminal-ում)։

**`sudo` և `su`**.

| | `sudo cmd` | `su -` |
|---|---|---|
| Ինչ է անում | Մեկ հրաման root-ով | Նոր shell root-ով |
| Գաղտնաբառը | **Քո**ը | **root**-ինը |
| Ով կարող է | Ով նշված է `/etc/sudoers`-ում (Ubuntu-ում՝ `sudo` group-ը) | Ով գիտի root-ի գաղտնաբառը |
| Log | Ամեն հրաման գրանցվում է (`/var/log/auth.log`) | Միայն մուտքը |

Ով և երբ է մուտք գործել. `last` (վերջին մուտքերը), `lastlog` (յուրաքանչյուր օգտատիրոջ վերջին մուտքը), իսկ `sudo`-ի բոլոր հրամանները՝ `sudo tail /var/log/auth.log`-ում։

`/etc/sudoers`-ը խմբագրիր միայն `sudo visudo`-ով. այն ստուգում է syntax-ը պահելուց առաջ. սխալ ֆայլը կարող է զրկել քեզ `sudo`-ից ընդհանրապես։

## 3. Ինչպես է kernel-ը ստուգում իրավունքները

### Visual
![Kernel-ի որոշումը, երբ process-ը բացում է ֆայլը. effective UID-ը 0 է (root)՝ թույլատրել (x-ի համար պետք է գոնե մեկ x բիթ). UID-ը հավասար է ֆայլի owner-ին՝ ստուգել owner-ի rwx-ը. process-ի group-երից մեկը հավասար է ֆայլի group-ին՝ ստուգել group-ի rwx-ը. հակառակ դեպքում՝ other-ի rwx-ը. միայն մեկ դաս է ստուգվում](permission_check.svg)

Ամեն process ունի UID և GID-եր (սովորաբար նրան գործարկած օգտատիրոջինը)։ Երբ process-ը կանչում է `open()`, kernel-ը.

1. Եթե effective UID-ը 0 է (**root**)՝ թույլատրում է կարդալը և գրելը։ Կատարելը (`x`)՝ միայն եթե ֆայլն ունի գոնե մեկ `x` բիթ։
2. Եթե UID-ը հավասար է ֆայլի **owner**-ին՝ ստուգում է **միայն** owner-ի `rwx`-ը։
3. Այլապես, եթե process-ի group-երից մեկը հավասար է ֆայլի **group**-ին՝ ստուգում է **միայն** group-ի `rwx`-ը։
4. Այլապես՝ **other**-ի `rwx`-ը։

Կարևոր է. ստուգվում է **միայն մեկ** դաս, առաջինը, որին process-ը պատկանում է։ Եթե ֆայլը `----r--r--` է, owner-ը **չի** կարող կարդալ այն, թեև բոլորը կարող են. owner-ը ստուգվում է owner-ի բիթերով, որոնք դատարկ են։

Բացի այդ, ճանապարհի **բոլոր թղթապանակները** պետք է ունենան `x` իրավունք. եթե `/home/anna`-ն `700` է, ոչ ոք (բացի anna-ից և root-ից) չի կարող կարդալ `/home/anna/public.txt`-ը, ինչ իրավունքներ էլ որ այն ունենա։

### `r`, `w`, `x`-ը թղթապանակների վրա

| Իրավունք | Ֆայլի վրա | Թղթապանակի վրա |
|---|---|---|
| `r` | Կարդալ պարունակությունը | Տեսնել անունների ցանկը (`ls`) |
| `w` | Փոխել պարունակությունը | Ստեղծել, ջնջել, վերանվանել ֆայլեր **նրա մեջ** |
| `x` | Գործարկել որպես ծրագիր | Մտնել (`cd`) և դիմել ֆայլերին անունով |

Վերջին տողից հետևում է անսպասելի բան. **ֆայլը ջնջելու համար ֆայլի իրավունքներ պետք չեն**, պետք է `w` թղթապանակի վրա։ Ջնջելը directory entry-ի հեռացում է (Օր 3), այսինքն՝ թղթապանակի փոփոխություն։

```bash
mkdir d ; touch d/f ; chmod 000 d/f
rm d/f              # rm: remove write-protected regular empty file 'd/f'? y  → ջնջվեց
chmod 500 d ; touch d/g    # Permission denied. թղթապանակում w չկա
chmod 100 d ; ls d         # Permission denied, բայց cat d/known_name աշխատում է
```

## 4. `chmod`, `chown`, `umask`

**Symbolic** ձևը. ում (`u` owner, `g` group, `o` other, `a` բոլորը), ինչ անել (`+`, `-`, `=`), որ իրավունքը.

```bash
chmod u+x script.sh            # owner-ին ավելացնել x
chmod g-w,o-r secret.txt       # group-ից հանել w, other-ից՝ r
chmod a=r readme.txt           # բոլորին՝ ճիշտ r
chmod -R g+rX project/         # ռեկուրսիվ. X-ը x է տալիս միայն թղթապանակներին (և արդեն x ունեցող ֆայլերին)
```

**Octal** ձևը. `r=4`, `w=2`, `x=1`, երեք թիվ՝ owner, group, other.

| Octal | Իրավունքներ | Որտեղ |
|---|---|---|
| `644` | `rw-r--r--` | Սովորական ֆայլ |
| `600` | `rw-------` | Գաղտնի ֆայլ. `~/.ssh/id_ed25519` |
| `755` | `rwxr-xr-x` | Ծրագիր, թղթապանակ |
| `700` | `rwx------` | Անձնական թղթապանակ. `~/.ssh` |
| `775` | `rwxrwxr-x` | Թիմի ընդհանուր թղթապանակ |

Ստուգել արդյունքը կարելի է երկու ձևով միանգամից. `stat -c '%A %a %U %G' secret.txt` տպում է `-rw-r----- 640 anna lab`. symbolic, octal, owner և group։

**Owner-ի փոփոխություն** կարող է անել միայն `root`-ը (այլապես օգտատերը կարող էր «նվիրել» ֆայլեր ուրիշներին և խաբել disk quota-ն).

```bash
sudo chown anna report.txt ; sudo chown anna:lab report.txt ; sudo chown -R anna: project/
chgrp lab report.txt           # group-ը կարող է փոխել owner-ը, եթե ինքն անդամ է այդ group-ի
```

**`umask`**-ը որոշում է նոր ֆայլերի իրավունքները։ Ծրագրերը ստեղծում են ֆայլերը `666`-ով (թղթապանակները՝ `777`-ով), և kernel-ը հանում է `umask`-ի բիթերը.

```bash
umask                    # 0022
touch f ; mkdir d ; ls -ld f d     # -rw-r--r-- f    drwxr-xr-x d
umask 077 ; touch g ; ls -l g      # -rw-------. միայն owner-ը
```

## 5. Հատուկ բիթեր. setuid, setgid, sticky

### Visual
![Երեք հատուկ բիթ. setuid (4000)՝ /usr/bin/passwd -rwsr-xr-x, ծրագիրը աշխատում է owner-ի (root) իրավունքներով և կարող է փոխել /etc/shadow-ը. setgid (2000)՝ թղթապանակի վրա նոր ֆայլերը ժառանգում են group-ը. sticky (1000)՝ /tmp drwxrwxrwt, բոլորը կարող են գրել, բայց ջնջել՝ միայն իրենց ֆայլերը. մեծատառ S կամ T՝ բիթը կա, բայց x չկա](special_bits.svg)

Իրավունքների 9 բիթից բացի կա ևս 3.

| Բիթ | Octal | `ls -l` | Ֆայլի վրա | Թղթապանակի վրա |
|---|---|---|---|---|
| setuid | `4000` | `rwsr-xr-x` | Ծրագիրը աշխատում է **owner-ի** իրավունքներով | — |
| setgid | `2000` | `rwxr-sr-x` | Ծրագիրը աշխատում է **group-ի** իրավունքներով | Նոր ֆայլերը ժառանգում են թղթապանակի group-ը |
| sticky | `1000` | `rwxrwxrwt` | — | Ջնջել կարող է միայն ֆայլի owner-ը |

Եթե բիթը կա, բայց համապատասխան `x`-ը չկա, `ls`-ը տպում է մեծատառ. `S` կամ `T`։

**setuid. ինչպես է սովորական օգտատերը փոխում իր գաղտնաբառը.** Գաղտնաբառերը `/etc/shadow`-ում են, որը կարող է փոխել միայն `root`-ը։ Բայց `passwd` ծրագիրը պատկանում է `root`-ին և ունի setuid բիթ.

```bash
ls -l /usr/bin/passwd /etc/shadow
# -rwsr-xr-x 1 root root   ... /usr/bin/passwd
# -rw-r----- 1 root shadow ... /etc/shadow
```

Երբ `student`-ը գործարկում է `passwd`-ը, process-ի **effective UID**-ը դառնում է 0 (root), իսկ **real UID**-ը մնում է 1000։ Ծրագիրը թույլ է տալիս փոխել միայն real UID-ի գաղտնաբառը։ Այդ պատճառով setuid ծրագրերը պետք է գրվեն չափազանց զգույշ. դրանց ցանկացած սխալ դառնում է root-ի իրավունքներ ստանալու ճանապարհ։

```bash
find / -perm -4000 -type f 2>/dev/null     # քո համակարգի բոլոր setuid ծրագրերը
```

**setgid թղթապանակի վրա. թիմի ընդհանուր թղթապանակ.**

```bash
sudo mkdir /srv/lab ; sudo chgrp lab /srv/lab ; sudo chmod 2775 /srv/lab
# հիմա anna-ի և aram-ի ստեղծած ֆայլերը կունենան lab group-ը, և երկուսն էլ կկարողանան փոխել դրանք
```

**sticky. `/tmp`.** Բոլորը պետք է կարողանան ֆայլեր ստեղծել `/tmp`-ում (`rwx` other-ի համար), բայց չպետք է կարողանան ջնջել ուրիշների ֆայլերը։ Sticky բիթը հենց դա է անում.

```bash
ls -ld /tmp          # drwxrwxrwt
```

## 6. Real և effective UID

Յուրաքանչյուր process ունի մի քանի UID.

- **Real UID**՝ ով է գործարկել process-ը։
- **Effective UID**՝ որի իրավունքներով են ստուգվում դիմումները ֆայլերին։

Սովորաբար դրանք նույնն են։ setuid ծրագրում effective-ը ֆայլի owner-ն է։ Process-ը կարող է փոխել դրանք `setreuid`-ով, բայց kernel-ը թույլ է տալիս միայն `root`-ին կամ setuid ծրագրին՝ վերադառնալ իր real UID-ին։ Այսպես են աշխատում `login`-ը և `sshd`-ն. դրանք աշխատում են root-ով, ստուգում են գաղտնաբառը, ապա փոխում երկու UID-ը օգտատիրոջինով և գործարկում նրա shell-ը։

```cpp
std::printf("real uid=%d effective uid=%d\n", getuid(), geteuid());
```

## 7. Ապահովել ֆայլերը և թղթապանակները

**Least privilege.** Աշխատիր սովորական օգտատիրոջով, `sudo` օգտագործիր միայն այն հրամանների համար, որոնց իսկապես պետք է։ Service-ները (web server, database) աշխատում են իրենց համակարգային օգտատերերով (`www-data`, `postgres`), ոչ թե root-ով։

**Գաղտնի ֆայլեր.**

```bash
chmod 700 ~/.ssh ; chmod 600 ~/.ssh/id_ed25519 ~/.ssh/authorized_keys
# sshd-ն կհրաժարվի key-ից, եթե այս իրավունքները ավելի լայն են
```

**Ստուգել համակարգը.**

```bash
find / -perm -4000 -type f 2>/dev/null          # setuid ծրագրեր. սովորական ցանկը փոքր է
find /home -perm -o+w -type f 2>/dev/null       # ֆայլեր, որոնք բոլորը կարող են փոխել
find / -nouser 2>/dev/null                      # ֆայլեր առանց գոյություն ունեցող owner-ի
```

**`x`-ը անվտանգության պատնեշ չէ.** Եթե օգտատերը կարող է **կարդալ** ծրագիրը, նա կարող է պատճենել այն իր թղթապանակ, տալ `x` և գործարկել։ Ծրագրի գործարկումն արգելելու համար պետք է արգելել նաև կարդալը։

**Link-եր և `/tmp`.** Եթե root-ով աշխատող ծրագիրը գրում է `/tmp/report.txt`-ում, հարձակվողը կարող է նախապես ստեղծել `/tmp/report.txt` անունով symlink դեպի `/etc/passwd`, և ծրագիրը կվերագրի այն։ Լուծումը՝ `mkstemp` (Օր 2) և `O_EXCL`, իսկ ժամանակակից kernel-ները ունեն `fs.protected_symlinks` պաշտպանություն։

**ACL և capabilities.** Եթե `rwx`-ի երեք դասը բավական չէ, կարելի է տալ իրավունքներ կոնկրետ օգտատիրոջ՝ **ACL**-ով (`setfacl -m u:anna:r file`, իսկ `getfacl file`-ը ցույց է տալիս դրանք. ACL ունեցող ֆայլի `ls -l`-ում իրավունքներից հետո երևում է `+`)։ Եթե ծրագրին պետք է root-ի իրավունքների մի փոքր մասը (օրինակ՝ `ping`-ին՝ raw socket-ներ), setuid-ի փոխարեն տալիս են **capability** (`getcap /usr/bin/ping`)։

## 8. Տիպիկ անվտանգության անցքեր ծրագրերում

ALP-ի 10.6 բաժինը թվարկում է սխալներ, որոնք հատկապես վտանգավոր են root-ով կամ setuid-ով աշխատող ծրագրերում.

- **Buffer overrun**. ֆիքսված չափի բուֆերի մեջ կարդալ օգտատիրոջ մուտքը առանց չափը ստուգելու (`gets`, `strcpy`, `sprintf`)։ Հարձակվողը կարող է վերագրել stack-ը և կատարել իր կոդը։ Օգտագործիր `std::string`, `fgets`, `snprintf`։
- **Race condition**. ստուգել, ապա օգտագործել (`access()`, ապա `open()`). երկու քայլի միջև ֆայլը կարող է փոխարինվել։ Բաց ֆայլը և ստուգիր արդեն բացվածը։
- **`system()` և `popen()` օգտատիրոջ մուտքով**. `system("grep " + word + " dict")`-ը, երբ `word = "x; rm -rf ~"`, կկատարի երկրորդ հրամանը (**command injection**)։ Օգտագործիր `fork` + `exec` (Օր 10) առանց shell-ի։

---

## Commands · Հրամաններ

*Ամփոփում. այս օրվա հրամանները, որոնք բացատրված են վերևում։ Անգիր անել պետք չէ. վերադարձիր այստեղ, երբ մոռանաս գրելաձևը։ Բոլոր օրերի հրամանների ցանկը [COMMANDS.md](../COMMANDS.md)-ում է։*

```bash
# Ով եմ ես
id ; whoami ; groups ; id anna
getent passwd student ; getent group sudo

# Օգտատերեր և group-եր (root-ի իրավունքներով)
sudo adduser anna                   # ինտերակտիվ (Debian/Ubuntu)
sudo useradd -m -s /bin/bash aram   # ցածր մակարդակի տարբերակ
sudo passwd aram
sudo groupadd lab ; sudo usermod -aG lab anna ; sudo gpasswd -d anna lab
sudo deluser --remove-home aram
sudo -i ; sudo -u anna whoami ; su - anna
sudo visudo                         # խմբագրել /etc/sudoers-ը անվտանգ

# Իրավունքներ
ls -l f ; ls -ld dir ; stat -c '%A %a %U %G' f
chmod u+x f ; chmod g-w,o-r f ; chmod a+r f ; chmod 640 f ; chmod -R g+rX dir
sudo chown anna f ; sudo chown anna:lab f ; chgrp lab f
umask ; umask 027

# Հատուկ բիթեր
ls -l /usr/bin/passwd ; ls -ld /tmp
chmod u+s prog ; chmod g+s shared/ ; chmod +t public/ ; chmod 2775 shared/
find / -perm -4000 -type f 2>/dev/null       # բոլոր setuid ծրագրերը
find / -perm -o+w -type f 2>/dev/null        # ֆայլեր, որտեղ բոլորը կարող են գրել

# Լրացուցիչ
getfacl f ; setfacl -m u:anna:r f            # ACL. իրավունքներ կոնկրետ օգտատիրոջ համար
getcap /usr/bin/ping                         # capabilities. root-ի իրավունքների մի մասը
last ; lastlog ; sudo tail /var/log/auth.log # ով և երբ է մուտք գործել
```

## Functions · Ֆունկցիաներ

*Ամփոփում. այս օրվա C/C++ ֆունկցիաները։ Ամբողջական ցանկը [API.md](../API.md)-ում է։*

```c
#include <unistd.h>
uid_t getuid(void);   uid_t geteuid(void);    // real / effective UID. միշտ հաջող են
gid_t getgid(void);   gid_t getegid(void);    // real / effective GID
// Լրացուցիչ group-երը. վերադարձնում է քանակը. size = 0-ով՝ միայն քանակը
int getgroups(int size, gid_t list[]);
// Ստուգել իրավունքը real UID-ով. mode = R_OK | W_OK | X_OK կամ F_OK (գոյություն). 0 կամ -1
int access(const char *path, int mode);
// Փոխել real և effective UID-ը (միայն root-ը կամ setuid ծրագիրը՝ սահմանափակ)
int setreuid(uid_t ruid, uid_t euid);
int chown(const char *path, uid_t owner, gid_t group);

#include <sys/stat.h>
int chmod(const char *path, mode_t mode);     // mode = S_IRUSR | S_IWUSR | ... կամ 0644
mode_t umask(mode_t mask);                    // նոր դիմակ, վերադարձնում է հինը
// st_mode-ի բիթերը. S_IRUSR S_IWUSR S_IXUSR (owner), S_IRGRP ... (group),
// S_IROTH ... (other), S_ISUID (setuid), S_ISGID (setgid), S_ISVTX (sticky)

#include <pwd.h>
#include <grp.h>
struct passwd *getpwnam(const char *name);    // անուն → UID, GID, home, shell
struct group *getgrgid(gid_t gid);            // GID → անուն և անդամներ
```

## Resources · Աղբյուրներ
- *Advanced Linux Programming*, Mitchell, Oldham, Samuel. Chapter 10 «Security». [`docs/linux_book.pdf`](../docs/linux_book.pdf)
- [Introduction to Linux](http://www.bu.edu/tech/files/2012/09/Introduction-to-Linux.pptx), BU IS&T, սլայդներ 41–44
- [`credentials(7)`](https://man7.org/linux/man-pages/man7/credentials.7.html), [`chmod(1)`](https://man7.org/linux/man-pages/man1/chmod.1.html), [`path_resolution(7)`](https://man7.org/linux/man-pages/man7/path_resolution.7.html), [`passwd(5)`](https://man7.org/linux/man-pages/man5/passwd.5.html), [`shadow(5)`](https://man7.org/linux/man-pages/man5/shadow.5.html), [`sudoers(5)`](https://man7.org/linux/man-pages/man5/sudoers.5.html), [`capabilities(7)`](https://man7.org/linux/man-pages/man7/capabilities.7.html)
- [Understanding Linux File Permissions](https://www.linux.com/learn/understanding-linux-file-permissions), linux.com. հին դասի հղումը, այժմ միայն ամփոփում է

## Hands-On Task · Գործնական առաջադրանք

Կրկնիր kernel-ի որոշումը ինքդ. [`template.cpp`](template.cpp)։

```bash
./day07 --selftest                # թեստեր կեղծ ֆայլերի և օգտատերերի վրա. PASS / FAIL
./day07 /etc/shadow /usr/bin/passwd /tmp ~/.bashrc
# -rw-r----- 0640 root   shadow  /etc/shadow      you: ---   access(): ---
# -rwsr-xr-x 4755 root   root    /usr/bin/passwd  you: r-x   access(): r-x
# drwxrwxrwt 1777 root   root    /tmp             you: rwx   access(): rwx
```

Լրացրու TODO-ները. `mode_string` (`ls -l`-ի ձևով՝ `s`/`S`/`t`/`T`-ով), `octal_string`, `can_access` (§3-ի ալգորիթմը), `can_delete` (թղթապանակի `w`+`x` և sticky), `my_groups` (`getgroups`)։ Քո արդյունքը համեմատվում է kernel-ի `access()`-ի հետ. դրանք պետք է համընկնեն։ Գործարկիր նաև `sudo ./day07 /etc/shadow`-ով և բացատրիր տարբերությունը։

## Self-Learning · Ինքնուրույն աշխատանք
1. Ստեղծիր երկու օգտատեր և ընդհանուր group։ Կազմակերպիր `/srv/project` թղթապանակ, որտեղ երկուսն էլ կարող են ստեղծել և փոխել ֆայլեր, բայց մնացածները չեն կարող նույնիսկ տեսնել ցանկը (setgid + `2770`)։
2. Ստեղծիր ֆայլ `----r--r--` իրավունքներով և փորձիր կարդալ այն որպես owner։ Բացատրիր, թե ինչու չի ստացվում։
3. Գրիր C++ ծրագիր, որը տպում է real և effective UID-ը։ Տուր նրան setuid բիթ (`sudo chown root prog && sudo chmod u+s prog`) և գործարկիր որպես սովորական օգտատեր։ Ինչ փոխվեց։ (Հետո ջնջիր ծրագիրը։)
4. Գտիր քո համակարգի բոլոր setuid ծրագրերը։ Ընտրիր երեքը և պարզիր `man`-ով, թե ինչու են դրանք setuid։
5. `umask 077`-ով ստեղծիր ֆայլ և թղթապանակ։ Ինչ իրավունքներ ստացան։ Ինչու է թղթապանակը `700`, ոչ թե `600`։
6. `sudo -l`. ինչ իրավունքներ ունես `sudo`-ում։ Կարդա `/etc/sudoers`-ը և `/etc/sudoers.d/`-ը։
7. Գրիր ծրագիր, որը կանչում է `system()` օգտատիրոջ մուտքով, և ցույց տուր command injection-ը։ Ուղղիր այն `fork` + `execvp`-ով (Օր 10)։
8. `/etc/shadow`-ի տողում գտիր գաղտնաբառի hash-ի ալգորիթմը (`$y$` = yescrypt, `$6$` = SHA-512)։ Ինչու են պահում hash, ոչ թե գաղտնաբառը։

## Self-Check · Ինքնաստուգում
Պատասխաններ չկան։ Մտածիր ինքնուրույն։

1. Կարող են երկու տարբեր անուններ ունենալ նույն UID-ը։ Ինչ կլինի նրանց ֆայլերի հետ։
2. Ֆայլը `-rw-r--r-- root root` է։ Կարող է `student`-ը ջնջել այն, եթե այն գտնվում է `student`-ի home թղթապանակում։ Ինչու։
3. Ինչո՞ւ `/tmp`-ը `rwxrwxrwt` է, ոչ թե `rwxrwxrwx`։ Ինչ կարող էր պատահել առանց `t`-ի։
4. `passwd` ծրագիրը ինչպես է փոխում `/etc/shadow`-ը, եթե օգտատերը իրավունք չունի նույնիսկ կարդալու այն։
5. Ինչու է setuid ծրագրում buffer overrun-ը շատ ավելի վտանգավոր, քան սովորական ծրագրում։
6. `ls -l`-ում `rwSr--r--`. ինչ է նշանակում մեծատառ `S`-ը։
7. Ինչու չի կարող սովորական օգտատերը `chown` անել իր ֆայլը ուրիշին։
8. Group-ին ավելացրիր `usermod -aG`-ով, բայց `groups`-ը նոր group-ը չի ցույց տալիս։ Ինչու։
9. Ինչ տարբերություն կա `sudo`-ի և `su`-ի միջև. ում գաղտնաբառն է պետք և ինչ է գրանցվում log-ում։
10. Ինչու է `access()`, ապա `open()` հաջորդականությունը վտանգավոր setuid ծրագրում։
11. Ծրագրից հանեցիր `x` իրավունքը, որ ուսանողները չգործարկեն այն։ Կաշխատի՞։

## Code Template · Կոդի ձևանմուշ
Տե՛ս [`template.cpp`](template.cpp)-ը։

```bash
g++ -std=c++17 -Wall -Wextra -O2 template.cpp -o day07
./day07 --selftest && ./day07 /etc/shadow /usr/bin/passwd /tmp
```
