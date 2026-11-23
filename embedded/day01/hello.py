#!/usr/bin/env python3
# Day 1: Hello in Python
# Same output as hello.c. Python is interpreted: there is no compile step,
# and the same file runs on the PC and on every board.
#
# Run:  python3 hello.py
#  or:  chmod +x hello.py && ./hello.py     (uses the #! line above)

import os
import platform

u = platform.uname()

print("Hello from Python!")
print(f"  host    : {u.node}")
print(f"  machine : {u.machine}")
print(f"  kernel  : {u.system} {u.release}")
print(f"  pid     : {os.getpid()}")
