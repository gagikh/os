#!/usr/bin/env bash
# Day 1: System report as a shell script
# Goal: replace every TODO with a command from today's lecture, so the script
#       prints a short report about the board it runs on.
#
# Run:  bash sysinfo.sh
# Save: bash sysinfo.sh > report.txt
#
# $( command ) runs the command and puts its output into the text.
# Try each command alone in the terminal first, then paste it here.

echo "=== System report: $(date '+%Y-%m-%d %H:%M') ==="     # already done, as an example
echo "user    : $(whoami) on $(hostname)"                    # already done

# TODO 1: board model.        Hint: cat /proc/device-tree/model  (pipe it into: tr -d '\0')
echo "board   : TODO"

# TODO 2: OS name.            Hint: grep PRETTY_NAME /etc/os-release
echo "os      : TODO"

# TODO 3: kernel version and CPU architecture.   Hint: uname -r ; uname -m
echo "kernel  : TODO"

# TODO 4: number of CPU cores.                   Hint: nproc
echo "cores   : TODO"

# TODO 5: free and total memory.                 Hint: free -h
echo "memory  : TODO"

# TODO 6: free space on the root disk /.         Hint: df -h /
echo "disk /  : TODO"

# TODO 7: IP address of the board.               Hint: hostname -I
echo "ip      : TODO"

# TODO 8: how long the board has been running.   Hint: uptime -p
echo "uptime  : TODO"

# TODO 9: CPU temperature. The file holds millidegrees: 45000 means 45.0 C.
#         Hint: cat /sys/class/thermal/thermal_zone0/temp
echo "temp    : TODO"

# TODO 10: the last 5 kernel messages.           Hint: dmesg | tail -n 5   (may need sudo)
echo "--- last 5 kernel messages ---"
