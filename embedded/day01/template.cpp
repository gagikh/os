// Day 1: System report in C++
// Goal: print the same report as sysinfo.sh, but by reading the kernel's text
//       files directly from a program instead of calling shell commands.
//
// Native build:  g++ -std=c++17 -Wall -Wextra -O2 template.cpp -o sysinfo
// Cross build:   aarch64-linux-gnu-g++ -std=c++17 -Wall -Wextra -O2 template.cpp -o sysinfo
// Run:           ./sysinfo
// No board?      Runs on any Linux PC or WSL2; board-only files print a fallback.
//
// Before each TODO, look at the file in the shell first:
//     cat -A /proc/device-tree/model
//     head -3 /proc/meminfo
//     cat /proc/uptime
//     cat /sys/class/thermal/thermal_zone0/temp

#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>
#include <sys/utsname.h>
#include <unistd.h>

// ---------------------------------------------------------------------------
// Helpers: ready to use, read them but do not change them
// ---------------------------------------------------------------------------

// First line of a text file, or `fallback` if the file cannot be opened.
std::string read_first_line(const std::string &path, const std::string &fallback)
{
    auto in = std::ifstream(path);
    auto line = std::string{};
    if (!in || !std::getline(in, line)) return fallback;
    return line;
}

// Value after "key<sep>" in a file such as /etc/os-release or /proc/meminfo.
std::string read_key(const std::string &path, const std::string &key, char sep)
{
    auto in = std::ifstream(path);
    auto line = std::string{};
    while (std::getline(in, line)) {
        if (0 == line.rfind(key + sep, 0)) return line.substr(key.size() + 1);
    }
    return "n/a";
}

// Removes spaces, tabs and quotes from both ends: "  \"Ubuntu\"" -> "Ubuntu".
std::string trim(const std::string &s)
{
    auto const first = s.find_first_not_of(" \t\"");
    auto const last = s.find_last_not_of(" \t\"");
    if (std::string::npos == first) return "";
    return s.substr(first, last - first + 1);
}

// ---------------------------------------------------------------------------
// Worked example: read it before you start
// ---------------------------------------------------------------------------

// Distribution name. /etc/os-release has a line: PRETTY_NAME="Ubuntu 22.04.5 LTS"
std::string os_name()
{
    return trim(read_key("/etc/os-release", "PRETTY_NAME", '='));
}

// ---------------------------------------------------------------------------
// Your turn: replace each TODO body
// ---------------------------------------------------------------------------

// TODO 1: board name from /proc/device-tree/model.
//   - use read_first_line() with the fallback "unknown (no device tree)"
//   - the string ends with '\0' (you saw ^@ with cat -A): remove it with pop_back()
std::string board_model()
{
    return "TODO";
}

// TODO 2: kernel release and CPU architecture, e.g. "5.15.148-tegra (aarch64)".
//   - auto u = utsname{}; uname(&u);  then use u.release and u.machine (see hello.cpp)
std::string kernel_info()
{
    return "TODO";
}

// TODO 3: number of CPU cores that are online: sysconf(_SC_NPROCESSORS_ONLN).
long cpu_cores()
{
    return 0;
}

// TODO 4: memory in MB from /proc/meminfo. The values there are in kB:
//         "MemTotal:        7999124 kB"
//   - read_key("/proc/meminfo", key, ':') returns "        7999124 kB"
//   - std::stol() skips the leading spaces and stops at " kB"
//   - divide by 1024
long memory_mb(const std::string &key)    // key is "MemTotal" or "MemAvailable"
{
    (void)key;    // remove this line when you use `key`
    return 0;
}

// TODO 5: hours since boot. /proc/uptime holds two numbers: "35123.45 69000.12";
//         the first one is seconds since boot. std::stod() reads it.
double uptime_hours()
{
    return 0.0;
}

// TODO 6: CPU temperature in degrees Celsius, e.g. "47.5 C".
//   - /sys/class/thermal/thermal_zone0/temp holds millidegrees: 47500 means 47.5 C
//   - return "n/a" if the file is missing (use "" as the fallback and check empty())
//   - auto const t = std::stol(raw);
//     return std::to_string(t / 1000) + "." + std::to_string(t % 1000 / 100) + " C";
std::string cpu_temperature()
{
    return "TODO";
}

// ---------------------------------------------------------------------------
// main: already complete, it calls your functions
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
