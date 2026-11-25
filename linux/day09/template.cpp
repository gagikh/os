// Day 9: Hardware and Drivers
// Goal: a hardware inventory built only from the kernel's files: /proc, /sys, /dev.
//       The same information lscpu, lsblk, ip link and lsusb print.
//
//     ./day09
//     cpu      : Intel(R) Core(TM) i7-8565U CPU @ 1.80GHz, 8 logical CPUs
//     block    : nvme0n1   476.9 GiB  259:0   fixed
//     net      : enp0s31f6 d8:9e:f3:12:34:56  up
//     usb      : 046d:c52b USB Receiver
//     random   : 8f3a91c0e2b45d17   (8 bytes from /dev/urandom)
//
// Compile:  g++ -std=c++17 -Wall -Wextra -O2 template.cpp -o day09
// Runs on any Linux; on a board you will see mmcblk0 instead of nvme0n1, and maybe no USB.

#include <algorithm>
#include <cstdio>
#include <fcntl.h>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <unistd.h>
#include <vector>
#include "../common/sys_check.h"

namespace fs = std::filesystem;

// ---------------------------------------------------------------------------
// Helpers: ready to use, read them but do not change them
// ---------------------------------------------------------------------------

// First line of a file, or "" if it cannot be read (many /sys files are optional).
std::string read_line(const fs::path &path)
{
    auto in = std::ifstream(path);
    auto line = std::string{};
    std::getline(in, line);
    return line;
}

// Names inside a directory, sorted; empty if it does not exist.
std::vector<std::string> list_dir(const fs::path &dir)
{
    auto names = std::vector<std::string>{};
    auto ec = std::error_code{};
    for (auto &entry : fs::directory_iterator(dir, ec)) names.push_back(entry.path().filename().string());
    std::sort(names.begin(), names.end());
    return names;
}

// "476.9 GiB" from a byte count
std::string human_size(double bytes)
{
    const char *units[] = {"B", "KiB", "MiB", "GiB", "TiB"};
    auto u = 0;
    while (bytes >= 1024.0 && u < 4) { bytes /= 1024.0; ++u; }
    char buf[32];
    std::snprintf(buf, sizeof(buf), "%.1f %s", bytes, units[u]);
    return buf;
}

// ---------------------------------------------------------------------------
// Your turn: replace each TODO body
// ---------------------------------------------------------------------------

// TODO 1: CPU model and the number of logical CPUs, from /proc/cpuinfo.
//   - every logical CPU has a line "processor\t: N": count them
//   - the model is on the first line starting with "model name" (x86),
//     or "Model" / "Hardware" (ARM boards: Raspberry Pi, Jetson); take the text after ": "
//   - return e.g. "Intel(R) Xeon(R) ..., 8 logical CPUs"; "unknown" if nothing found
std::string cpu_summary()
{
    return "TODO";
}

// TODO 2: one line per block device in /sys/block, e.g. "nvme0n1   476.9 GiB  259:0   fixed"
//   - for each name in list_dir("/sys/block"):
//       size      : read_line(dir / "size") is in 512-byte SECTORS, whatever the device
//       skip devices of size 0 (unused loop0..loop7)
//       major:minor: read_line(dir / "dev")
//       removable : read_line(dir / "removable") == "1"  -> "removable", else "fixed"
std::vector<std::string> block_devices()
{
    return {};
}

// TODO 3: one line per network interface in /sys/class/net: "eth0  02:fc:00:00:00:01  up"
//   - files: address (the MAC), operstate (up / down / unknown)
std::vector<std::string> net_interfaces()
{
    return {};
}

// TODO 4: one line per USB device in /sys/bus/usb/devices: "046d:c52b  USB Receiver"
//   - only directories that HAVE an idVendor file are devices (the others are interfaces)
//   - files: idVendor, idProduct, product (product may be missing: print "?")
//   - no /sys/bus/usb at all (a VM, a container)? return an empty list
std::vector<std::string> usb_devices()
{
    return {};
}

// TODO 5: n random bytes from /dev/urandom, as hex: "8f3a91c0e2b45d17"
//   - a device is opened and read like a file (README §3):
//       auto const fd = SYS_CHECK(open("/dev/urandom", O_RDONLY));
//       SYS_CHECK(read(fd, buf, n));  close(fd);
//   - format each byte with "%02x"
std::string random_hex(std::size_t n)
{
    (void)n;    // remove this line when you use the parameter
    return "TODO";
}

// ---------------------------------------------------------------------------
// main: already complete, it calls your functions
// ---------------------------------------------------------------------------
int main()
{
    std::cout << "=== Hardware inventory ===\n";
    std::cout << "cpu      : " << cpu_summary() << '\n';
    for (auto &line : block_devices()) std::cout << "block    : " << line << '\n';
    for (auto &line : net_interfaces()) std::cout << "net      : " << line << '\n';
    auto const usb = usb_devices();
    if (usb.empty()) std::cout << "usb      : (none visible)\n";
    for (auto &line : usb) std::cout << "usb      : " << line << '\n';
    std::cout << "random   : " << random_hex(8) << "   (8 bytes from /dev/urandom)\n";
    return 0;
}
