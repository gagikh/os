// Day 7: Users, Ownership and Permissions
// Goal: repeat the kernel's permission decision in your own code, and check that
//       you get the same answer as the kernel's access().
//
//     ./day07 --selftest                       tests on made-up files and users
//     ./day07 /etc/shadow /usr/bin/passwd /tmp
//     -rw-r----- 0640 root   shadow  /etc/shadow      you: ---   access(): ---
//
// Compile:  g++ -std=c++17 -Wall -Wextra -O2 template.cpp -o day07

#include <cstdio>
#include <cstring>
#include <grp.h>
#include <iostream>
#include <pwd.h>
#include <string>
#include <sys/stat.h>
#include <unistd.h>
#include <vector>
#include "../common/sys_check.h"

// What the caller wants to do: a bit mask, like access()'s R_OK | W_OK | X_OK
constexpr int kRead = 4;
constexpr int kWrite = 2;
constexpr int kExec = 1;

// ---------------------------------------------------------------------------
// Helpers: ready to use, read them but do not change them
// ---------------------------------------------------------------------------

// "rwx", "r-x", ... for a mask of kRead | kWrite | kExec
std::string rwx(int mask)
{
    auto s = std::string("---");
    if (0 != (mask & kRead)) s[0] = 'r';
    if (0 != (mask & kWrite)) s[1] = 'w';
    if (0 != (mask & kExec)) s[2] = 'x';
    return s;
}

// A file as the kernel sees it in the inode (Day 3): owner, group, mode bits
struct file_t {
    uid_t owner;
    gid_t group;
    mode_t mode;    // type + permission bits, as in struct stat::st_mode
};

std::string user_name(uid_t uid)
{
    auto const pw = getpwuid(uid);
    return pw ? pw->pw_name : std::to_string(uid);
}

std::string group_name(gid_t gid)
{
    auto const gr = getgrgid(gid);
    return gr ? gr->gr_name : std::to_string(gid);
}

// ---------------------------------------------------------------------------
// Your turn: replace each TODO body
// ---------------------------------------------------------------------------

// TODO 1: the 10-character string of ls -l, e.g. "-rwsr-xr-x", "drwxrwxrwt".
//   - [0]: '-' S_ISREG, 'd' S_ISDIR, 'l' S_ISLNK, 'c' S_ISCHR, 'b' S_ISBLK, 'p' S_ISFIFO, 's' S_ISSOCK
//   - [1..9]: r w x for owner (S_IRUSR S_IWUSR S_IXUSR), group (S_IRGRP ...), other (S_IROTH ...)
//   - special bits replace an x:  S_ISUID -> [3], S_ISGID -> [6], S_ISVTX -> [9]
//       setuid/setgid: 's' if the x bit is set, 'S' if not
//       sticky:        't' if the x bit is set, 'T' if not
std::string mode_string(mode_t mode)
{
    (void)mode;    // remove this line when you use the parameter
    return "??????????";
}

// TODO 2: the permission bits as 4 octal digits, e.g. "0644", "4755", "1777".
//   - only the low 12 bits: mode & 07777
//   - char buf[8]; std::snprintf(buf, sizeof(buf), "%04o", ...)
std::string octal_string(mode_t mode)
{
    (void)mode;    // remove this line when you use the parameter
    return "????";
}

// TODO 3: may a process with this uid and these groups do `want` (kRead | kWrite | kExec)?
//   The kernel's rule (README §3):
//   - uid 0 (root): read and write always allowed; exec allowed if the file is a
//     directory or has at least one x bit (S_IXUSR | S_IXGRP | S_IXOTH)
//   - uid == f.owner      -> use ONLY the owner bits: (f.mode >> 6) & 7
//   - f.group in groups   -> use ONLY the group bits: (f.mode >> 3) & 7
//   - otherwise           -> the other bits:          f.mode & 7
//   - allowed when every bit of `want` is present: want == (want & bits)
bool can_access(const file_t &f, uid_t uid, const std::vector<gid_t> &groups, int want)
{
    (void)f;
    (void)uid;
    (void)groups;
    (void)want;    // remove these lines when you use the parameters
    return false;
}

// TODO 4: may this process delete (unlink) `file` from directory `dir`?
//   - it needs write AND exec on the DIRECTORY (the file's own bits do not matter: Day 3)
//   - if the directory has the sticky bit (S_ISVTX), additionally the uid must be
//     the file's owner, the directory's owner, or 0
bool can_delete(const file_t &dir, const file_t &file, uid_t uid, const std::vector<gid_t> &groups)
{
    (void)dir;
    (void)file;
    (void)uid;
    (void)groups;    // remove these lines when you use the parameters
    return false;
}

// TODO 5: every group of this process: the effective gid plus the supplementary groups.
//   - auto const n = SYS_CHECK(getgroups(0, nullptr));   the count
//   - std::vector<gid_t> list(n); SYS_CHECK(getgroups(n, list.data()));
//   - push_back(getegid()) as well (it is not always in the list)
std::vector<gid_t> my_groups()
{
    return {};
}

// ---------------------------------------------------------------------------
// Tests and main: already complete
// ---------------------------------------------------------------------------
int failures = 0;

