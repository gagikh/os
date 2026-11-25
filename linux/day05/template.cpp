// Day 5: Archiving and Compression
// Goal: write a real ustar (.tar) archive that GNU tar can read, and read
//       archives that GNU tar wrote.
//
//     ./day05 -c out.tar a.txt numbers.txt   create
//     ./day05 -t out.tar                     list:  mode size name
//     tar -tvf out.tar                       the real tar must accept your archive
//
// Compile:  g++ -std=c++17 -Wall -Wextra -O2 template.cpp -o day05
// Format:   https://www.gnu.org/software/tar/manual/html_node/Standard.html

#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <grp.h>
#include <iostream>
#include <pwd.h>
#include <string>
#include <sys/stat.h>
#include <vector>
#include "../common/sys_check.h"

constexpr std::size_t kBlock = 512;

// The POSIX ustar header. Every field is TEXT: names, or octal numbers ending in '\0'.
struct ustar_header {
    char name[100];
    char mode[8];
    char uid[8];
    char gid[8];
    char size[12];
    char mtime[12];
    char chksum[8];
    char typeflag;
    char linkname[100];
    char magic[6];
    char version[2];
    char uname[32];
    char gname[32];
    char devmajor[8];
    char devminor[8];
    char prefix[155];
    char pad[12];
};
static_assert(kBlock == sizeof(ustar_header), "a header is exactly one block");

// ---------------------------------------------------------------------------
// Helpers: ready to use, read them but do not change them
// ---------------------------------------------------------------------------

// Copy a string into a fixed-size text field (truncated if too long, rest stays '\0').
void put_text(char *field, std::size_t width, const std::string &value)
{
    std::memcpy(field, value.data(), std::min(width, value.size()));
}

std::string read_whole_file(const std::string &path)
{
    auto in = std::ifstream(path, std::ios::binary);
    return std::string(std::istreambuf_iterator<char>(in), {});
}

// Zero bytes needed after `size` bytes of data to reach the next block boundary.
std::size_t padding(std::size_t size)
{
    return (kBlock - size % kBlock) % kBlock;
}

// ---------------------------------------------------------------------------
// Your turn: replace each TODO body
// ---------------------------------------------------------------------------

// TODO 1: write `value` into a numeric field of `width` bytes:
//   width - 1 octal digits, zero-padded, then '\0'.   e.g. width 8, value 420 -> "0000644\0"
//   - fill from the right: field[width - 1] = '\0'; then for each position i from
//     width - 2 down to 0:  field[i] = '0' + (value & 7);  value >>= 3;
//     (one octal digit is exactly 3 bits)
void put_octal(char *field, std::size_t width, unsigned long value)
{
    (void)field;
    (void)width;
    (void)value;    // remove these lines when you use the parameters
}

// TODO 2: fill a header for a regular file from its stat() data.
//   - zero the whole header first: auto h = ustar_header{};
//   - name; mode = st.st_mode & 07777; uid; gid; size; mtime (all octal, put_octal)
//   - typeflag '0'; magic "ustar" (put_text: the '\0' comes for free); version "00"
//     (version is NOT '\0'-terminated: copy the 2 bytes with std::memcpy)
//   - uname / gname from getpwuid(st.st_uid) / getgrgid(st.st_gid), if they exist
//   - leave chksum for TODO 3
ustar_header make_header(const std::string &name, const struct stat &st)
{
    (void)name;
    (void)st;    // remove these lines when you use the parameters
    return ustar_header{};
}

// TODO 3: compute and store the checksum.
//   - fill h.chksum with 8 spaces, then add up all 512 bytes as UNSIGNED values
//     (reinterpret_cast<const unsigned char *>(&h))
//   - store it as 6 octal digits, '\0', ' ':  put_octal(h.chksum, 7, sum); h.chksum[7] = ' ';
void set_checksum(ustar_header &h)
{
    (void)h;    // remove this line when you use the parameter
}

// TODO 4: append one file to the archive: header, data, zero padding.
//   - SYS_CHECK(stat(path.c_str(), &st)); skip (with a message on stderr) anything
//     that is not a regular file (S_ISREG) or whose name is 100 characters or longer
//   - make_header, set_checksum, write the 512 header bytes
//   - write the file's bytes (read_whole_file), then padding(size) zero bytes
void add_file(std::ofstream &out, const std::string &path)
{
    (void)out;
    (void)path;    // remove these lines when you use the parameters
}

// TODO 5: list an archive: for every header print  "<mode> <size> <name>"
//   - read 512 bytes at a time; an all-zero name (name[0] == '\0') means the end
//   - mode and size are octal text: std::strtoul(h.size, nullptr, 8)
//   - skip the data: size + padding(size) bytes  (in.seekg(n, std::ios::cur))
//   - check the magic: if it does not start with "ustar", print an error and stop
void list_archive(const std::string &archive)
{
    (void)archive;    // remove this line when you use the parameter
}

// ---------------------------------------------------------------------------
// main: already complete, it calls your functions
// ---------------------------------------------------------------------------
int main(int argc, char *argv[])
{
    if (3 <= argc && 0 == std::strcmp(argv[1], "-c")) {
        auto out = std::ofstream(argv[2], std::ios::binary);
        if (!out) { std::perror(argv[2]); return 2; }
        for (auto i = 3; i < argc; ++i) add_file(out, argv[i]);
        auto const zeros = std::vector<char>(2 * kBlock, '\0');   // end of archive: two zero blocks
        out.write(zeros.data(), static_cast<std::streamsize>(zeros.size()));
        return 0;
    }
    if (3 == argc && 0 == std::strcmp(argv[1], "-t")) {
        list_archive(argv[2]);
        return 0;
    }
    std::cerr << "Usage: day05 -c ARCHIVE FILE...  |  day05 -t ARCHIVE\n";
    return 2;
}
