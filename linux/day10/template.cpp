// Day 10: a mini shell · փոքր shell
//
//   g++ -std=c++17 -Wall -Wextra -O2 template.cpp -o day10
//   ./day10                 # ինտերակտիվ
//   ./day10 --selftest      # ստուգումներ
//
// Լրացրու TODO-ները։ Ամեն ֆունկցիա արդեն compile է լինում, որ կարողանաս
// աշխատել մեկ-մեկ և ստուգել --selftest-ով։

#include <sys/wait.h>
#include <unistd.h>

#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "../common/sys_check.h"

using Words = std::vector<std::string>;

struct Shell {
    int  last_status = 0;      // $? — վերջին հրամանի exit status-ը
    bool running     = true;   // false → exit
};

// ---------------------------------------------------------------- helpers (ready to use)

// Words → argv, որը պահանջում է execvp. վերջում nullptr։
// Ցուցիչները ցույց են տալիս words-ի ներսը, ուստի words-ը պետք է ապրի exec-ից առաջ։
static std::vector<char *> make_argv(Words &words)
{
    auto argv = std::vector<char *>{};
    for (auto &w : words) argv.push_back(w.data());
    argv.push_back(nullptr);
    return argv;
}

// Ընթացիկ թղթապանակը prompt-ի համար
static std::string current_dir()
{
    char buf[4096];
    return nullptr != getcwd(buf, sizeof buf) ? std::string(buf) : std::string("?");
}

// Երեխայում. փոխարինել ծրագիրը։ Եթե exec-ը ձախողվեց, տպել սխալը և _exit(127)։
// Ինչու _exit. exit-ը կդատարկեր ծնողից պատճենված stdio buffer-ները (Օր 10, §1)։
[[noreturn, maybe_unused]] static void exec_or_die(Words &words)
{
    auto argv = make_argv(words);
    execvp(argv[0], argv.data());
    std::fprintf(stderr, "%s: %s\n", argv[0], std::strerror(errno));
    _exit(127);
}

// ---------------------------------------------------------------- worked example

// Բաժանել տողը բառերի ըստ բացատների։ Չակերտներ չենք մշակում։
// "ls  -l /"  → {"ls", "-l", "/"}
static Words split_words(std::string const &line)
{
    auto in    = std::istringstream(line);
    auto words = Words{};
    for (auto w = std::string{}; in >> w;) words.push_back(w);
    return words;
}

// ---------------------------------------------------------------- TODO

// TODO 1. waitpid-ի raw status-ը → shell-ի $?
//   նորմալ ավարտ      → WEXITSTATUS(raw)
//   սպանված signal-ով → 128 + WTERMSIG(raw)
static int decode_status(int raw)
{
    (void)raw;
    return -1;
}

// TODO 2. Built-in հրամաններ։ Վերադարձնել true, եթե words[0]-ն built-in է (և կատարել այն)։
//   exit            → sh.running = false
//   cd [dir]        → chdir(dir կամ $HOME). սխալի դեպքում perror("cd") և last_status = 1, այլապես 0
//   status          → տպել sh.last_status-ը (status-ը ինքը չի փոխում last_status-ը)
// Ինչու cd-ն չի կարող լինել առանձին ծրագիր. Օր 2, environment-ի ժառանգումը։
static bool run_builtin(Words const &words, Shell &sh)
{
    (void)words;
    (void)sh;
    return false;
}

// TODO 3. Գործարկել արտաքին հրաման։
//   fork → երեխայում exec_or_die(words)
//   background == false → waitpid(pid) և վերադարձնել decode_status(...)
//   background == true  → տպել "[pid]\n" և վերադարձնել 0 (չսպասել)
static int run_command(Words words, bool background)
{
    (void)words;
    (void)background;
    return -1;
}

// TODO 4. Մաքրել ավարտված ֆոնային երեխաներին (zombie-ներին), առանց սպասելու.
//   waitpid(-1, &raw, WNOHANG) ցիկլով, քանի դեռ վերադարձնում է > 0
//   ամեն մեկի համար տպել "[pid] done, status N\n"
//   վերադարձնել մաքրվածների քանակը
static int reap_background()
{
    return 0;
}

// TODO 5. left | right.
//   pipe(fds)
//   երեխա 1. dup2(fds[1], STDOUT_FILENO), փակել երկու ծայրը, exec_or_die(left)
//   երեխա 2. dup2(fds[0], STDIN_FILENO),  փակել երկու ծայրը, exec_or_die(right)
//   ծնող. փակել երկու ծայրը (այլապես right-ը երբեք EOF չի ստանա), սպասել երկուսին
//   վերադարձնել right-ի status-ը (ինչպես bash-ը)
static int run_pipeline(Words left, Words right)
{
    (void)left;
    (void)right;
    return -1;
}

