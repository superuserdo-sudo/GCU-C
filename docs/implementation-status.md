# Implementation status

GCU currently provides all 110 command names, but native implementation coverage is still partial.

## Native C implementations

`alodir`, `cop`, `cpath`, `dclr`, `dir`, `envshow`, `envx`, `fend`, `fstat`, `fsync`, `ftop`, `hlnk`, `huh`, `lnkf`, `mdir`, `mkq`, `mos`, `movef`, `no`, `pchk`, `pmad` (explanatory status), `punch`, `rdem`, `rlnk`, `rom`, `see`, `true`, `false`, `type`, `ulnk`, `wai`, `wami`, `wcnt`, `wimarch`, `yes`, and `cowf`.

## Host utility compatibility mappings

Specialized text processing, checksums, encodings, permissions/ownership, disk usage, process control, and several system information commands are forwarded to host tools through `src/compat.c`.

## Porting checklist for each native replacement

1. Preserve the command name and documented options.
2. Keep paths as separate arguments; never build shell command strings.
3. Check short reads/writes, `EINTR`, close errors, and output errors.
4. Use predictable exit statuses and report errors to stderr.
5. Add smoke tests plus edge cases (empty input, large files, unusual filenames, permission failures).
6. Compare output and exit status against the reference behavior.
7. Benchmark only after correctness tests pass.
