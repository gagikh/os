# Day 11: Networking and Sockets · Օր 11. Network և socket-ներ

## Objectives · Նպատակներ
- Բացատրել IP հասցեն, network ինտերֆեյսը, port-ը, TCP-ն և UDP-ն
- Ստուգել network-ի վիճակը. `ip`, `ping`, `ss`, `nc`, `nmap`, `curl`, `dig`
- Գրել TCP server և client socket API-ով (`socket`, `bind`, `listen`, `accept`, `connect`, `send`, `recv`)
- Բացատրել byte order-ը (`htons`) և ինչու է այն պետք
- Սպասարկել մի քանի client մեկ thread-ով `poll()`-ի միջոցով և համեմատել `fork`/thread-ի տարբերակների հետ

## Key Concepts · Հիմնական գաղափարներ
- IPv4 / IPv6 հասցե, subnet, gateway, DNS
- Network ինտերֆեյս (`lo`, `eth0`, `wlan0`)
- Port, TCP (stream, կապով, հուսալի) և UDP (datagram, առանց կապի)
- Socket-ը file descriptor է (Օր 3, 9)
- Server. `socket → bind → listen → accept → recv/send → close`
- Client. `socket → connect → send/recv → close`
- Network byte order. `htons`, `htonl`, `ntohs`
- Մի քանի client. `fork`, thread, `poll`/`epoll`

## Definitions · Սահմանումներ

*Այսօր ներմուծվող տերմինները։ Նույն տեքստը կա [GLOSSARY.md](../GLOSSARY.md)-ում։*

**IP address** — Համակարգչի (ավելի ճիշտ՝ ինտերֆեյսի) հասցեն network-ում։ IPv4-ը 32 բիթ է (`192.168.1.10`), IPv6-ը՝ 128 բիթ (`fe80::1`)։ `127.0.0.1`-ը (`localhost`) միշտ նույն համակարգիչն է։

**Network interface** — Network-ին միացման կետ. ֆիզիկական (`eth0`, `wlan0`, `enp3s0`) կամ վիրտուալ (`lo`՝ loopback, `docker0`, `tun0`)։ Ամեն ինտերֆեյս ունի իր IP հասցեն(երը)։

**Subnet / CIDR** — Հասցեների խումբ, որոնք նույն տեղական network-ում են։ `192.168.1.0/24`. առաջին 24 բիթը network-ը է, վերջին 8-ը՝ սարքը (256 հասցե)։

**Gateway (router)** — Սարք, որին ուղարկվում են այլ network-ների համար նախատեսված packet-ները։ Ցույց է տալիս `ip route`-ը (`default via ...`)։

**DNS** — Service, որը անունը (`github.com`) վերածում է IP հասցեի։ Linux-ում՝ `/etc/hosts` և `/etc/resolv.conf`։

**Port** — 16 բիթանոց թիվ (0–65535), որը համակարգչի ներսում ընտրում է ծրագիրը։ IP-ն «շենքն» է, port-ը՝ «բնակարանը»։ 0–1023-ը պահանջում են root (Օր 7). 22՝ ssh, 80՝ http, 443՝ https։

**TCP** — Կապով, հուսալի **բայթերի stream**. տվյալները հասնում են առանց կորուստի և նույն հերթականությամբ։ Հաղորդագրությունների սահմաններ չկան. երկու `send` կարող են հասնել մեկ `recv`-ով և հակառակը։

**UDP** — Առանց կապի **datagram**-ներ. ամեն մեկը առանձին հաղորդագրություն է, բայց կարող է կորչել, կրկնվել կամ գալ այլ հերթականությամբ։ Արագ է. օգտագործվում է DNS-ում, video stream-ում, խաղերում, sensor-ներից տվյալների համար։

**Socket** — Network կապի ծայրակետ, որը ծրագիրը տեսնում է որպես **file descriptor**։ Նրա վրա աշխատում են `read`/`write`-ը, `close`-ը, `poll`-ը, ինչպես ֆայլերի և pipe-երի վրա։

