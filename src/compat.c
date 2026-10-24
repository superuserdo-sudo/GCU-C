#define _POSIX_C_SOURCE 200809L
#include "gcu.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

struct alias { const char *gcu; const char *host; };

/* Compatibility mappings are intentionally centralized and easy to audit. */
static const struct alias aliases[] = {
    {"fcow", "tac"}, {"numl", "nl"}, {"wfof", "od"},
    {"b32", "base32"}, {"b64", "base64"}, {"bnc", "base64"},
    {"rff", "fmt"}, {"pag", "pr"}, {"warp", "fold"},
    {"fspl", "split"}, {"cspat", "csplit"}, {"bsum", "sum"},
    {"cksm", "cksum"}, {"md5c", "md5sum"}, {"b2ck", "b2sum"},
    {"sh1c", "sha1sum"}, {"sh224", "sha224sum"}, {"sh256", "sha256sum"},
    {"sh384", "sha384sum"}, {"sh512", "sha512sum"}, {"ordf", "sort"},
    {"shufx", "shuf"}, {"dupf", "uniq"}, {"lcom", "comm"},
    {"dtop", "tsort"}, {"ptix", "ptx"}, {"fldx", "cut"},
    {"mrgc", "paste"}, {"jkey", "join"}, {"chrx", "tr"},
    {"t2sp", "expand"}, {"s2tb", "unexpand"}, {"dblk", "dd"},
    {"instme", "install"}, {"wipf", "shred"}, {"nodef", "mknod"},
    {"ownf", "chown"}, {"grpown", "chgrp"}, {"permf", "chmod"},
    {"dfree", "df"}, {"dusage", "du"}, {"truncf", "truncate"},
    {"fprn", "printf"}, {"testmp", "test"}, {"exval", "expr"},
    {"mout", "tee"}, {"mbn", "basename"}, {"mdn", "dirname"},
    {"jroot", "chroot"}, {"prio", "nice"}, {"stay", "nohup"},
    {"ttycfg", "stty"}, {"idme", "id"}, {"grps", "groups"},
    {"wimlog", "who"}, {"usrlite", "logname"}, {"usrs", "users"},
    {"wit", "date"}, {"hidn", "hostid"}, {"wimh", "hostname"},
    {"ncore", "nproc"}, {"upsys", "uptime"}, {"wimt", "tty"},
    {"sigsend", "kill"}, {"mmsleep", "sleep"}, {"factr", "factor"},
    {"seqn", "seq"}, {"tmpf", "mktemp"}, {"dumpme", "hexdump"},
    {"bufctl", "stdbuf"}, {"tmo", "timeout"}, {"ctxset", "chcon"}, {"ctxrun", "runcon"},
    {"alodir", "ls"}, {"dir", "ls"}
};

static const char *find_host_command(const char *name)
{
    for (size_t i = 0; i < sizeof aliases / sizeof aliases[0]; ++i)
        if (!strcmp(name, aliases[i].gcu)) return aliases[i].host;
    return NULL;
}

static int path_exists(const char *name)
{
    const char *path = getenv("PATH");
    if (!path) path = "/bin:/usr/bin";
    char *copy = strdup(path);
    if (!copy) return 0;
    int found = 0;
    char *save = NULL;
    for (char *part = strtok_r(copy, ":", &save); part; part = strtok_r(NULL, ":", &save)) {
        if (!*part) part = ".";
        size_t needed = strlen(part) + strlen(name) + 2;
        char *candidate = malloc(needed);
        if (!candidate) break;
        snprintf(candidate, needed, "%s/%s", part, name);
        if (access(candidate, X_OK) == 0) found = 1;
        free(candidate);
        if (found) break;
    }
    free(copy);
    return found;
}

static int validate_path(const char *name, int argc, char **argv)
{
    if (argc < 2) return gcu_usage(name, "PATH...");
    int status = 0;
    for (int i = 1; i < argc; ++i) {
        const unsigned char *p = (const unsigned char *)argv[i];
        if (!*p || strchr(argv[i], '\n') || strchr(argv[i], '\r')) {
            fprintf(stderr, "%s: invalid pathname: %s\n", name, argv[i]);
            status = 1;
        } else {
            for (; *p; ++p) {
                if (*p < 32 || *p == 127) {
                    fprintf(stderr, "%s: pathname contains a control character\n", name);
                    status = 1;
                    break;
                }
            }
        }
    }
    return status;
}

int gcu_run_compat(const char *name, int argc, char **argv)
{
    if (!strcmp(name, "pchk")) return validate_path(name, argc, argv);
    if (!strcmp(name, "ftgn")) {
        char **args = calloc((size_t)argc + 1, sizeof *args);
        if (!args) { gcu_perror(name, "memory"); return 1; }
        args[0] = (char *)"numfmt";
        for (int i = 1; i < argc; ++i) args[i] = argv[i];
        execvp(args[0], args);
        gcu_perror(name, args[0]); free(args); return errno == ENOENT ? 127 : 126;
    }

    const char *host = find_host_command(name);
    if (!host) {
        fprintf(stderr, "%s: no implementation is registered for this command\n", name);
        return 127;
    }
    if (!path_exists(host)) {
        fprintf(stderr, "%s: requires host utility '%s', which was not found in PATH\n", name, host);
        return 127;
    }

    /* Preserve argv boundaries; no shell is involved. */
    char **args = calloc((size_t)argc + 1, sizeof *args);
    if (!args) { gcu_perror(name, "memory"); return 1; }
    args[0] = (char *)host;
    for (int i = 1; i < argc; ++i) args[i] = argv[i];
    args[argc] = NULL;
    execvp(host, args);
    gcu_perror(name, host);
    free(args);
    return errno == ENOENT ? 127 : 126;
}
