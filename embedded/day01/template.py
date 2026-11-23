#!/usr/bin/env python3
# Day 1: System report in Python
# Goal: the same report as template.cpp, in Python. Compare how much shorter it
#       is, and notice that both programs only read text files from /proc, /sys
#       and /etc.
#
# Run:  python3 template.py

import os
import platform
from pathlib import Path


# ---------------------------------------------------------------------------
# Helpers: ready to use, read them but do not change them
# ---------------------------------------------------------------------------

def read_text(path, fallback):
    """Whole file as text, or `fallback` if it cannot be read."""
    try:
        return Path(path).read_text()
    except OSError:
        return fallback


def read_key(path, key, sep):
    """Value after "key<sep>" in a file such as /etc/os-release, without quotes."""
    for line in read_text(path, "").splitlines():
        if line.startswith(key + sep):
            return line[len(key) + 1:].strip().strip('"')
    return "n/a"


# ---------------------------------------------------------------------------
# Worked example: read it before you start
# ---------------------------------------------------------------------------

def os_name():
    """Distribution name from PRETTY_NAME="..." in /etc/os-release."""
    return read_key("/etc/os-release", "PRETTY_NAME", "=")


# ---------------------------------------------------------------------------
# Your turn: replace each TODO body
# ---------------------------------------------------------------------------

def board_model():
    """TODO 1: /proc/device-tree/model without the trailing '\\0' (str.rstrip("\\0")).
    Fallback: "unknown (no device tree)"."""
    return "TODO"


def kernel_info():
    """TODO 2: "<release> (<machine>)" from platform.uname()."""
    return "TODO"


def cpu_cores():
    """TODO 3: os.cpu_count()."""
    return 0


def memory_mb(key):
    """TODO 4: MemTotal or MemAvailable from /proc/meminfo, in MB.
    read_key returns "7999124 kB": split() it, int() the first part, // 1024."""
    return 0


def uptime_hours():
    """TODO 5: first number in /proc/uptime is seconds since boot. Return hours."""
    return 0.0


def cpu_temperature():
    """TODO 6: /sys/class/thermal/thermal_zone0/temp is in millidegrees.
    Return e.g. "47.5 C", or "n/a" if the file is missing."""
    return "TODO"


# ---------------------------------------------------------------------------
# Already complete: it calls your functions
# ---------------------------------------------------------------------------

print("=== System report ===")
print(f"board   : {board_model()}")
print(f"os      : {os_name()}")
print(f"kernel  : {kernel_info()}")
print(f"cores   : {cpu_cores()}")
print(f"memory  : {memory_mb('MemAvailable')} MB free of {memory_mb('MemTotal')} MB")
print(f"uptime  : {uptime_hours():.1f} h")
print(f"temp    : {cpu_temperature()}")