**Listening socket** — Server-ի socket, որը `listen`-ից հետո ընդունում է նոր կապեր։ Ամեն `accept` վերադարձնում է **նոր** socket տվյալ client-ի համար, իսկ լսողը շարունակում է լսել։

**Byte order (endianness)** — Բազմաբայթ թվի բայթերի հերթականությունը memory-ի մեջ։ x86-ը և ARM-ը little-endian են, իսկ network protocol-ները՝ big-endian («network byte order»)։ `htons` (host to network short), `htonl`, `ntohs`, `ntohl` ֆունկցիաները փոխակերպում են։

**`poll`** — System call, որը սպասում է միանգամից **մի քանի** file descriptor-ի, մինչև նրանցից որևէ մեկը պատրաստ լինի (կարդալու տվյալ կա, նոր կապ է եկել, կապը փակվել է)։ Թույլ է տալիս մեկ thread-ով սպասարկել շատ client-ներ։ Linux-ում ավելի մասշտաբելի տարբերակը `epoll`-ն է։

---

## 1. Network-ը Linux-ում

### Visual
![Packet-ի շերտերը. ծրագիրը գրում է տվյալներ socket-ում, TCP-ն ավելացնում է port-երը (source 43122, destination 5555), IP-ն՝ հասցեները (192.168.1.10 → 192.168.1.20), Ethernet-ը՝ MAC հասցեները. ստացող կողմում kernel-ը հանում է header-ները և ըստ destination port-ի տալիս տվյալները ճիշտ socket-ին](network_layers.svg)

Երբ ծրագիրը ուղարկում է տվյալներ, kernel-ը դրանք «փաթաթում» է մի քանի header-ով.

| Շերտ | Ինչ է ավելացնում | Ինչ է որոշում |
|---|---|---|
| **Application** | Տվյալներ (HTTP, մեր չաթի տեքստը) | Ինչ է ասվում |
| **Transport** (TCP / UDP) | Source և destination **port** | Որ **ծրագրին** է |
| **Network** (IP) | Source և destination **IP** | Որ **համակարգչին** է |
| **Link** (Ethernet, Wi-Fi) | **MAC** հասցեներ | Որ սարքին է **տեղական** network-ում |

Ծրագրի համար այս ամենը թաքնված է. նա տեսնում է միայն socket-ը։

Առաջին քայլերը նոր board-ի վրա (Jetson, Raspberry Pi).

```bash
ip -br addr
# lo        UNKNOWN  127.0.0.1/8 ::1/128
# eth0      UP       192.168.1.20/24 fe80::...
ip route
# default via 192.168.1.1 dev eth0      ← gateway
ping -c 3 192.168.1.1                   # router-ը հասանելի է
ping -c 3 github.com                    # DNS-ը և ինտերնետը աշխատում են
ss -tlnp                                # ինչ service-ներ են լսում (sshd-ը 22-ի վրա)
```

Եթե `ping 8.8.8.8`-ը աշխատում է, իսկ `ping github.com`-ը՝ ոչ, խնդիրը DNS-ն է (`/etc/resolv.conf`)։

Անունը IP-ի վերածելու համար համակարգը նախ նայում է `/etc/hosts` ֆայլը, ապա հարցնում է `/etc/resolv.conf`-ում նշված DNS server-ին։ Ձեռքով ստուգել.

```bash
cat /etc/hosts                 # տեղական անուններ. 127.0.0.1 localhost
dig +short github.com          # DNS-ի պատասխանը (կամ host github.com)
```

Իսկ եթե ուզում ես տեսնել, թե իրականում ինչ է անցնում network-ով, `tcpdump`-ը ցույց է տալիս packet-ները (պահանջում է root).

```bash
sudo tcpdump -i lo port 5555 -A     # lo ինտերֆեյսի packet-ները port 5555-ի վրա, տեքստով
curl -v http://example.com          # HTTP հարցում. -v-ն ցույց է տալիս, թե ինչ է ուղարկվում և ստացվում
```

## 2. `nc`. network-ի «շվեյցարական դանակը»

`nc` (netcat)-ը միացնում է stdin/stdout-ը TCP կամ UDP socket-ի հետ (Օր 2, 4)։

