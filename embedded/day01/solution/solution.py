#!/usr/bin/env python3
# Day 1 solution: system report in Python.
# Same structure as ../template.py, with every TODO filled in.
#
# Run:  python3 solution.py

import os
import platform
from pathlib import Path


def read_text(path, fallback):
    try:
        return Path(path).read_text()
    except OSError:
        return fallback


def read_key(path, key, sep):
    for line in read_text(path, "").splitlines():
        if line.startswith(key + sep):
            return line[len(key) + 1:].strip().strip('"')
    return "n/a"


def os_name():
    return read_key("/etc/os-release", "PRETTY_NAME", "=")


def board_model():
    return read_text("/proc/device-tree/model", "unknown (no device tree)").rstrip("\0")


def kernel_info():
    u = platform.uname()
    return f"{u.release} ({u.machine})"


def cpu_cores():
    return os.cpu_count()


def memory_mb(key):
    value = read_key("/proc/meminfo", key, ":")
    return int(value.split()[0]) // 1024 if value != "n/a" else 0


def uptime_hours():
    return float(read_text("/proc/uptime", "0").split()[0]) / 3600


def cpu_temperature():
    raw = read_text("/sys/class/thermal/thermal_zone0/temp", "").strip()
    return f"{int(raw) / 1000:.1f} C" if raw else "n/a"


print("=== System report ===")
print(f"board   : {board_model()}")
print(f"os      : {os_name()}")
print(f"kernel  : {kernel_info()}")
print(f"cores   : {cpu_cores()}")
print(f"memory  : {memory_mb('MemAvailable')} MB free of {memory_mb('MemTotal')} MB")
print(f"uptime  : {uptime_hours():.1f} h")
print(f"temp    : {cpu_temperature()}")
