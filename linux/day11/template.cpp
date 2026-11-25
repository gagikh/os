// Day 11: TCP chat server with poll() · TCP չաթ սերվեր poll()-ով
//
//   g++ -std=c++17 -Wall -Wextra -O2 template.cpp -o day11
//   ./day11 5555            # սերվեր
//   nc localhost 5555       # հաճախորդ (բացիր 2-3 terminal)
//   ./day11 --selftest      # ստուգումներ
//
// Մեկ thread, մեկ poll(). սերվերը միաժամանակ սպասում է նոր կապերի և
// բոլոր հաճախորդների հաղորդագրություններին։ Ինչ գրում է մեկը, ստանում են մյուսները։

#include <arpa/inet.h>
#include <netinet/in.h>
#include <poll.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <unistd.h>

#include <csignal>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <vector>

#include "../common/sys_check.h"

struct Server {
    int                        listen_fd = -1;
    std::vector<struct pollfd> fds;          // fds[0]՝ listen socket-ը, մնացածը՝ հաճախորդներ
};

static volatile sig_atomic_t g_stop = 0;
static void on_stop(int) { g_stop = 1; }

// ---------------------------------------------------------------- helpers (ready to use)

// Ուղարկել բոլոր բայթերը. send()-ը կարող է ուղարկել մասնակի։
// MSG_NOSIGNAL. եթե հաճախորդը փակել է կապը, ստանալ EPIPE, ոչ թե SIGPIPE (Օր 8)։
[[maybe_unused]] static bool send_all(int fd, std::string const &data)
{
    auto sent = std::size_t{0};
    while (sent < data.size()) {
        auto const n = send(fd, data.data() + sent, data.size() - sent, MSG_NOSIGNAL);
        if (n <= 0) return false;
        sent += static_cast<std::size_t>(n);
    }
    return true;
}

// Կապի մյուս ծայրի հասցեն. "127.0.0.1:43122"
[[maybe_unused]] static std::string peer_name(int fd)
{
    auto addr = sockaddr_in{};
    auto len  = socklen_t{sizeof addr};
    if (-1 == getpeername(fd, reinterpret_cast<sockaddr *>(&addr), &len)) return "?";
    char ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &addr.sin_addr, ip, sizeof ip);
    return std::string(ip) + ":" + std::to_string(ntohs(addr.sin_port));
}

// Որ port-ին է կապված socket-ը (օգտակար, երբ bind-ին տվել ենք port 0, և OS-ը ընտրել է)
static int local_port(int fd)
{
    auto addr = sockaddr_in{};
    auto len  = socklen_t{sizeof addr};
    SYS_CHECK(getsockname(fd, reinterpret_cast<sockaddr *>(&addr), &len));
    return ntohs(addr.sin_port);
}

// ---------------------------------------------------------------- worked example: client side

// Միանալ host:port-ին. հաճախորդի կողմը ամբողջությամբ։
// socket → լրացնել sockaddr_in (byte order. htons) → connect
static int connect_to(char const *ip, int port)
{
    auto const fd = SYS_CHECK(socket(AF_INET, SOCK_STREAM, 0));

    auto addr       = sockaddr_in{};
    addr.sin_family = AF_INET;
    addr.sin_port   = htons(static_cast<uint16_t>(port));    // host → network byte order
    if (1 != inet_pton(AF_INET, ip, &addr.sin_addr)) {
        std::fprintf(stderr, "bad address %s\n", ip);
        std::exit(1);
    }
    SYS_CHECK(connect(fd, reinterpret_cast<sockaddr *>(&addr), sizeof addr));
    return fd;
}

// ---------------------------------------------------------------- TODO

// TODO 1. Ստեղծել լսող socket-ը.
//   socket(AF_INET, SOCK_STREAM, 0)
//   setsockopt(SO_REUSEADDR, 1)   — որ վերագործարկելիս bind-ը չձախողվի "Address already in use"-ով
//   sockaddr_in. INADDR_ANY (htonl), port (htons)
//   bind, listen(fd, 16)
// Վերադարձնել fd-ն։ port == 0 → OS-ը կընտրի ազատ port (տես local_port)։
static int make_listener(int port)
{
    (void)port;
    return -1;
}

// TODO 2. Ուղարկել text-ը բոլոր հաճախորդներին, բացի from_fd-ից (և բացի listen socket-ից)։
// Օգտագործիր send_all։
[[maybe_unused]] static void broadcast(Server &srv, int from_fd, std::string const &text)
{
    (void)srv;
    (void)from_fd;
    (void)text;
}

// TODO 3. Նոր կապ.
//   accept(listen_fd, nullptr, nullptr)
//   ավելացնել srv.fds-ում {fd, POLLIN, 0}
//   նոր հաճախորդին ուղարկել  "* welcome, <peer_name>\n"
//   մյուսներին՝              "* <peer_name> joined\n"
[[maybe_unused]] static void accept_client(Server &srv)
{
    (void)srv;
}