```bash
nc -l 5555                    # terminal 1. server
nc localhost 5555             # terminal 2. client. գրիր տող, այն կհայտնվի terminal 1-ում

nc -l 5555 > received.tar     # ֆայլի փոխանցում. ընդունող
tar -cf - project | nc 192.168.1.20 5555   # ուղարկող (Օր 5)

printf 'GET / HTTP/1.0\r\nHost: example.com\r\n\r\n' | nc example.com 80   # HTTP «ձեռքով»
```

`nmap`-ը ստուգում է, որ port-երն են բաց։ Օգտագործիր այն **միայն** քո սարքերի վրա. այլոց network-ների սկանավորումը առանց թույլտվության կարող է խախտել օրենքը և կանոնները։

```bash
nmap -p 1-1024 localhost
nmap -sn 192.168.1.0/24       # որ սարքերն են միացված տեղական network-ում (քո network-ում)
```

## 3. Socket API

### Visual
![Server-ը կանչում է socket, bind (port 5555), listen, ապա accept-ը սպասում է. client-ը կանչում է socket և connect. TCP handshake-ից հետո accept-ը վերադարձնում է նոր fd. ապա send/recv երկու ուղղությամբ, close. recv-ը վերադարձնում է 0, երբ մյուս կողմը փակել է կապը](socket_flow.svg)

**Server**

```cpp
auto const lfd = socket(AF_INET, SOCK_STREAM, 0);          // TCP socket

auto one = 1;
setsockopt(lfd, SOL_SOCKET, SO_REUSEADDR, &one, sizeof one);   // թույլ տալ արագ վերագործարկել

auto addr = sockaddr_in{};
addr.sin_family      = AF_INET;
addr.sin_addr.s_addr = htonl(INADDR_ANY);                  // բոլոր ինտերֆեյսները
addr.sin_port        = htons(5555);                        // byte order!
bind(lfd, reinterpret_cast<sockaddr *>(&addr), sizeof addr);
listen(lfd, 16);

auto const cfd = accept(lfd, nullptr, nullptr);            // սպասում է client-ին
char buf[1024];
auto const n = recv(cfd, buf, sizeof buf, 0);              // 0 → client-ը փակեց կապը
send(cfd, buf, n, 0);                                      // echo
close(cfd);
```

**client**

```cpp
auto const fd = socket(AF_INET, SOCK_STREAM, 0);
auto addr = sockaddr_in{};
addr.sin_family = AF_INET;
addr.sin_port   = htons(5555);
inet_pton(AF_INET, "127.0.0.1", &addr.sin_addr);
connect(fd, reinterpret_cast<sockaddr *>(&addr), sizeof addr);
send(fd, "hi\n", 3, 0);
```

Հիշիր երեք թակարդ.

1. **Byte order**. `addr.sin_port = 5555` առանց `htons`-ի little-endian սարքի վրա կլսի port 46613 (`0x15B3` → `0xB315`)։ Ստուգիր `ss -tlnp`-ով։
2. **TCP-ն stream է, ոչ թե հաղորդագրություններ**. `send("hello")` + `send("world")`-ը կարող է հասնել որպես մեկ `recv` `"helloworld"` կամ երեք մասով։ Հաղորդագրությունների սահմանները պետք է որոշի քո protocol-ը (օրինակ՝ `\n`-ով կամ երկարությամբ սկզբում)։
3. **`send`-ը կարող է ուղարկել ոչ բոլորը**. ստուգիր վերադարձրած արժեքը և ցիկլով ուղարկիր մնացածը (`send_all` template-ում)։ Եթե մյուս կողմը փակել է կապը, `send`-ը կառաջացնի `SIGPIPE` (Օր 8). `MSG_NOSIGNAL`-ով կստանաս `EPIPE` սխալ։

**UDP**-ի համար `listen`/`accept`/`connect` պետք չեն. `socket(AF_INET, SOCK_DGRAM, 0)`, `bind`, ապա `recvfrom`/`sendto`, որոնք ամեն հաղորդագրության հետ ստանում/տալիս են հասցեն։

