// Day 1 solution: system report in C++.
// Same structure as ../template.cpp, with every TODO filled in.
//
// Build:  g++ -std=c++17 -Wall -Wextra -O2 solution.cpp -o sysinfo_solution
// Run:    ./sysinfo_solution

#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>
#include <sys/utsname.h>
#include <unistd.h>

// ---------------------------------------------------------------------------
// Helpers (same as in the template)
// ---------------------------------------------------------------------------

std::string read_first_line(const std::string &path, const std::string &fallback)
{
    auto in = std::ifstream(path);
    auto line = std::string{};
    if (!in || !std::getline(in, line)) return fallback;
    return line;
}

std::string read_key(const std::string &path, const std::string &key, char sep)
{
    auto in = std::ifstream(path);
    auto line = std::string{};
    while (std::getline(in, line)) {
        if (0 == line.rfind(key + sep, 0)) return line.substr(key.size() + 1);
    }
    return "n/a";
}

std::string trim(const std::string &s)
{
    auto const first = s.find_first_not_of(" \t\"");
    auto const last = s.find_last_not_of(" \t\"");
    if (std::string::npos == first) return "";
    return s.substr(first, last - first + 1);
}

// ---------------------------------------------------------------------------
// Report items
// ---------------------------------------------------------------------------

std::string os_name()
{
    return trim(read_key("/etc/os-release", "PRETTY_NAME", '='));
}

// 1. The device tree string ends with '\0'; a PC has no device tree at all.
std::string board_model()
{
    auto model = read_first_line("/proc/device-tree/model", "unknown (no device tree)");
    if (!model.empty() && '\0' == model.back()) model.pop_back();
    return model;
}

// 2. uname() is a system call: the kernel fills the struct for us.
std::string kernel_info()
{
    auto u = utsname{};
    if (0 != uname(&u)) return "n/a";
    return std::string(u.release) + " (" + u.machine + ")";
}

// 3. Cores that are online right now (some boards switch cores off to save power).
long cpu_cores()
{
    return sysconf(_SC_NPROCESSORS_ONLN);
}

// 4. "MemTotal:        7999124 kB" -> 7811 MB
long memory_mb(const std::string &key)
{
    auto const value = read_key("/proc/meminfo", key, ':');
    if ("n/a" == value) return 0;
    return std::stol(value) / 1024;
}

// 5. "35123.45 69000.12" -> 9.8 h
double uptime_hours()
{
    return std::stod(read_first_line("/proc/uptime", "0")) / 3600.0;
}

// 6. "47500" -> "47.5 C"
std::string cpu_temperature()
{
    auto const raw = read_first_line("/sys/class/thermal/thermal_zone0/temp", "");
    if (raw.empty()) return "n/a";
    auto const t = std::stol(raw);
    return std::to_string(t / 1000) + "." + std::to_string(t % 1000 / 100) + " C";
}

// ---------------------------------------------------------------------------
int main()
{
    std::cout << std::fixed << std::setprecision(1)
              << "=== System report ===\n"
              << "board   : " << board_model() << '\n'
              << "os      : " << os_name() << '\n'
              << "kernel  : " << kernel_info() << '\n'
              << "cores   : " << cpu_cores() << '\n'
              << "memory  : " << memory_mb("MemAvailable") << " MB free of "
                              << memory_mb("MemTotal") << " MB\n"
              << "uptime  : " << uptime_hours() << " h\n"
              << "temp    : " << cpu_temperature() << '\n';
    return 0;
}
