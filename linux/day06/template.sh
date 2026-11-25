#!/usr/bin/env bash
# Day 6: Bash Scripting
# Goal: tidy a directory by moving each file into a subdirectory by type:
#       images/ documents/ code/ archives/ other/
#
#     ./template.sh --selftest         build a test directory and check your functions
#     ./template.sh -n ~/Downloads     dry run: only print what would move
#     ./template.sh -v ~/Downloads     move, and say what is happening (on stderr)
#
# Exit codes: 0 ok, 1 usage error, 2 not a directory.
# When you are done:  shellcheck template.sh

set -uo pipefail

readonly EXIT_OK=0
readonly EXIT_USAGE=1
readonly EXIT_NODIR=2

DRY_RUN=0        # -n
VERBOSE=0        # -v
TARGET=""        # the directory to organize
declare -A COUNTS=()   # category -> number of files moved
SELFTEST_DIR=""         # used only by --selftest

# ---------------------------------------------------------------------------
# Helpers: ready to use, read them but do not change them
# ---------------------------------------------------------------------------

usage() {
    echo "Usage: $0 [-n] [-v] [-h] DIR"
    echo "  -n  dry run: print what would be moved, move nothing"
    echo "  -v  verbose: print every move to stderr"
    echo "  -h  this help"
    echo "  $0 --selftest   run the built-in tests"
}

# Messages go to stderr, so that stdout stays clean for the report (Day 2).
log() {
    if (( VERBOSE )); then echo "$*" >&2; fi
}

# ---------------------------------------------------------------------------
# Your turn: replace each TODO body
# ---------------------------------------------------------------------------

# TODO 1: print the category of one file name: images, documents, code, archives or other.
#   - the extension is ${name##*.}; lowercase it with ${ext,,}  (so photo.JPG is an image)
#   - a name without a dot has no extension: [[ $name == *.* ]] || { echo other; return; }
#   - use case:  jpg|jpeg|png|gif|bmp -> images
#                txt|md|pdf|doc|docx|odt -> documents
#                c|cpp|h|hpp|py|sh -> code
#                tar|gz|tgz|xz|zst|zip|7z -> archives
#                anything else -> other
category_of() {
    local name=$1
    echo "TODO"
}

# TODO 2: read the options with getopts "nvh", then exactly one directory.
#   - -n: DRY_RUN=1   -v: VERBOSE=1   -h: usage; exit "$EXIT_OK"
#   - unknown option: usage >&2; exit "$EXIT_USAGE"
#   - shift $(( OPTIND - 1 )); then if $# is not 1: usage >&2; exit "$EXIT_USAGE"
#   - TARGET=$1; if it is not a directory: echo "$TARGET: not a directory" >&2; exit "$EXIT_NODIR"
parse_args() {
    TARGET=${1:-}
}

# TODO 3: move every regular file directly inside "$TARGET" into "$TARGET/<category>/".
#   - shopt -s nullglob;  for path in "$TARGET"/*; do ... done
#     (the * does not match hidden files: .bashrc stays where it is)
#   - skip anything that is not a regular file: [[ -f $path ]] || continue
#   - name=${path##*/}; category=$(category_of "$name")
#   - dry run: echo "would move $name -> $category/"   (stdout)
#     otherwise: mkdir -p "$TARGET/$category" && mv -n -- "$path" "$TARGET/$category/"
#                and log "moved $name -> $category/"
#     (-n: never overwrite an existing file; --: names that start with - are safe)
#   - count it: COUNTS[$category]=$(( ${COUNTS[$category]:-0} + 1 ))
#   - ALWAYS quote "$path" and "$name": the test directory has a file with a space in its name
organize() {
    :
}

# TODO 4: print a summary, one line per category, sorted by name, then the total:
#       archives       1
#       code           2
#       total          3
#   - loop over the keys: for category in "${!COUNTS[@]}"; ...  printf '%-12s %3d\n' ...
#   - pipe the loop into sort; then print the total
#   - in dry-run mode start with the line "(dry run)"
report() {
    echo "TODO"
}