Անունով միանալու համար (ոչ թե IP-ով) օգտագործիր `getaddrinfo("example.com", "80", ...)`, որը աշխատում է և IPv4-ի, և IPv6-ի հետ։

## 4. Մի քանի client

Վերը նշված server-ը սպասարկում է միայն մեկ client. `accept`-ը և `recv`-ը **արգելափակող** են (blocking)։ Երեք տարբերակ.

| Ինչպես | Գաղափար | Առավելություն | Թերություն |
|---|---|---|---|
| `fork` ամեն client-ի համար | Օր 10. child-ը սպասարկում է, parent-ը վերադառնում է `accept`-ին | Պարզ, մեկուսացված | Դժվար է տվյալներ կիսել client-ների միջև (չաթ) |
| Thread ամեն client-ի համար | Օր 10. `std::thread(serve, cfd).detach()` | Ընդհանուր memory | Mutex-ներ, race condition-ներ, հազարավոր thread-եր |
| **`poll`** (կամ `epoll`) | Մեկ thread սպասում է **բոլոր** fd-ներին միաժամանակ | Մեկ thread, առանց lock-ների | Կոդը «իրադարձությունների» տեսքով է |

`poll`-ով ցիկլը.

```cpp
std::vector<pollfd> fds = {{lfd, POLLIN, 0}};    // սկզբում միայն լսող socket-ը
while (true) {
    poll(fds.data(), fds.size(), -1);            // քնել, մինչև ինչ-որ բան պատրաստ լինի
    if (fds[0].revents & POLLIN) { /* accept → push_back({cfd, POLLIN, 0}) */ }
    for (auto i = 1u; i < fds.size(); ++i)
        if (fds[i].revents & POLLIN) { /* recv. 0 → close և erase, այլապես broadcast */ }
}
```

`nginx`-ը, `redis`-ը, Node.js-ը աշխատում են հենց այս սկզբունքով (`epoll`-ով)։ Embedded համակարգերում նույն ցիկլով կարելի է միաժամանակ սպասել socket-ին, serial port-ին (`/dev/ttyUSB0`, Օր 9) և timer-ին, քանի որ բոլորը file descriptor-ներ են։

---

## Commands · Հրամաններ

*Ամփոփում. այս օրվա հրամանները, որոնք բացատրված են վերևում։ Անգիր անել պետք չէ. վերադարձիր այստեղ, երբ մոռանաս գրելաձևը։ Բոլոր օրերի հրամանների ցանկը [COMMANDS.md](../COMMANDS.md)-ում է։*

```bash
ip -br addr                  # ինտերֆեյսներ և նրանց IP-ները (կարճ)
ip link                      # ինտերֆեյսներ, MAC հասցեներ, UP/DOWN
ip route                     # route-ների աղյուսակ, default gateway
ping -c 3 8.8.8.8            # արդյոք հասանելի է (ICMP)
cat /etc/hosts /etc/resolv.conf
dig +short github.com        # DNS. անուն → IP (կամ host, nslookup)
ss -tlnp                     # ով է լսում TCP port-երի վրա (t=TCP, l=listen, n=թվեր, p=process)
ss -tnp                      # հաստատված TCP կապերը
nc -l 5555                   # «server». լսել port 5555-ը
nc localhost 5555            # «client». միանալ
nc -u -l 5556 ; nc -u localhost 5556   # նույնը UDP-ով
nmap -p 1-1024 localhost     # որ port-երն են բաց (միայն քո սարքերի վրա)
curl -v http://example.com   # HTTP հարցում, ցույց է տալիս header-ները
sudo tcpdump -i lo port 5555 -A   # տեսնել packet-ները
```

## Functions · Ֆունկցիաներ

*Ամփոփում. այս օրվա C ֆունկցիաները։ Ամբողջական ցանկը [API.md](../API.md)-ում է։*

