// Day 1: Hello in C
// Prints who and where we are: host name, CPU architecture, kernel and process id.
//
// Native build (on the board):   gcc -Wall -O2 hello.c -o hello_c
// Cross build (on an x86 PC):    aarch64-linux-gnu-gcc -Wall -O2 hello.c -o hello_c
// Run:                           ./hello_c

#include <stdio.h>
#include <sys/utsname.h>
#include <unistd.h>

int main(void)
{
    struct utsname u;
    if (0 != uname(&u)) {
        perror("uname");
        return 1;
    }

    printf("Hello from C!\n");
    printf("  host    : %s\n", u.nodename);
    printf("  machine : %s\n", u.machine);   // aarch64 on all three boards, x86_64 on a PC
    printf("  kernel  : %s %s\n", u.sysname, u.release);
    printf("  pid     : %d\n", (int)getpid());
    return 0;
}