// ---------------------------------------------------------------- ready: one line → action

static void execute(std::string const &line, Shell &sh)
{
    auto words = split_words(line);
    if (words.empty()) return;

    auto background = false;
    if ("&" == words.back()) { background = true; words.pop_back(); }
    if (words.empty()) return;

    for (auto i = std::size_t{0}; i < words.size(); ++i) {
        if ("|" != words[i]) continue;
        auto left  = Words(words.begin(), words.begin() + i);
        auto right = Words(words.begin() + i + 1, words.end());
        if (left.empty() || right.empty()) {
            std::fprintf(stderr, "syntax error near '|'\n");
            sh.last_status = 2;
            return;
        }
        sh.last_status = run_pipeline(left, right);
        return;
    }

    if (run_builtin(words, sh)) return;
    sh.last_status = run_command(words, background);
}

// ---------------------------------------------------------------- self-test

static int g_fail = 0;

static void check(bool ok, char const *what)
{
    std::printf("%s  %s\n", ok ? "PASS" : "FAIL", what);
    if (!ok) ++g_fail;
}

static int selftest()
{
    // decode_status-ի համար raw արժեքները ստանում ենք իրական երեխաներից
    auto raw_of = [](char const *script) {
        auto const pid = SYS_CHECK(fork());
        if (0 == pid) { execl("/bin/sh", "sh", "-c", script, (char *)nullptr); _exit(127); }
        auto raw = 0;
        SYS_CHECK(waitpid(pid, &raw, 0));
        return raw;
    };
    check(0   == decode_status(raw_of("exit 0")),    "decode_status: exit 0 → 0");
    check(3   == decode_status(raw_of("exit 3")),    "decode_status: exit 3 → 3");
    check(137 == decode_status(raw_of("kill -9 $$")), "decode_status: SIGKILL → 137");
    check(143 == decode_status(raw_of("kill $$")),    "decode_status: SIGTERM → 143");

    std::fflush(stdout);   // մինչև fork. այլապես buffer-ը կտպվի երկու անգամ
    check(0   == run_command({"true"}, false),               "run_command: true → 0");
    check(1   == run_command({"false"}, false),              "run_command: false → 1");
    check(127 == run_command({"no-such-cmd-day10"}, false),  "run_command: unknown → 127");
    check(5   == run_command({"sh", "-c", "exit 5"}, false), "run_command: argv passed → 5");

    auto sh = Shell{};
    check(run_builtin({"cd", "/tmp"}, sh) && "/tmp" == current_dir() && 0 == sh.last_status,
          "builtin cd /tmp");
    check(run_builtin({"cd", "/no/such/dir"}, sh) && 1 == sh.last_status, "builtin cd: bad dir → 1");
    check(!run_builtin({"ls"}, sh), "builtin: ls is not a builtin");
    check(run_builtin({"exit"}, sh) && !sh.running, "builtin exit");

    check(0 == run_pipeline({"echo", "a", "b", "c"}, {"grep", "-q", "b"}), "pipeline: echo | grep -q b → 0");
    check(1 == run_pipeline({"echo", "abc"}, {"grep", "-q", "x"}),         "pipeline: echo | grep -q x → 1");
    check(0 == run_pipeline({"yes"}, {"head", "-n", "1"}),                 "pipeline: yes | head ends (SIGPIPE)");

    std::fflush(stdout);
    check(0 == run_command({"true"}, true), "background: returns 0 at once");
    usleep(200 * 1000);
    std::fflush(stdout);
    check(1 == reap_background(), "reap_background: one zombie cleaned");
    check(0 == reap_background(), "reap_background: nothing left");

    std::printf("\n%s (%d failed)\n", 0 == g_fail ? "ALL PASSED" : "SOME FAILED", g_fail);
    return 0 == g_fail ? 0 : 1;
}

// ---------------------------------------------------------------- main

int main(int argc, char **argv)
{
    if (1 < argc && 0 == std::strcmp(argv[1], "--selftest")) return selftest();

    auto       sh          = Shell{};
    auto const interactive = isatty(STDIN_FILENO);

    for (auto line = std::string{}; sh.running;) {
        reap_background();
        if (interactive) std::printf("day10:%s$ ", current_dir().c_str());
        std::fflush(stdout);
        if (!std::getline(std::cin, line)) break;   // Ctrl-D
        execute(line, sh);
    }
    return sh.last_status;
}
