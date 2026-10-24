#!/bin/sh
set -eu
ROOT=$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)
"$ROOT/build.sh"
count=$(wc -l < "$ROOT/commands.txt" | tr -d ' ')
[ "$count" -eq 110 ] || { echo "expected 110 command names, found $count" >&2; exit 1; }
if grep -n '[!?]' "$ROOT/commands.txt"; then
    echo 'command names must not contain ! or ?' >&2
    exit 1
fi
while IFS= read -r command; do
    [ -x "$ROOT/build/bin/$command" ] || { echo "missing executable link: $command" >&2; exit 1; }
done < "$ROOT/commands.txt"
printf 'GCU command inventory passed (%s commands)\n' "$count"
