# GCU architecture

GCU is a multi-call executable. `build.sh` compiles `src/main.c` and the implementation modules into `build/gcu`, then creates one symbolic link per entry in `commands.txt` under `build/bin/`.

At runtime, the executable reads its basename and dispatches to the matching implementation. The process arguments are preserved as an argument vector; GCU does not build shell command strings for dispatch.

## Modules

- `main.c`: command name discovery, version/help, ordered dispatch.
- `files.c`: filesystem and path operations; uses file descriptors and `fstatat` where appropriate.
- `text.c`: stream-oriented operations and text helpers.
- `system.c`: environment and system identity operations.
- `compat.c`: documented host-command mappings for commands not yet native.

## Performance choices

- C17 and libc/POSIX interfaces keep the core direct and predictable.
- File-copy loops use a 256 KiB buffer to reduce read/write syscall frequency on large files.
- The default build uses `-O3`; `-march=native` is enabled for local performance builds.
- LTO is opt-in because Clang's LTO plugin is not installed in every environment.
- Performance should be evaluated with repeatable workloads. For filesystem commands, cache state, storage speed, file size, and syscall behavior can matter more than instruction count.

## Porting policy

Native replacements should preserve command name, documented options, output format, exit status, error behavior, and handling of unusual filenames. Add tests before replacing a compatibility mapping. Do not claim GNU compatibility unless the behavior is tested.
