#define _POSIX_C_SOURCE 200809L
#include "gcu.h"

#include <stdio.h>
#include <string.h>

const char *gcu_basename(const char *path)
{
    const char *slash = strrchr(path, '/');
    return slash ? slash + 1 : path;
}

void gcu_error(const char *name, const char *message)
{
    fprintf(stderr, "%s: %s\n", name, message);
}

void gcu_perror(const char *name, const char *path)
{
    fprintf(stderr, "%s: %s: ", name, path);
    perror(NULL);
}

int gcu_usage(const char *name, const char *usage)
{
    fprintf(stderr, "Usage: %s %s\n", name, usage);
    return 2;
}

static int show_help(const char *name)
{
    printf("%s %s — Gill Core Utilities\n", name, GCU_VERSION);
    printf("Usage: %s [OPTIONS] [ARGUMENTS...]\n", name);
    puts("Use --help for this summary and --version to show the version.");
    return 0;
}

int main(int argc, char **argv)
{
    const char *name = (argc > 0 && argv[0]) ? gcu_basename(argv[0]) : "gcu";

    if (argc > 1 && (!strcmp(argv[1], "--version") || !strcmp(argv[1], "-V"))) {
        printf("%s %s (C17, glibc target)\n", name, GCU_VERSION);
        return 0;
    }
    if (argc > 1 && !strcmp(argv[1], "--help"))
        return show_help(name);

    int result = gcu_run_files(name, argc, argv);
    if (result != -2)
        return result;

    result = gcu_run_text(name, argc, argv);
    if (result != -2)
        return result;

    result = gcu_run_system(name, argc, argv);
    if (result != -2)
        return result;

    return gcu_run_compat(name, argc, argv);
}