// TODO 4. srv.fds[i] հաճախորդը ունի տվյալներ.
//   n = recv(fd, buf, sizeof buf, 0)
//   n <= 0  → հաճախորդը գնաց. մյուսներին "* <peer_name> left\n", close(fd), հեռացնել srv.fds-ից
//             (peer_name-ը կանչիր close-ից առաջ)
//   n > 0   → մյուսներին "[<peer_name>] " + տվյալները
// Վերադարձնել true, եթե հաճախորդը հեռացվեց (որ կանչողը չավելացնի i-ն)։
[[maybe_unused]] static bool handle_client(Server &srv, std::size_t i)
{
    (void)srv;
    (void)i;
    return false;
}

// TODO 5. Գլխավոր ցիկլ.
//   while (!g_stop)
//     poll(srv.fds.data(), srv.fds.size(), -1)    (EINTR → շարունակել ցիկլը)
//     fds[0].revents & POLLIN  → accept_client
//     մյուսների համար revents & (POLLIN | POLLHUP | POLLERR) → handle_client
// Ուշադրություն. accept_client-ը և handle_client-ը փոխում են srv.fds-ը։
static void serve(Server &srv)
{
    (void)srv;
}

// ---------------------------------------------------------------- self-test

static int g_fail = 0;

static void check(bool ok, char const *what)
{
    std::printf("%s  %s\n", ok ? "PASS" : "FAIL", what);
    std::fflush(stdout);
    if (!ok) ++g_fail;
}

// Կարդալ այն, ինչ եկել է մինչև ms միլիվայրկյան (կամ մինչև կապի փակվելը)
static std::string receive(int fd, int ms = 1000)
{
    auto out = std::string{};
    auto p   = pollfd{fd, POLLIN, 0};
    while (0 < poll(&p, 1, ms)) {
        char buf[512];
        auto const n = recv(fd, buf, sizeof buf, 0);
        if (n <= 0) break;
        out.append(buf, static_cast<std::size_t>(n));
        ms = 100;                                   // եկածից հետո՝ կարճ սպասում մնացածին
    }
    return out;
}

static bool has(std::string const &s, char const *part) { return std::string::npos != s.find(part); }

static int selftest()
{
    auto const lfd = make_listener(0);
    check(0 <= lfd, "make_listener returns a socket");
    if (lfd < 0) { std::printf("\nSOME FAILED (start with TODO 1)\n"); return 1; }
    auto const port = local_port(lfd);
    check(0 < port, "listener bound to a port");

    std::fflush(stdout);
    auto const pid = SYS_CHECK(fork());
    if (0 == pid) {                                  // երեխա. սերվեր
        auto srv = Server{lfd, {{lfd, POLLIN, 0}}};
        serve(srv);
        _exit(0);
    }
    close(lfd);

    auto const a = connect_to("127.0.0.1", port);
    check(has(receive(a), "* welcome"), "A gets a welcome");
    auto const b = connect_to("127.0.0.1", port);
    check(has(receive(b), "* welcome"), "B gets a welcome");
    check(has(receive(a), "joined"), "A sees B joined");

    send_all(a, "hello from A\n");
    auto const got_b = receive(b);
    check(has(got_b, "hello from A"), "B receives A's message");
    check(has(got_b, "[127.0.0.1:"), "message is prefixed with the sender");
    check(receive(a, 300).empty(), "A does not get its own message back");

    auto const c = connect_to("127.0.0.1", port);
    receive(c); receive(a); receive(b);
    send_all(c, "hi all\n");
    check(has(receive(a), "hi all") && has(receive(b), "hi all"), "C's message reaches A and B");

    close(a);
    check(has(receive(b), "left"), "B sees A left");
    send_all(b, "still here\n");
    check(has(receive(c), "still here"), "server keeps working after a client leaves");

    close(b); close(c);
    kill(pid, SIGTERM);
    auto raw = 0;
    waitpid(pid, &raw, 0);
    check(WIFEXITED(raw) && 0 == WEXITSTATUS(raw), "server stops cleanly on SIGTERM");

    std::printf("\n%s (%d failed)\n", 0 == g_fail ? "ALL PASSED" : "SOME FAILED", g_fail);
    return 0 == g_fail ? 0 : 1;
}

// ---------------------------------------------------------------- main

int main(int argc, char **argv)
{
    struct sigaction sa {};
    sa.sa_handler = on_stop;                         // առանց SA_RESTART. poll-ը կվերադառնա EINTR-ով
    sigaction(SIGINT, &sa, nullptr);
    sigaction(SIGTERM, &sa, nullptr);

    if (1 < argc && 0 == std::strcmp(argv[1], "--selftest")) return selftest();

    auto const port = 1 < argc ? std::atoi(argv[1]) : 5555;
    auto const lfd  = make_listener(port);
    if (lfd < 0) { std::fprintf(stderr, "make_listener: TODO\n"); return 1; }

    std::printf("chat server on port %d. Try: nc localhost %d\n", local_port(lfd), local_port(lfd));
    auto srv = Server{lfd, {{lfd, POLLIN, 0}}};
    serve(srv);

    for (auto const &p : srv.fds) close(p.fd);
    std::printf("\nbye\n");
}
