// Day 4: pipe lab · pipe-երի լաբորատորիա
//
//   g++ -std=c++17 -Wall -Wextra -O2 template.cpp -o day04
//   ./day04
//
// Ամեն TODO փորձ է, որ չափում կամ ցույց է տալիս pipe-ի մեկ հատկություն.
// buffer-ի չափը, EOF-ը, SIGPIPE-ը, երկու ուղղությամբ կապը ծրագրի հետ։
// Սկզբում ամեն ինչ FAIL է. լրացրու TODO-ները հերթով։

#include <fcntl.h>
#include <limits.h>   // PIPE_BUF
#include <sys/wait.h>
#include <unistd.h>

#include <cerrno>
#include <csignal>
#include <cstdio>
#include <cstring>
#include <fstream>
#include <string>
#include <vector>

#include "../common/sys_check.h"

// ---------------------------------------------------------------- helpers (ready to use)

// Գրել բոլոր բայթերը. write()-ը կարող է գրել ավելի քիչ, քան խնդրել ենք
[[maybe_unused]] static bool write_all(int fd, std::string const &data)
{
    auto done = std::size_t{0};
    while (done < data.size()) {
        auto const n = write(fd, data.data() + done, data.size() - done);
        if (n < 0) return false;
        done += static_cast<std::size_t>(n);
    }
    return true;
}

// Կարդալ մինչև EOF (read-ը վերադարձնում է 0)
[[maybe_unused]] static std::string read_all(int fd)
{
    auto out = std::string{};
    char buf[4096];
    for (ssize_t n; 0 < (n = read(fd, buf, sizeof buf));) out.append(buf, static_cast<std::size_t>(n));
    return out;
}

// Միացնել O_NONBLOCK. write-ը չի սպասի, այլ կվերադարձնի -1 և errno = EAGAIN
[[maybe_unused]] static void set_nonblocking(int fd)
{
    auto const flags = SYS_CHECK(fcntl(fd, F_GETFL));
    SYS_CHECK(fcntl(fd, F_SETFL, flags | O_NONBLOCK));
}

// ---------------------------------------------------------------- worked example

// Pipe մեկ process-ի ներսում. գրել մի ծայրից, կարդալ մյուսից։
// fds[0]՝ կարդալու ծայր, fds[1]՝ գրելու ծայր (հիշիր. 0 = stdin, 1 = stdout)
static std::string echo_through_pipe(std::string const &text)
{
    int fds[2];
    SYS_CHECK(pipe(fds));
    std::printf("    pipe() gave fds[0] = %d (read end), fds[1] = %d (write end)\n", fds[0], fds[1]);

    SYS_CHECK(write(fds[1], text.data(), text.size()));   // տվյալները հիմա kernel-ի buffer-ում են
    SYS_CHECK(close(fds[1]));                             // գրողներ այլևս չկան → read-ը կստանա EOF

    auto const out = read_all(fds[0]);
    SYS_CHECK(close(fds[0]));
    return out;
}

// ---------------------------------------------------------------- TODO

// TODO 1. Չափել pipe-ի buffer-ը.
//   pipe(fds), set_nonblocking(fds[1])
//   ցիկլով write(fds[1], buf, 1024), մինչև write-ը վերադարձնի -1 և errno == EAGAIN
//   (վերջին write-ը կարող է գրել 1024-ից պակաս. գումարիր վերադարձրած արժեքները)
//   փակել երկու ծայրը, վերադարձնել գրված բայթերի քանակը
static long measure_capacity()
{
    return -1;
}

// TODO 2. Փոխել buffer-ի չափը և չափել կրկին.
//   pipe(fds), fcntl(fds[1], F_SETPIPE_SZ, request)
//   վերադարձնել fcntl(fds[1], F_GETPIPE_SZ). kernel-ը կլորացնում է էջերի քանակով (2-ի աստիճան)
//   Որքան է առավելագույնը առանց root-ի. cat /proc/sys/fs/pipe-max-size
static long resize_capacity(long request)
{
    (void)request;
    return -1;
}

// TODO 3. Child-ը գրում է, parent-ը կարդում է.
//   pipe(fds), fork()
//   child. close(fds[0]), write_all(fds[1], n բայթ 'x'), close(fds[1]), _exit(0)
//   parent.  close(fds[1])   ← ԿԱՐԵՎՈՐ. այլապես read-ը երբեք չի ստանա EOF
//          total = read_all(fds[0]).size(), close(fds[0]), waitpid
//   վերադարձնել total
// n-ը կարող է լինել buffer-ից մեծ. child-ը կսպասի write-ում, մինչև parent-ը կարդա։
static long child_to_parent(long n)
{
    (void)n;
    return -1;
}

