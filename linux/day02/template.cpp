// Day 2: The Shell and the Program's Environment
// Goal: a calculator whose operands may be environment variables, with GNU-style
//       options, errors on stderr and meaningful exit codes.
//
//     export x=10 y=20
//     ./day02 x+y              -> 30
//     ./day02 -v 'x*y'         -> stderr: x = 10, y = 20     stdout: 200
//     z=7 ./day02 z/2          -> 3
//     ./day02 x/0 ; echo $?    -> stderr: division by zero   status: 3
//     ./day02 --list           -> every environment variable whose value is an integer
//
// Compile:  g++ -std=c++17 -Wall -Wextra -O2 template.cpp -o day02
// Quote expressions with * in them: './day02 x*y' would be expanded by the shell.

#include <cstdio>
#include <cstdlib>
#include <getopt.h>
#include <iostream>
#include <optional>
#include <string>
#include <unistd.h>    // extern char **environ

// ---------------------------------------------------------------------------
// Exit codes: the program's "answer" to the shell, read with echo $?
// ---------------------------------------------------------------------------
constexpr int kExitOk = 0;          // result printed
constexpr int kExitUsage = 1;       // bad options or no expression
constexpr int kExitVariable = 2;    // variable not set, or not a number
constexpr int kExitMath = 3;        // division by zero, unknown operator

// ---------------------------------------------------------------------------
// Helpers: ready to use, read them but do not change them
// ---------------------------------------------------------------------------

void print_usage(FILE *stream)
{
    std::fprintf(stream,
                 "Usage: day02 [OPTIONS] EXPRESSION\n"
                 "EXPRESSION is A<op>B, op is one of + - * /,\n"
                 "A and B are integers or names of environment variables.\n"
                 "  -h, --help      show this help and exit\n"
                 "  -v, --verbose   print the value of each variable to stderr\n"
                 "  -l, --list      list environment variables with integer values\n");
}

// true for "42", "-7"; false for "", "4x", "abc"
bool is_integer(const std::string &s)
{
    if (s.empty()) return false;
    auto pos = std::size_t{('-' == s[0] || '+' == s[0]) ? 1u : 0u};
    if (s.size() == pos) return false;
    for (; pos < s.size(); ++pos)
        if (s[pos] < '0' || s[pos] > '9') return false;
    return true;
}

struct options_t {
    bool verbose = false;
    bool list = false;
    std::string expression;
};

struct expression_t {
    std::string left;
    char op;
    std::string right;
};

// ---------------------------------------------------------------------------
// Worked example: read it before you start
// ---------------------------------------------------------------------------

// "x+y" -> {"x", '+', "y"}. The operator search starts at index 1, so "-5+x"
// keeps its minus sign as part of the left operand. Try ./day02 -5+x: getopt_long
// takes "-5" for an option. Run it as ./day02 -- -5+x  ("--" = end of options).
std::optional<expression_t> split_expression(const std::string &text)
{
    auto const pos = text.find_first_of("+-*/", 1);
    if (std::string::npos == pos || text.size() - 1 == pos) return std::nullopt;
    return expression_t{text.substr(0, pos), text[pos], text.substr(pos + 1)};
}

// ---------------------------------------------------------------------------
// Your turn: replace each TODO body
// ---------------------------------------------------------------------------

// TODO 1: parse the options with getopt_long.          man 3 getopt_long
//   - short options "hvl", long options "help", "verbose", "list" (all no_argument)
//   - -h: print_usage(stdout) and exit(kExitOk)
//   - unknown option ('?'): print_usage(stderr) and exit(kExitUsage)
//   - after the loop, unless --list was given, exactly one argument must remain:
//     argv[optind] is the expression. Otherwise print_usage(stderr), exit(kExitUsage).
options_t parse_options(int argc, char *argv[])
{
    (void)argc;
    (void)argv;    // remove these two lines when you use the parameters
    auto options = options_t{};
    return options;
}

// TODO 2: the value of one operand.
//   - an integer literal ("42") is returned as is: std::stol
//   - otherwise it is a variable name: getenv(token.c_str())
//       nullptr          -> stderr "<name> is not set",            exit(kExitVariable)
//       not an integer   -> stderr "<name>=<value> is not a number", exit(kExitVariable)
//   - if verbose, print "<name> = <value>" to stderr (not stdout: why?)
long value_of(const std::string &token, bool verbose)
{
    (void)token;
    (void)verbose;    // remove these two lines when you use the parameters
    return 0;
}

// TODO 3: apply the operator.
//   - switch on op: '+', '-', '*', '/'
//   - division by zero -> stderr "division by zero", exit(kExitMath)
long compute(long left, char op, long right)
{
    (void)left;
    (void)op;
    (void)right;    // remove these three lines when you use the parameters
    return 0;
}

// TODO 4: print every environment variable whose value is an integer, one per line:
//     x=10
//   - loop over environ until nullptr: for (auto var = environ; nullptr != *var; ++var)
//   - each *var is "NAME=value": split at the FIRST '='
//   - print only those where is_integer(value)
void list_numeric_environment()
{
    std::cout << "TODO\n";
}

// ---------------------------------------------------------------------------
// main: already complete, it calls your functions
// ---------------------------------------------------------------------------
int main(int argc, char *argv[])
{
    auto const options = parse_options(argc, argv);

    if (options.list) {
        list_numeric_environment();
        return kExitOk;
    }

    auto const expression = split_expression(options.expression);
    if (!expression) {
        std::cerr << "cannot parse expression '" << options.expression << "'\n";
        print_usage(stderr);
        return kExitUsage;
    }

    auto const left = value_of(expression->left, options.verbose);
    auto const right = value_of(expression->right, options.verbose);
    std::cout << compute(left, expression->op, right) << '\n';
    return kExitOk;
}