void check(bool ok, const char *what)
{
    std::cout << (ok ? "  PASS  " : "  FAIL  ") << what << '\n';
    failures += ok ? 0 : 1;
}

int selftest()
{
    // Users: anna 1001 (groups 1001, 2000 "lab"), aram 1002 (group 1002), root 0
    auto const anna = uid_t{1001};
    auto const aram = uid_t{1002};
    auto const anna_groups = std::vector<gid_t>{1001, 2000};
    auto const aram_groups = std::vector<gid_t>{1002};

    std::cout << "mode_string / octal_string\n";
    check("-rw-r--r--" == mode_string(S_IFREG | 0644), "0644 file  -> -rw-r--r--");
    check("-rwsr-xr-x" == mode_string(S_IFREG | 04755), "4755 file  -> -rwsr-xr-x");
    check("drwxrwxrwt" == mode_string(S_IFDIR | 01777), "1777 dir   -> drwxrwxrwt");
    check("-rwSr--r--" == mode_string(S_IFREG | 04644), "4644 file  -> -rwSr--r-- (S: no x)");
    check("drwxrws---" == mode_string(S_IFDIR | 02770), "2770 dir   -> drwxrws---");
    check("lrwxrwxrwx" == mode_string(S_IFLNK | 0777), "symlink    -> lrwxrwxrwx");
    check("4755" == octal_string(S_IFREG | 04755), "octal 4755");
    check("0644" == octal_string(S_IFREG | 0644), "octal 0644");

    std::cout << "can_access\n";
    auto const report = file_t{anna, 2000, S_IFREG | 0640};     // -rw-r----- anna lab
    check(can_access(report, anna, anna_groups, kRead | kWrite), "owner anna may read+write");
    check(!can_access(report, anna, anna_groups, kExec), "owner anna may not execute");
    check(!can_access(report, aram, aram_groups, kRead), "aram (other) may not read");
    check(can_access(report, aram, {1002, 2000}, kRead), "aram in lab may read");
    check(!can_access(report, aram, {1002, 2000}, kWrite), "aram in lab may not write");
    auto const odd = file_t{anna, 2000, S_IFREG | 0044};        // ----r--r-- anna lab
    check(!can_access(odd, anna, anna_groups, kRead), "----r--r--: the OWNER may not read (only one class is checked)");
    check(can_access(odd, aram, aram_groups, kRead), "----r--r--: others may");
    check(can_access(report, 0, {0}, kRead | kWrite), "root reads and writes anything");
    check(!can_access(report, 0, {0}, kExec), "root may not execute a file with no x bit");
    check(can_access(file_t{anna, 2000, S_IFDIR | 0700}, 0, {0}, kExec), "root may enter any directory");

    std::cout << "can_delete\n";
    auto const shared = file_t{0, 2000, S_IFDIR | 0775};         // drwxrwxr-x root lab
    auto const tmp = file_t{0, 0, S_IFDIR | 01777};              // drwxrwxrwt root root
    auto const annas = file_t{anna, 1001, S_IFREG | 0444};       // read-only file of anna
    check(can_delete(shared, annas, aram, {1002, 2000}), "aram (lab) may delete anna's read-only file in a lab dir");
    check(!can_delete(shared, annas, aram, aram_groups), "aram outside lab may not");
    check(!can_delete(tmp, annas, aram, aram_groups), "/tmp is sticky: aram may not delete anna's file");
    check(can_delete(tmp, annas, anna, anna_groups), "/tmp: anna may delete her own file");
    check(can_delete(tmp, annas, 0, {0}), "/tmp: root may delete anything");

    std::cout << (0 == failures ? "all tests passed\n" : "some tests failed\n");
    return failures;
}

int main(int argc, char *argv[])
{
    if (2 == argc && 0 == std::strcmp(argv[1], "--selftest")) return selftest();
    if (argc < 2) {
        std::cerr << "Usage: day07 --selftest | day07 FILE...\n";
        return 1;
    }

    auto const uid = geteuid();
    auto const groups = my_groups();
    std::cout << "you are " << user_name(uid) << " (uid " << uid << "), in " << groups.size() << " groups\n";
    for (auto i = 1; i < argc; ++i) {
        struct stat st;
        if (-1 == lstat(argv[i], &st)) { std::perror(argv[i]); continue; }
        auto const f = file_t{st.st_uid, st.st_gid, st.st_mode};
        auto mine = 0, kernel = 0;
        for (auto bit : {kRead, kWrite, kExec}) {
            if (can_access(f, uid, groups, bit)) mine |= bit;
            auto const mode = (kRead == bit) ? R_OK : (kWrite == bit) ? W_OK : X_OK;
            if (0 == access(argv[i], mode)) kernel |= bit;
        }
        std::printf("%s %s %-8s %-8s %-20s you: %s   access(): %s%s\n", mode_string(st.st_mode).c_str(),
                    octal_string(st.st_mode).c_str(), user_name(st.st_uid).c_str(),
                    group_name(st.st_gid).c_str(), argv[i], rwx(mine).c_str(), rwx(kernel).c_str(),
                    mine == kernel ? "" : "   <-- differs");
    }
    return 0;
}