# ---------------------------------------------------------------------------
# Self test: already complete. Read it to see what each TODO must do.
# ---------------------------------------------------------------------------
selftest() {
    local failures=0
    SELFTEST_DIR=$(mktemp -d)
    trap 'rm -rf -- "$SELFTEST_DIR"' EXIT   # clean up when the script ends, whatever happens
    local tmp=$SELFTEST_DIR

    check() {   # check "description" command...
        local what=$1; shift
        if "$@"; then echo "  PASS  $what"; else echo "  FAIL  $what"; failures=$(( failures + 1 )); fi
    }

    echo "category_of"
    check "photo.JPG is an image"           test "$(category_of photo.JPG)" = images
    check "notes.txt is a document"         test "$(category_of notes.txt)" = documents
    check "build.sh is code"                test "$(category_of build.sh)" = code
    check "backup.tar.gz is an archive"     test "$(category_of backup.tar.gz)" = archives
    check "Makefile (no dot) is other"      test "$(category_of Makefile)" = other

    echo "parse_args (run in a subshell, so its exit does not end the test)"
    ( parse_args ) >/dev/null 2>&1;                 check "no argument -> exit 1"      test $? -eq "$EXIT_USAGE"
    ( parse_args /no/such/dir ) >/dev/null 2>&1;    check "missing dir -> exit 2"      test $? -eq "$EXIT_NODIR"
    ( parse_args -x "$tmp" ) >/dev/null 2>&1;       check "unknown option -> exit 1"   test $? -eq "$EXIT_USAGE"
    ( parse_args -n "$tmp"; echo "$DRY_RUN $TARGET" ) 2>/dev/null | grep -q "^1 $tmp\$"
    check "-n DIR sets DRY_RUN and TARGET" test "${PIPESTATUS[1]}" -eq 0

    local d="$tmp/downloads"
    mkdir -p "$d/subdir"
    touch "$d/photo.JPG" "$d/my photo.png" "$d/notes.txt" "$d/build.sh" "$d/backup.tar.gz" "$d/Makefile" "$d/.bashrc"

    echo "organize (dry run)"
    local out
    out=$( (parse_args -n "$d" && organize && report) 2>/dev/null )
    check "dry run moves nothing"           test -f "$d/photo.JPG"
    check "dry run says what it would do"   grep -q "would move photo.JPG -> images/" <<< "$out"

    echo "organize"
    out=$( (parse_args "$d" && organize && report) 2>/dev/null )
    check "photo.JPG -> images/"            test -f "$d/images/photo.JPG"
    check "'my photo.png' (with a space)"   test -f "$d/images/my photo.png"
    check "notes.txt -> documents/"         test -f "$d/documents/notes.txt"
    check "build.sh -> code/"               test -f "$d/code/build.sh"
    check "backup.tar.gz -> archives/"      test -f "$d/archives/backup.tar.gz"
    check "Makefile -> other/"              test -f "$d/other/Makefile"
    check ".bashrc (hidden) stays"          test -f "$d/.bashrc"
    check "subdir/ is not touched"          test -d "$d/subdir"

    echo "report"
    check "images counted twice"            grep -Eq '^images +2$' <<< "$out"
    check "total is 6"                      grep -Eq '^total +6$' <<< "$out"

    echo "an existing file is never overwritten"
    echo new > "$d/notes.txt"
    ( parse_args "$d" && organize ) >/dev/null 2>&1
    check "documents/notes.txt kept"        test ! -s "$d/documents/notes.txt"

    if (( failures == 0 )); then echo "all tests passed"; else echo "$failures test(s) failed"; fi
    return "$failures"
}

# ---------------------------------------------------------------------------
# main
# ---------------------------------------------------------------------------
if [[ ${1:-} == --selftest ]]; then
    selftest
    exit $?
fi
parse_args "$@"
organize
report
exit "$EXIT_OK"
