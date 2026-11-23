#!/usr/bin/env bash
# Day 1 solution: system report as a shell script.
# Every line is one command from today's lecture, glued together with pipes.
#
# Run:  bash sysinfo.sh            (prints the report)
#       bash sysinfo.sh > report.txt  (saves it to a file)

echo "=== System report: $(date '+%Y-%m-%d %H:%M') ==="
echo "user    : $(whoami) on $(hostname)"
echo "board   : $(tr -d '\0' 2>/dev/null < /proc/device-tree/model || echo 'unknown (no device tree)')"
echo "os      : $(grep PRETTY_NAME /etc/os-release | cut -d= -f2 | tr -d '"')"
echo "kernel  : $(uname -r) ($(uname -m))"
echo "cores   : $(nproc)"
echo "memory  : $(free -h | awk '/^Mem:/ {print $7 " free of " $2}')"
echo "disk /  : $(df -h / | awk 'NR==2 {print $4 " free of " $2}')"
echo "ip      : $(hostname -I 2>/dev/null | awk '{print $1}')"
echo "uptime  : $(uptime -p)"

if [ -r /sys/class/thermal/thermal_zone0/temp ]; then
    echo "temp    : $(awk '{printf "%.1f C", $1 / 1000}' /sys/class/thermal/thermal_zone0/temp)"
else
    echo "temp    : n/a"
fi

echo "--- last 5 kernel messages ---"
# dmesg may need sudo on newer distributions (kernel.dmesg_restrict=1)
if dmesg > /dev/null 2>&1; then
    dmesg | tail -n 5
else
    echo "(no permission: run with sudo to see dmesg)"
fi