// TODO 4. Գրել pipe-ում, որը ոչ ոք չի կարդում.
//   signal(SIGPIPE, SIG_IGN)   — այլապես process-ը կմեռնի SIGPIPE-ով
//   pipe(fds), close(fds[0]), write(fds[1], "x", 1)
//   պահել errno-ն, close(fds[1]), signal(SIGPIPE, SIG_DFL)
//   վերադարձնել errno-ն (պետք է լինի EPIPE)
static int write_without_reader()
{
    return -1;
}

// TODO 5. Անցկացնել input-ը արտաքին ծրագրով և ստանալ նրա ելքը (ինչպես popen, բայց երկու ուղղությամբ).
//   pipe(to_child), pipe(from_child), fork()
//   child. dup2(to_child[0], 0), dup2(from_child[1], 1), փակել բոլոր չորս fd-երը, execvp, _exit(127)
//   parent.  փակել to_child[0] և from_child[1]
//          write_all(to_child[1], input), close(to_child[1])   ← child-ը կստանա EOF
//          output = read_all(from_child[0]), close(from_child[0]), waitpid
//   վերադարձնել output
// Ուշադրություն. input-ը պետք է տեղավորվի buffer-ում (տես Self-Learning 4. deadlock)։
static std::string run_filter(std::vector<std::string> argv, std::string const &input)
{
    (void)argv;
    (void)input;
    return "";
}

// ---------------------------------------------------------------- experiments (ready)

static int g_fail = 0;

static void check(bool ok, char const *what)
{
    std::printf("%s  %s\n", ok ? "PASS" : "FAIL", what);
    std::fflush(stdout);   // fork-ից առաջ. այլապես buffer-ը կտպվի երկու անգամ (Օր 10)
    if (!ok) ++g_fail;
}

int main()
{
    auto max_size = 0L;
    std::ifstream("/proc/sys/fs/pipe-max-size") >> max_size;
    std::printf("PIPE_BUF (atomic write limit) = %d bytes\n", PIPE_BUF);
    std::printf("pipe-max-size (without root)  = %ld bytes\n\n", max_size);

    std::printf("Worked example:\n");
    check("hello\n" == echo_through_pipe("hello\n"), "echo_through_pipe returns what was written");

    std::printf("\n1. Buffer capacity\n");
    auto const cap = measure_capacity();
    std::printf("    measured capacity: %ld bytes (%ld pages of %ld)\n", cap, cap / sysconf(_SC_PAGESIZE),
                sysconf(_SC_PAGESIZE));
    check(16 * sysconf(_SC_PAGESIZE) == cap, "default buffer is 16 pages (64 KiB with 4 KiB pages)");

    std::printf("\n2. Resizing\n");
    auto const big = resize_capacity(1 << 20);
    std::printf("    asked 1048576, got %ld\n", big);
    check(1048576 == big || (big == max_size && max_size < 1048576), "F_SETPIPE_SZ to 1 MiB");
    auto const odd = resize_capacity(100000);
    std::printf("    asked 100000, got %ld\n", odd);
    check(131072 == odd, "size is rounded up to a power of two pages");

    std::printf("\n3. Child writes, parent reads\n");
    check(10 == child_to_parent(10), "10 bytes arrive");
    check(1000000 == child_to_parent(1000000), "1 MB arrives through a 64 KiB pipe (writer blocks and waits)");

    std::printf("\n4. No reader\n");
    auto const err = write_without_reader();
    std::printf("    errno = %d (%s)\n", err, -1 == err ? "TODO" : std::strerror(err));
    check(EPIPE == err, "write to a pipe without readers fails with EPIPE");

    std::printf("\n5. Two-way pipe to a program\n");
    check("apple\nbanana\ncherry\n" == run_filter({"sort"}, "cherry\napple\nbanana\n"), "sort");
    check("3\n" == run_filter({"wc", "-l"}, "a\nb\nc\n"), "wc -l");
    check("HELLO" == run_filter({"tr", "a-z", "A-Z"}, "hello"), "tr a-z A-Z");

    std::printf("\n%s (%d failed)\n", 0 == g_fail ? "ALL PASSED" : "SOME FAILED", g_fail);
    return 0 == g_fail ? 0 : 1;
}
