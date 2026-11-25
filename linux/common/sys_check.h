// common/sys_check.h
//
// Error checking for Linux system calls, used by every day's template.cpp.
// Include with a relative path, e.g. from dayNN/template.cpp:
//
//   #include "../common/sys_check.h"
//
// Why this exists: most system calls (open, read, fork, uname, ...) report an
// error by returning -1 and leaving the reason in the global variable `errno`.
// Nothing stops the program if you ignore that -1: it simply continues with a
// bad file descriptor or an empty buffer and fails somewhere later, far from
// the real cause. SYS_CHECK stops at the exact line and prints why.
#pragma once

#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <cstring>

// Wrap any call that returns -1 on error:
//   SYS_CHECK(uname(&u));
//   auto const fd = SYS_CHECK(open("data.txt", O_RDONLY));   // the value is passed through
//
// On error prints, for example:
//   template.cpp:42: open("data.txt", O_RDONLY) failed: No such file or directory
#define SYS_CHECK(call)                                                        \
    ([&]() {                                                                   \
        auto const ret__ = (call);                                             \
        if (-1 == ret__) {                                                     \
            std::fprintf(stderr, "%s:%d: %s failed: %s\n",                     \
                         __FILE__, __LINE__, #call, std::strerror(errno));     \
            std::exit(EXIT_FAILURE);                                           \
        }                                                                      \
        return ret__;                                                          \
    }())
