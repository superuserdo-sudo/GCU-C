# GCU — Gill Core Utilities

**Version:** 0.4.0  
**Language:** C17  
**Compiler:** Clang (default)  
**Target libc:** glibc  
**Platform:** Linux

GCU is a multi-call command suite: one compiled program is exposed under each command name through symbolic links. The command name is taken from `argv[0]`, so no shell is used to dispatch commands.

## Build and activate

```sh
./build.sh
source ./env.sh
```

That is the intended short workflow. `source ./env.sh` adds `build/bin` to the current shell's `PATH`; it does not install anything system-wide.

Run a command:

```sh
see .
cowf README.md
mdir -p demo/subdir
punch demo/subdir/example.txt
type "Hello from GCU"
wai
```

Use `command --version` to see the GCU version. Use `command --help` for the common help summary.

## Performance build

The default build uses `-O3 -march=native -fno-plt` and compiler warnings. `-march=native` tunes the executable for the machine that builds it; do not distribute that binary as a portable x86-64 build to unknown CPUs.

Portable build:

```sh
GCU_NATIVE=0 ./build.sh
```

Choose a different compiler or optimization level:

```sh
CC=clang GCU_OPT=-O2 ./build.sh
```

Link-time optimization is optional because some Clang installations do not have a working LTO linker plugin:

```sh
GCU_LTO=1 ./build.sh
```

The build intentionally does not use `-Ofast`: it can change language semantics and is not a general-purpose speed switch for filesystem tools.

## Architecture

- `src/main.c` — entry point and command dispatch.
- `src/files.c` — native filesystem operations and the buffered file-copy path.
- `src/text.c` — native text, stream, and status commands.
- `src/system.c` — native environment and system-information commands.
- `src/compat.c` — explicit compatibility mappings for commands that currently forward to host utilities.
- `include/gcu.h` — shared API and version definitions.
- `commands.txt` — complete 110-command name list.
- `profiles/minimal.txt` — suggested minimal command profile.
- `docs/` — command reference, architecture, performance notes, and implementation status.
- `tests/` — smoke and inventory checks.

## Implementation status — read this

The full command inventory is present, but **not all 110 commands have been reimplemented natively in C yet**. Core operations such as listing, copying, moving, creating/removing paths, reading streams, line counts, environment inspection, and basic system information are implemented in GCU's C source. More specialized commands currently use the explicit mappings in `src/compat.c` and require the corresponding host utility in `PATH`.

This is deliberate and visible rather than silently pretending those utilities are native implementations. The next development step is to replace compatibility mappings one group at a time, with tests that compare output, exit status, and edge cases.

## Safety notes

- `rom` removes directory entries; review paths before running it.
- `jroot`, `ownf`, `grpown`, `nodef`, `ctxset`, and `ctxrun` can require privileges or affect system state.
- `wipf` forwards to the host `shred` utility; overwrite operations are not a guarantee of secure erasure on SSDs, copy-on-write filesystems, or storage with remapping.
- `pmad` cannot modify the parent shell from an ordinary executable. Load shell integration in the current shell when needed.

## License

GGill General Source Code and Software Property License (GSPL)

Version 1.1

Copyright (c) 2026 Rayan Abdelli

All rights reserved, subject to the permissions expressly granted by this License.

