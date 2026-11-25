// Day 8: Processes, Signals, Services and Packages
// Goal: a small service that reacts to signals the way real daemons do:
//       SIGTERM / SIGINT  -> finish the current tick, clean up, exit 0
//       SIGHUP            -> re-read the config file
//       SIGUSR1           -> print statistics
//
//     printf 'interval_ms=500\nmessage=working\n' > day08.conf
//     ./day08 --config day08.conf &
//     kill -USR1 $! ; kill -HUP $! ; kill $! ; wait $! ; echo $?
//
// Compile:  g++ -std=c++17 -Wall -Wextra -O2 template.cpp -o day08
// As a systemd service: see day08.service

#include <cerrno>
#include <csignal>
#include <cstring>
#include <ctime>
#include <fstream>
#include <iostream>
#include <string>
#include <unistd.h>
#include "../common/sys_check.h"

// ---------------------------------------------------------------------------
// Flags set by the signal handler and read by the main loop.
// A handler may run in the middle of ANY line of the program, so it must not
// call printf, new, std::string, ...: it only sets one of these.
// ---------------------------------------------------------------------------
volatile sig_atomic_t g_stop = 0;      // SIGTERM or SIGINT arrived: holds the signal number
volatile sig_atomic_t g_reload = 0;    // SIGHUP arrived
volatile sig_atomic_t g_report = 0;    // SIGUSR1 arrived

// ---------------------------------------------------------------------------
// Helpers: ready to use, read them but do not change them
// ---------------------------------------------------------------------------

struct config_t {
    long interval_ms = 1000;
    std::string message = "tick";
};

// "[12:00:01] text" on stderr. Under systemd, stderr goes to the journal.
void log(const std::string &text)
{
    auto const now = std::time(nullptr);
    char stamp[16];
    std::strftime(stamp, sizeof(stamp), "%H:%M:%S", std::localtime(&now));
    std::cerr << '[' << stamp << "] " << text << std::endl;
}

// Sleep, but wake up early when a signal arrives: nanosleep then fails with EINTR.
void sleep_ms(long ms)
{
    auto ts = timespec{ms / 1000, (ms % 1000) * 1000000L};
    if (-1 == nanosleep(&ts, nullptr) && EINTR != errno) std::perror("nanosleep");
}

// ---------------------------------------------------------------------------
// Your turn: replace each TODO body
// ---------------------------------------------------------------------------

// TODO 1: the signal handler. Only set flags, nothing else:
//   SIGTERM or SIGINT -> g_stop = sig     SIGHUP -> g_reload = 1     SIGUSR1 -> g_report = 1
void on_signal(int sig)
{
    (void)sig;    // remove this line when you use the parameter
}

// TODO 2: install on_signal for SIGTERM, SIGINT, SIGHUP and SIGUSR1 with sigaction.
//   - struct sigaction sa = {};  sa.sa_handler = on_signal;  sigemptyset(&sa.sa_mask);
//   - SYS_CHECK(sigaction(SIGTERM, &sa, nullptr)); ... for each signal
//   - (do NOT set SA_RESTART: we want nanosleep to be interrupted)
void install_handlers()
{
}

// TODO 3: read "key=value" lines into cfg. Keys: interval_ms (a number), message (text).
//   - return false if the file cannot be opened (cfg stays unchanged)
//   - ignore empty lines, lines starting with '#', and unknown keys
//   - an interval_ms below 10 is too fast: keep the old value and log a warning
bool load_config(const std::string &path, config_t &cfg)
{
    (void)path;
    (void)cfg;    // remove these lines when you use the parameters
    return false;
}

// TODO 4: react to the flags. Called by main once per loop, BEFORE sleeping.
//   - g_reload: clear it, call load_config, log "config reloaded: interval N ms"
//               (or "config reload failed, keeping the old one")
//   - g_report: clear it, log "stats: T ticks, S s, interval N ms"
//               (seconds since start = std::time(nullptr) - started)
void handle_flags(const std::string &path, config_t &cfg, long ticks, std::time_t started)
{
    (void)path;
    (void)cfg;
    (void)ticks;
    (void)started;    // remove these lines when you use the parameters
}

// ---------------------------------------------------------------------------
// main: already complete, it calls your functions
// ---------------------------------------------------------------------------
int main(int argc, char *argv[])
{
    if (3 != argc || 0 != std::strcmp(argv[1], "--config")) {
        std::cerr << "Usage: day08 --config FILE\n";
        return 1;
    }
    auto const path = std::string(argv[2]);
    auto cfg = config_t{};
    if (!load_config(path, cfg)) log("cannot read " + path + ", using defaults");

    install_handlers();
    log("started, pid " + std::to_string(getpid()) + ", interval " + std::to_string(cfg.interval_ms) + " ms");

    auto const started = std::time(nullptr);
    auto ticks = 0L;
    while (0 == g_stop) {
        handle_flags(path, cfg, ticks, started);
        ++ticks;
        log(cfg.message + " (tick " + std::to_string(ticks) + ")");
        sleep_ms(cfg.interval_ms);
    }

    log(std::string(SIGTERM == g_stop ? "SIGTERM" : "SIGINT") + ": stopped cleanly after " +
        std::to_string(ticks) + " ticks");
    return 0;
}
