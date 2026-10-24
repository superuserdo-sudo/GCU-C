#define _POSIX_C_SOURCE 200809L
#include "gcu.h"

#include <errno.h>
#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/utsname.h>
#include <unistd.h>

extern char **environ;

int gcu_run_system(const char *name, int argc, char **argv)
{
    if (!strcmp(name, "huh")) { printf("%lu\n", (unsigned long)getuid()); return 0; }
    if (!strcmp(name, "wami")) {
        struct passwd *user = getpwuid(geteuid());
        if (!user) { printf("%lu\n", (unsigned long)geteuid()); return 0; }
        puts(user->pw_name); return 0;
    }
    if (!strcmp(name, "wimarch") || !strcmp(name, "mos")) {
        struct utsname system;
        if (uname(&system) < 0) { gcu_perror(name, "uname"); return 1; }
        if (!strcmp(name, "wimarch")) puts(system.machine);
        else printf("%s %s %s %s %s\n", system.sysname, system.nodename,
                    system.release, system.version, system.machine);
        return 0;
    }
    if (!strcmp(name, "envshow")) {
        if (argc == 1) {
            for (char **item = environ; *item; ++item) puts(*item);
            return ferror(stdout) ? 1 : 0;
        }
        int status = 0;
        for (int i = 1; i < argc; ++i) {
            const char *value = getenv(argv[i]);
            if (value) puts(value);
            else status = 1;
        }
        return status;
    }
    if (!strcmp(name, "envx")) {
        int index = 1;
        while (index < argc && strchr(argv[index], '=') && argv[index][0] != '=') {
            char *assignment = strdup(argv[index]);
            if (!assignment) { gcu_perror(name, "memory"); return 1; }
            char *equals = strchr(assignment, '=');
            *equals = '\0';
            if (setenv(assignment, equals + 1, 1) < 0) {
                gcu_perror(name, assignment); free(assignment); return 1;
            }
            free(assignment);
            ++index;
        }
        if (index == argc) {
            for (char **item = environ; *item; ++item) puts(*item);
            return ferror(stdout) ? 1 : 0;
        }
        execvp(argv[index], &argv[index]);
        gcu_perror(name, argv[index]);
        return errno == ENOENT ? 127 : 126;
    }
    if (!strcmp(name, "pmad")) {
        fputs("pmad: shell functions must be loaded into the current shell; use: source /path/to/share/gcu/shell/pmad.sh\n", stderr);
        return 2;
    }
    return -2;
}
