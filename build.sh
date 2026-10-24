#!/bin/sh
set -eu

ROOT=$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)
BUILD_DIR=${GCU_BUILD_DIR:-"$ROOT/build"}
CC=${CC:-clang}
OPT=${GCU_OPT:--O3}

mkdir -p "$BUILD_DIR/bin"

CFLAGS="-std=c17 $OPT -fno-plt -Wall -Wextra -Wpedantic -Wconversion -Wshadow -D_FORTIFY_SOURCE=3"
LDFLAGS=""
if [ "${GCU_LTO:-0}" = 1 ]; then
    CFLAGS="$CFLAGS -flto"
    LDFLAGS="-flto"
fi
if [ "${GCU_NATIVE:-1}" = 1 ]; then
    CFLAGS="$CFLAGS -march=native"
fi

# Word-split flags intentionally so users can override CC/CFLAGS conventionally.
# shellcheck disable=SC2086
$CC $CFLAGS -I"$ROOT/include" \
    "$ROOT/src/main.c" "$ROOT/src/files.c" "$ROOT/src/text.c" \
    "$ROOT/src/system.c" "$ROOT/src/compat.c" \
    $LDFLAGS -o "$BUILD_DIR/gcu"

while IFS= read -r command || [ -n "$command" ]; do
    case "$command" in ''|\#*) continue ;; esac
    ln -sf gcu "$BUILD_DIR/bin/$command"
done < "$ROOT/commands.txt"

ln -sf ../gcu "$BUILD_DIR/bin/gcu"

printf 'Built GCU %s with %s (%s)\n' "$(cat "$ROOT/VERSION.txt")" "$CC" "$OPT"
printf 'Commands: %s\n' "$(wc -l < "$ROOT/commands.txt" | tr -d ' ')"
printf 'Binary: %s\n' "$BUILD_DIR/gcu"
