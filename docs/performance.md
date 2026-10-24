# Performance notes

## Build profiles

Default local build:

```sh
./build.sh
```

Portable CPU baseline:

```sh
GCU_NATIVE=0 ./build.sh
```

Compare optimization levels:

```sh
GCU_OPT=-O2 ./build.sh
GCU_OPT=-O3 ./build.sh
```

Optional LTO, if the local linker supports it:

```sh
GCU_LTO=1 ./build.sh
```

## Benchmarking

Measure more than one run and compare medians. Keep the input, output destination, filesystem, and cache conditions consistent. For example:

```sh
/usr/bin/time -f 'elapsed=%e user=%U sys=%S maxrss=%MKB' \
  ./build/bin/cop large-input.bin /tmp/gcu-copy.bin
```

Do not compare a cold-cache run with a warm-cache run and conclude that a compiler flag caused the difference. Also test correctness after every optimization.

## Current limitation

The optimization flags only affect code compiled from GCU's C source. Commands forwarded by `src/compat.c` execute host utilities, so their performance is primarily the host utility's implementation and build configuration.
