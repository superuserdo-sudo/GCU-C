#define _POSIX_C_SOURCE 200809L
#include "gcu.h"

#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static FILE *open_input(const char *name, const char *path)
{
    if (!path || !strcmp(path, "-")) return stdin;
    FILE *stream = fopen(path, "rb");
    if (!stream) gcu_perror(name, path);
    return stream;
}

static int close_input(const char *name, FILE *stream)
{
    if (stream == stdin) return 0;
    if (fclose(stream) == EOF) { gcu_perror(name, "close"); return 1; }
    return 0;
}

static int cmd_type(const char *name, int argc, char **argv)
{
    int newline = 1;
    int index = 1;
    if (index < argc && !strcmp(argv[index], "-n")) { newline = 0; ++index; }
    for (int i = index; i < argc; ++i) {
        if (i > index) putchar(' ');
        fputs(argv[i], stdout);
    }
    if (newline) putchar('\n');
    if (ferror(stdout)) { gcu_perror(name, "stdout"); return 1; }
    return 0;
}

static int cmd_cowf(const char *name, int argc, char **argv)
{
    int first = 1;
    if (first < argc && !strcmp(argv[first], "--")) ++first;
    if (first == argc) {
        char buffer[128 * 1024];
        size_t n;
        while ((n = fread(buffer, 1, sizeof buffer, stdin)) > 0)
            if (fwrite(buffer, 1, n, stdout) != n) { gcu_perror(name, "stdout"); return 1; }
        return ferror(stdin) ? 1 : 0;
    }

    int status = 0;
    char buffer[128 * 1024];
    for (int i = first; i < argc; ++i) {
        FILE *input = open_input(name, argv[i]);
        if (!input) { status = 1; continue; }
        size_t n;
        while ((n = fread(buffer, 1, sizeof buffer, input)) > 0) {
            if (fwrite(buffer, 1, n, stdout) != n) { gcu_perror(name, "stdout"); status = 1; break; }
        }
        if (ferror(input)) { gcu_perror(name, argv[i]); status = 1; }
        if (close_input(name, input)) status = 1;
    }
    if (fflush(stdout) == EOF) { gcu_perror(name, "stdout"); status = 1; }
    return status;
}

static int cmd_headtail(const char *name, int argc, char **argv, int tail)
{
    long count = 10;
    int index = 1;
    if (index < argc && argv[index][0] == '-' && argv[index][1]) {
        char *end = NULL;
        long parsed = strtol(argv[index] + 1, &end, 10);
        if (end && *end == '\0' && parsed >= 0) { count = parsed; ++index; }
    }
    if (argc - index > 1) return gcu_usage(name, "[-COUNT] [FILE]");
    FILE *input = open_input(name, index < argc ? argv[index] : "-");
    if (!input) return 1;

    int status = 0;
    if (!tail) {
        char *line = NULL;
        size_t capacity = 0;
        ssize_t length;
        while (count > 0 && (length = getline(&line, &capacity, input)) >= 0) {
            if (fwrite(line, 1, (size_t)length, stdout) != (size_t)length) { status = 1; break; }
            --count;
        }
        free(line);
    } else {
        /* Ring buffer: memory is proportional to the requested line count. */
        char **lines = calloc((size_t)(count ? count : 1), sizeof *lines);
        size_t *lengths = calloc((size_t)(count ? count : 1), sizeof *lengths);
        if (!lines || !lengths) { free(lines); free(lengths); gcu_error(name, "out of memory"); close_input(name, input); return 1; }
        char *line = NULL;
        size_t capacity = 0, used = 0, next = 0;
        ssize_t length;
        while (count > 0 && (length = getline(&line, &capacity, input)) >= 0) {
            char *copy = malloc((size_t)length);
            if (!copy) { gcu_error(name, "out of memory"); status = 1; break; }
            memcpy(copy, line, (size_t)length);
            free(lines[next]);
            lines[next] = copy;
            lengths[next] = (size_t)length;
            next = (next + 1) % (size_t)count;
            if (used < (size_t)count) ++used;
        }
        size_t start = used == (size_t)count ? next : 0;
        for (size_t i = 0; i < used; ++i) {
            size_t slot = (start + i) % (size_t)count;
            if (lines[slot] && fwrite(lines[slot], 1, lengths[slot], stdout) != lengths[slot]) status = 1;
            free(lines[slot]);
        }
        free(line); free(lines); free(lengths);
    }

    if (ferror(input)) { gcu_perror(name, "input"); status = 1; }
    if (close_input(name, input)) status = 1;
    if (ferror(stdout)) { gcu_perror(name, "stdout"); status = 1; }
    return status;
}

static int cmd_wcnt(const char *name, int argc, char **argv)
{
    int count_lines = 0, count_words = 0, count_bytes = 0, count_chars = 0;
    int i = 1;
    for (; i < argc && argv[i][0] == '-'; ++i) {
        if (!strcmp(argv[i], "--")) { ++i; break; }
        for (const char *p = argv[i] + 1; *p; ++p) {
            if (*p == 'l') count_lines = 1;
            else if (*p == 'w') count_words = 1;
            else if (*p == 'c') count_bytes = 1;
            else if (*p == 'm') count_chars = 1;
            else { gcu_error(name, "unknown count option"); return 2; }
        }
    }
    if (!count_lines && !count_words && !count_bytes && !count_chars)
        count_lines = count_words = count_bytes = 1;

    int files = argc - i;
    if (!files) files = 1;
    int status = 0;
    for (int f = 0; f < files; ++f) {
        const char *path = argc - i ? argv[i + f] : "-";
        FILE *input = open_input(name, path);
        if (!input) { status = 1; continue; }
        unsigned long long lines = 0, words = 0, bytes = 0, chars = 0;
        int c, in_word = 0;
        while ((c = getc(input)) != EOF) {
            ++bytes; ++chars;
            if (c == '\n') ++lines;
            if (isspace((unsigned char)c)) in_word = 0;
            else if (!in_word) { ++words; in_word = 1; }
        }
        if (ferror(input)) { gcu_perror(name, path); status = 1; }
        if (count_lines) printf("%llu ", lines);
        if (count_words) printf("%llu ", words);
        if (count_bytes) printf("%llu ", bytes);
        if (count_chars) printf("%llu ", chars);
        if (strcmp(path, "-") != 0 || argc - i > 1) printf("%s", path);
        putchar('\n');
        if (close_input(name, input)) status = 1;
    }
    return status;
}

int gcu_run_text(const char *name, int argc, char **argv)
{
    if (!strcmp(name, "cowf")) return cmd_cowf(name, argc, argv);
    if (!strcmp(name, "ftop")) return cmd_headtail(name, argc, argv, 0);
    if (!strcmp(name, "fend")) return cmd_headtail(name, argc, argv, 1);
    if (!strcmp(name, "wcnt")) return cmd_wcnt(name, argc, argv);
    if (!strcmp(name, "type")) return cmd_type(name, argc, argv);
    if (!strcmp(name, "true")) return 0;
    if (!strcmp(name, "false")) return 1;
    if (!strcmp(name, "no") || !strcmp(name, "yes")) {
        const char *text = argc > 1 ? argv[1] : (!strcmp(name, "no") ? "y" : "n");
        while (puts(text) != EOF) { }
        return ferror(stdout) ? 1 : 0;
    }
    return -2;
}
