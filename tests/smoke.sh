#!/bin/sh
set -eu
ROOT=$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)
"$ROOT/build.sh"
BIN="$ROOT/build/bin"
TMP=$(mktemp -d)
trap 'rm -rf "$TMP"' EXIT HUP INT TERM

printf 'alpha\nbeta\n' > "$TMP/input"
"$BIN/cowf" "$TMP/input" > "$TMP/output"
cmp "$TMP/input" "$TMP/output"
[ "$("$BIN/wai")" = "$(pwd)" ]
"$BIN/mdir" "$TMP/dir"
"$BIN/punch" "$TMP/dir/empty"
"$BIN/cop" "$TMP/input" "$TMP/dir/copy"
cmp "$TMP/input" "$TMP/dir/copy"
[ "$("$BIN/wcnt" -l "$TMP/input" | awk '{print $1}')" = 2 ]
"$BIN/rom" "$TMP/dir/empty" "$TMP/dir/copy"
"$BIN/rdem" "$TMP/dir"
"$BIN/false" && exit 1 || [ "$?" -eq 1 ]
"$BIN/true"
printf 'GCU smoke tests passed\n'