```c
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <poll.h>

int socket(int domain, int type, int protocol);   // AF_INET, SOCK_STREAM (TCP) կամ SOCK_DGRAM (UDP), 0
int setsockopt(int fd, int level, int opt, const void *val, socklen_t len);   // SOL_SOCKET, SO_REUSEADDR
int bind(int fd, const struct sockaddr *addr, socklen_t len);    // կապել socket-ը հասցե:port-ին
int listen(int fd, int backlog);                  // դառնալ լսող. backlog՝ սպասող կապերի հերթ
int accept(int fd, struct sockaddr *addr, socklen_t *len);       // նոր կապ → նոր fd
int connect(int fd, const struct sockaddr *addr, socklen_t len); // client. միանալ server-ին
ssize_t send(int fd, const void *buf, size_t n, int flags);      // կարող է ուղարկել n-ից պակաս
ssize_t recv(int fd, void *buf, size_t n, int flags);            // 0 → մյուս կողմը փակեց կապը
ssize_t sendto(...); ssize_t recvfrom(...);       // UDP. հասցեն ամեն հաղորդագրության հետ
int getpeername(int fd, struct sockaddr *addr, socklen_t *len);  // ով է մյուս կողմում

uint16_t htons(uint16_t x);  uint32_t htonl(uint32_t x);         // host → network byte order
uint16_t ntohs(uint16_t x);  uint32_t ntohl(uint32_t x);         // network → host
int inet_pton(int af, const char *src, void *dst);               // "127.0.0.1" → բինար
const char *inet_ntop(int af, const void *src, char *dst, socklen_t n);   // բինար → տեքստ
int getaddrinfo(const char *host, const char *port, const struct addrinfo *hints, struct addrinfo **res);

struct sockaddr_in { sa_family_t sin_family; in_port_t sin_port; struct in_addr sin_addr; };
int poll(struct pollfd *fds, nfds_t n, int timeout_ms);          // -1՝ սպասել անվերջ
struct pollfd { int fd; short events; short revents; };          // POLLIN, POLLHUP, POLLERR
```

