// Day 4: Pipes, Filters and Regular Expressions
// Goal: a small grep. It reads files or stdin, so it works inside pipelines,
//       uses POSIX regular expressions, and returns grep's exit codes.
//
//     ./day04 root /etc/passwd
//     ./day04 -n -i ROOT /etc/passwd
//     ./day04 -E -c '^(root|daemon):' /etc/passwd
//     cat /etc/passwd | ./day04 -v nologin
//     ./day04 bash /etc/passwd /etc/shells      (several files: "file:line")
//     diff <(./day04 -n root /etc/passwd) <(grep -n root /etc/passwd) && echo same
//
// Compile:  g++ -std=c++17 -Wall -Wextra -O2 template.cpp -o day04

#include <fstream>
#include <getopt.h>
#include <iostream>
#include <regex.h>
#include <string>
#include <vector>

// grep's exit codes (GNU grep manual, "Exit Status")
constexpr int kExitMatch = 0;       // at least one line selected
constexpr int kExitNoMatch = 1;     // no line selected
constexpr int kExitError = 2;       // an error: bad regex, missing file

// ---------------------------------------------------------------------------
// Helpers: ready to use, read them but do not change them
// ---------------------------------------------------------------------------

struct options_t {
    bool ignore_case = false;   // -i
    bool invert = false;        // -v
    bool line_numbers = false;  // -n
    bool count_only = false;    // -c
    bool extended = false;      // -E
    std::string pattern;
    std::vector<std::string> files;   // empty -> read stdin
};

void print_usage(std::ostream &out)
{
    out << "Usage: day04 [-i] [-v] [-n] [-c] [-E] PATTERN [FILE...]\n"
           "  -i  ignore case       -v  select non-matching lines\n"
           "  -n  print line numbers -c  print only a count of selected lines\n"
           "  -E  extended regular expressions (ERE)\n"
           "With no FILE, or when FILE is -, read standard input.\n";
}

// ---------------------------------------------------------------------------
// Your turn: replace each TODO body
// ---------------------------------------------------------------------------

// TODO 1: parse "ivncE" with getopt (short options are enough here).
//   - unknown option: print_usage(std::cerr), exit(kExitError)
//   - after the options: argv[optind] is the pattern (missing -> usage, exit(kExitError)),
//     argv[optind + 1 ...] are the files
options_t parse_options(int argc, char *argv[])
{
    (void)argc;
    (void)argv;    // remove these lines when you use the parameters
    return options_t{};
}

// TODO 2: compile the pattern.                                  man 3 regcomp
//   - flags: REG_NOSUB always (we only ask "does it match?"),
//            | REG_EXTENDED for -E, | REG_ICASE for -i
//   - on failure: regerror() into a char buffer, print "day04: <message>" to stderr,
//     exit(kExitError)
void compile(regex_t &re, const options_t &options)
{
    (void)re;
    (void)options;    // remove these lines when you use the parameters
}

// TODO 3: scan one stream. Returns the number of SELECTED lines.
//   - read with while (std::getline(in, line))  (never while (!in.eof()), Day 3)
//   - a line matches when 0 == regexec(&re, line.c_str(), 0, nullptr, 0)
//   - it is selected when it matches XOR -v
//   - unless -c: print selected lines as  [name:][number:]line
//       name   only when show_name is true (more than one file)
//       number only with -n, counting from 1
long scan(std::istream &in, const std::string &name, bool show_name, const regex_t &re,
          const options_t &options)
{
    (void)in;
    (void)name;
    (void)show_name;
    (void)re;
    (void)options;    // remove these lines when you use the parameters
    return 0;
}

// TODO 4: run over every file (or stdin) and return grep's exit code.
//   - no files, or the name "-": scan(std::cin, "(standard input)", ...)
//   - a file that cannot be opened: stderr "day04: <name>: No such file or directory",
//     remember the error, continue with the next file
//   - with -c, print the count per file: [name:]count
//   - result: kExitError if any error happened, else kExitMatch if anything
//     was selected, else kExitNoMatch
int run(const regex_t &re, const options_t &options)
{
    (void)re;
    (void)options;    // remove these lines when you use the parameters
    return kExitNoMatch;
}

// ---------------------------------------------------------------------------
// main: already complete, it calls your functions
// ---------------------------------------------------------------------------
int main(int argc, char *argv[])
{
    auto const options = parse_options(argc, argv);
    auto re = regex_t{};
    compile(re, options);
    auto const status = run(re, options);
    regfree(&re);
    return status;
}
