// Day 1: Hello in C++
// Same output as hello.c, written in C++.
//
// Native build (on the board):   g++ -std=c++17 -Wall -O2 hello.cpp -o hello_cpp
// Cross build (on an x86 PC):    aarch64-linux-gnu-g++ -std=c++17 -Wall -O2 hello.cpp -o hello_cpp
// Run:                           ./hello_cpp

#include <cstdio>
#include <iostream>
#include <sys/utsname.h>
#include <unistd.h>

int main()
{
    auto u = utsname{};
    if (0 != uname(&u)) {
        std::perror("uname");
        return 1;
    }

    std::cout << "Hello from C++!\n"
              << "  host    : " << u.nodename << '\n'
              << "  machine : " << u.machine << '\n'
              << "  kernel  : " << u.sysname << ' ' << u.release << '\n'
              << "  pid     : " << getpid() << '\n';
    return 0;
}