## Resources · Աղբյուրներ
- *Advanced Linux Programming*, Chapter 5.5 «Sockets» (local և Internet-domain socket-ներ, server-ի օրինակ). [`docs/linux_book.pdf`](../docs/linux_book.pdf)
- [BinaryTides: Server and client example with C sockets on Linux](https://www.binarytides.com/server-client-example-c-sockets-linux/). echo server, client, thread ամեն կապի համար (տես նաև մեկնաբանությունները. `close`-ի և `strlen`-ի սխալները)
- [University of Crete CS556: Socket programming tutorial](http://www.csd.uoc.gr/~hy556/material/tutorials/cs556-3rd-tutorial.pdf). TCP/IP շերտեր, TCP և UDP, byte order, framing, `select`, `fork`/thread server-ներ, broadcast և multicast
- [`socket(7)`](https://man7.org/linux/man-pages/man7/socket.7.html), [`tcp(7)`](https://man7.org/linux/man-pages/man7/tcp.7.html), [`udp(7)`](https://man7.org/linux/man-pages/man7/udp.7.html), [`ip(7)`](https://man7.org/linux/man-pages/man7/ip.7.html), [`poll(2)`](https://man7.org/linux/man-pages/man2/poll.2.html), [`epoll(7)`](https://man7.org/linux/man-pages/man7/epoll.7.html), [`getaddrinfo(3)`](https://man7.org/linux/man-pages/man3/getaddrinfo.3.html)
- [Beej's Guide to Network Programming](https://beej.us/guide/bgnet/)
- Հասանելի չէ. Basel համալսարանի «M02 Socket Programming in C» (հղումը այժմ վերահղում է այլ էջ)

## Hands-On Task · Գործնական առաջադրանք

Գրիր TCP չաթ server `poll()`-ով. [`template.cpp`](template.cpp)։

```bash
./day11 5555
chat server on port 5555. Try: nc localhost 5555
```

```bash
# terminal 2                              # terminal 3
nc localhost 5555                         nc localhost 5555
* welcome, 127.0.0.1:43122                * welcome, 127.0.0.1:43130
* 127.0.0.1:43130 joined                  [127.0.0.1:43122] բարև
բարև
```

Լրացրու TODO-ները. `make_listener`, `broadcast`, `accept_client`, `handle_client`, `serve`։ Ստուգիր `./day11 --selftest`-ով, ապա `nc`-ով։ Board-ի վրա գործարկիր server-ը, և միացիր laptop-ից `nc <board-ip> 5555`-ով (ստուգիր IP-ն `ip -br addr`-ով)։

## Self-Learning · Ինքնուրույն աշխատանք
1. Գործարկիր server-ը, և `ss -tnp`-ով տես բոլոր կապերը։ Ինչու ունի ամեն client տարբեր port։
2. Հեռացրու `htons`-ը `make_listener`-ից։ Որ port-ի վրա է server-ը հիմա լսում։ Հաշվիր ձեռքով։
3. Հեռացրու `SO_REUSEADDR`-ը, անջատիր server-ը Ctrl-C-ով, երբ client կա, և անմիջապես գործարկիր կրկին։ Ինչ սխալ ես ստանում։ Կարդա `TIME_WAIT`-ի մասին (`ss -tan`)։
4. Ավելացրու `/nick <name>` հրամանը. client-ի անունը պահիր `std::map<int, std::string>`-ում։ Ինչ խնդիր է առաջանում, եթե `/nick`-ը գալիս է երկու `recv`-ով։ Լուծիր՝ պահելով անավարտ տողի buffer ամեն client-ի համար։
5. Գրիր նույն չաթը thread-ով ամեն client-ի համար (Օր 10)։ Որտեղ է պետք mutex։ Համեմատիր կոդի բարդությունը։
6. Գրիր UDP «sensor» ծրագիր. board-ը ամեն վայրկյան ուղարկում է CPU-ի ջերմաստիճանը (Օր 9, embedded Օր 1) UDP-ով, laptop-ը ստանում և տպում է (`recvfrom`)։ Ինչ է պատահում, եթե ստացողը աշխատում չէ։
7. Փոխարինիր `poll`-ը `epoll`-ով (`epoll_create1`, `epoll_ctl`, `epoll_wait`)։ Ինչու է `epoll`-ը ավելի լավ 10000 client-ի դեպքում։
8. Գրիր փոքր HTTP server, որը ցանկացած հարցման պատասխանում է `HTTP/1.0 200 OK` և քո `day09`-ի hardware inventory-ով։ Բացիր այն browser-ով `http://<board-ip>:8080`։

## Self-Check · Ինքնաստուգում
Պատասխաններ չկան։ Մտածիր ինքնուրույն։

1. Ինչ տարբերություն կա IP հասցեի և port-ի միջև։ Ինչու է երկուսն էլ պետք։
2. `ping 8.8.8.8`-ը աշխատում է, `ping github.com`-ը՝ ոչ։ Որտեղ է խնդիրը։
3. Երբ ընտրել TCP, երբ UDP։ Բեր մեկական օրինակ embedded համակարգերից։
4. Ինչու `send`-ի երկու կանչը կարող է հասնել որպես մեկ `recv`։ Ինչպես բաժանել հաղորդագրությունները։
5. Ինչու է պետք `htons`-ը, եթե x86-ը և ARM-ը երկուսն էլ little-endian են։
6. Ինչ է վերադարձնում `accept`-ը, և ինչու այն նույն socket-ը չէ, ինչ `lfd`-ն։
7. `recv`-ը վերադարձրեց 0։ Ինչ է դա նշանակում։ Իսկ -1-ը։
8. Ինչու 1024-ից փոքր port-ի վրա լսելը պահանջում է root (Օր 7)։
9. Ինչ առավելություններ ունի `poll`-ը thread ամեն client-ի համար տարբերակի նկատմամբ, և ինչ թերություններ։
10. Ինչու կարող է `poll`-ը միաժամանակ սպասել socket-ին և serial port-ին։

## Code Template · Կոդի ձևանմուշ
Տե՛ս [`template.cpp`](template.cpp)-ը։

```bash
g++ -std=c++17 -Wall -Wextra -O2 template.cpp -o day11
./day11 --selftest
./day11 5555            # ապա այլ terminal-ներում. nc localhost 5555
```
