// Day 1: Introduction to Linux
// Goal: write a "passport" of the running kernel: its version, who built it,
//       which distribution surrounds it, how many modules are loaded, and
//       whether the kernel is tainted (for example by a non-GPL module).
//
// Compile:  g++ -std=c++17 -Wall -Wextra -O2 template.cpp -o day01
// Run:      ./day01
// Works on: any Linux (Ubuntu, WSL2, a VM, a board). Not on macOS: no /proc there.
//
// Before each TODO, look at the file in the shell first:
//     uname -a
//     cat /proc/version
//     cat /etc/os-release
//     head -3 /proc/modules ; wc -l /proc/modules
//     cat /proc/sys/kernel/tainted

#include <fstream>
#include <iostream>
#include <string>
#include <sys/utsname.h>
#include "../common/sys_check.h"

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

// Value after "key<sep>" in a file such as /etc/os-release, or "" if absent.
std::string read_key(const std::string &path, const std::string &key, char sep)
{
    auto in = std::ifstream(path);
    auto line = std::string{};
    while (std::getline(in, line)) {
        if (0 == line.rfind(key + sep, 0)) return line.substr(key.size() + 1);
    }
    return "";
}

// Removes spaces, tabs and quotes from both ends: "  \"Ubuntu\"" -> "Ubuntu".
std::string trim(const std::string &s)
{
    auto const first = s.find_first_not_of(" \t\"");
    auto const last = s.find_last_not_of(" \t\"");
    if (std::string::npos == first) return "";
    return s.substr(first, last - first + 1);
}

// The taint flags from https://docs.kernel.org/admin-guide/tainted-kernels.html
// kTaintFlags[bit] describes bit number `bit` of /proc/sys/kernel/tainted.
struct taint_flag_t {
    char letter;
    const char *meaning;
};

constexpr taint_flag_t kTaintFlags[] = {
    {'P', "a proprietary (non-GPL) module was loaded"},
    {'F', "a module was force loaded"},
    {'S', "the kernel runs on an out-of-specification system"},
    {'R', "a module was force unloaded"},
    {'M', "a processor reported a Machine Check Exception"},
    {'B', "a bad page reference or unexpected page flags were found"},
    {'U', "a user-space application requested the taint"},
    {'D', "the kernel recently died (an OOPS or BUG)"},
    {'A', "an ACPI table was overridden by the user"},
    {'W', "the kernel issued a warning"},
    {'C', "a staging driver was loaded"},
    {'I', "a workaround for a platform firmware bug was applied"},
    {'O', "an externally built (out-of-tree) module was loaded"},
    {'E', "an unsigned module was loaded"},
    {'L', "a soft lockup occurred"},
    {'K', "the kernel was live patched"},
    {'X', "auxiliary taint, defined by the distribution"},
    {'T', "the kernel was built with the randstruct plugin"},
    {'N', "an in-kernel test (such as KUnit) was run"},
    {'J', "user space used a mutating debug operation in fwctl"},
};
constexpr auto kTaintFlagCount = sizeof(kTaintFlags) / sizeof(kTaintFlags[0]);   // 20

// ---------------------------------------------------------------------------
// Worked example: read it before you start
// ---------------------------------------------------------------------------

// Distribution name. /etc/os-release has a line: PRETTY_NAME="Ubuntu 24.04.1 LTS"
std::string distribution()
{
    auto const name = trim(read_key("/etc/os-release", "PRETTY_NAME", '='));
    return name.empty() ? "unknown" : name;
}

// ---------------------------------------------------------------------------
// Your turn: replace each TODO body
// ---------------------------------------------------------------------------

// TODO 1: kernel release and CPU architecture, e.g. "6.8.0-49-generic (x86_64)".
//   - auto u = utsname{};  SYS_CHECK(uname(&u));
//   - use u.release and u.machine.   man 2 uname
std::string kernel_release()
{
    return "TODO";
}

// TODO 2: who built the kernel. /proc/version looks like:
//     Linux version 6.8.0-49-generic (buildd@lcy02-amd64-103) (x86_64-linux-gnu-gcc-12 ...) ...
//   Return the text inside the FIRST pair of parentheses: "buildd@lcy02-amd64-103".
//   - read_first_line(), then std::string::find('(') and find(')', start)
std::string kernel_builder()
{
    return "TODO";
}

// TODO 3: distribution family from /etc/os-release, e.g. "ubuntu, like debian".
//   - ID=ubuntu and ID_LIKE=debian. Debian itself has only ID=debian (no ID_LIKE):
//     return just "debian" then.
std::string distribution_family()
{
    return "TODO";
}

// TODO 4: number of loaded kernel modules = number of lines in /proc/modules.
//   - std::ifstream + a while (std::getline(...)) loop. Compare with: lsmod | wc -l
//     (lsmod prints one extra header line).
int module_count()
{
    return 0;
}

// TODO 5: the taint value from /proc/sys/kernel/tainted (a decimal number).
//   - std::stoul(read_first_line(...)). Return 0 if the file cannot be read.
unsigned long taint_value()
{
    return 0;
}

// TODO 6: print one line per set bit of `value`, e.g.
//     P (bit 0): a proprietary (non-GPL) module was loaded
//   - loop bit = 0 .. kTaintFlagCount - 1
//   - a bit is set when 0 != ((value >> bit) & 1u)
//   - kTaintFlags[bit].letter and kTaintFlags[bit].meaning give the text
void print_taint_flags(unsigned long value)
{
    (void)value;    // remove this line when you use `value`
    std::cout << "  TODO\n";
}

// ---------------------------------------------------------------------------
// main: already complete, it calls your functions
// ---------------------------------------------------------------------------
int main()
{
    std::cout << "=== Kernel passport ===\n"
              << "kernel        : " << kernel_release() << '\n'
              << "built by      : " << kernel_builder() << '\n'
              << "distribution  : " << distribution() << '\n'
              << "family        : " << distribution_family() << '\n'
              << "modules       : " << module_count() << " loaded\n";

    auto const taint = taint_value();
    std::cout << "tainted       : " << taint << '\n';
    if (0 == taint)
        std::cout << "  clean: only GPL modules, no errors since boot\n";
    else
        print_taint_flags(taint);
    return 0;
}
