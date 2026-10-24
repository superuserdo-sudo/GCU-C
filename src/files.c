#define _POSIX_C_SOURCE 200809L
#define _XOPEN_SOURCE 700
#include "gcu.h"

#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

int gcu_copy_file(const char *src, const char *dst)
{
    int input = open(src, O_RDONLY | O_CLOEXEC);
    if (input < 0) { gcu_perror("cop", src); return 1; }

    struct stat st;
    mode_t mode = 0666;
    if (fstat(input, &st) == 0 && S_ISREG(st.st_mode))
        mode = st.st_mode & 0777;

    int output = open(dst, O_WRONLY | O_CREAT | O_TRUNC | O_CLOEXEC, mode);
    if (output < 0) {
        int saved = errno;
        close(input);
        errno = saved;
        gcu_perror("cop", dst);
        return 1;
    }

    /* A larger buffer reduces syscall overhead for ordinary file copies. */
    char buffer[256 * 1024];
    int status = 0;
    for (;;) {
        ssize_t got = read(input, buffer, sizeof buffer);
        if (got == 0) break;
        if (got < 0) {
            if (errno == EINTR) continue;
            gcu_perror("cop", src);
            status = 1;
            break;
        }

        ssize_t offset = 0;
        while (offset < got) {
            ssize_t written = write(output, buffer + offset, (size_t)(got - offset));
            if (written < 0) {
                if (errno == EINTR) continue;
                gcu_perror("cop", dst);
                status = 1;
                goto finished;
            }
            offset += written;
        }
    }

finished:
    if (close(input) < 0 && status == 0) { gcu_perror("cop", src); status = 1; }
    if (close(output) < 0 && status == 0) { gcu_perror("cop", dst); status = 1; }
    return status;
}

static int list_directory(const char *name, const char *path, bool detailed, bool show_hidden)
{
    DIR *directory = opendir(path);
    if (!directory) { gcu_perror(name, path); return 1; }

    int status = 0;
    struct dirent *entry;
    while ((entry = readdir(directory)) != NULL) {
        const char *item = entry->d_name;
        if (!show_hidden && item[0] == '.') continue;

        if (!detailed) {
            puts(item);
            continue;
        }

        struct stat info;
        if (fstatat(dirfd(directory), item, &info, AT_SYMLINK_NOFOLLOW) < 0) {
            gcu_perror(name, item);
            status = 1;
            continue;
        }

        char type = S_ISDIR(info.st_mode) ? 'd' :
                    S_ISLNK(info.st_mode) ? 'l' :
                    S_ISFIFO(info.st_mode) ? 'p' :
                    S_ISSOCK(info.st_mode) ? 's' :
                    S_ISCHR(info.st_mode) ? 'c' :
                    S_ISBLK(info.st_mode) ? 'b' : '-';
        printf("%c %04o %10lld %s\n", type, (unsigned)(info.st_mode & 07777), (long long)info.st_size, item);
    }

    if (closedir(directory) < 0) { gcu_perror(name, path); status = 1; }
    return status;
}

static int cmd_list(const char *name, int argc, char **argv, bool detailed, bool brief)
{
    bool hidden = false;
    int first_path = 1;
    for (int i = 1; i < argc; ++i) {
        if (!strcmp(argv[i], "-a") || !strcmp(argv[i], "--all")) hidden = true;
        else if (!strcmp(argv[i], "-l") || !strcmp(argv[i], "--long")) detailed = true;
        else if (!strcmp(argv[i], "--")) { first_path = i + 1; break; }
        else if (argv[i][0] == '-') {
            char message[256]; snprintf(message, sizeof message, "unsupported option: %s", argv[i]);
            gcu_error(name, message); return 2;
        } else { first_path = i; break; }
        first_path = i + 1;
    }

    if (first_path >= argc) return list_directory(name, ".", detailed, hidden);
    int status = 0;
    for (int i = first_path; i < argc; ++i) {
        struct stat st;
        if (stat(argv[i], &st) < 0) { gcu_perror(name, argv[i]); status = 1; continue; }
        if (S_ISDIR(st.st_mode)) {
            if (argc - first_path > 1) printf("%s:\n", argv[i]);
            if (list_directory(name, argv[i], detailed, hidden)) status = 1;
        } else {
            if (detailed) printf("- %04o %10lld %s\n", (unsigned)(st.st_mode & 07777), (long long)st.st_size, argv[i]);
            else puts(argv[i]);
        }
    }
    (void)brief;
    return status;
}

static int cmd_mkdir(const char *name, int argc, char **argv)
{
    bool parents = false;
    mode_t mode = 0777;
    int i = 1;
    for (; i < argc; ++i) {
        if (!strcmp(argv[i], "-p")) parents = true;
        else if (!strcmp(argv[i], "-m") && i + 1 < argc) {
            char *end = NULL; unsigned long parsed = strtoul(argv[++i], &end, 8);
            if (!end || *end || parsed > 07777) return gcu_usage(name, "[-p] [-m MODE] DIRECTORY...");
            mode = (mode_t)parsed;
        } else if (!strcmp(argv[i], "--")) { ++i; break; }
        else if (argv[i][0] == '-') return gcu_usage(name, "[-p] [-m MODE] DIRECTORY...");
        else break;
    }
    if (i == argc) return gcu_usage(name, "[-p] [-m MODE] DIRECTORY...");

    int status = 0;
    for (; i < argc; ++i) {
        if (parents) {
            char *copy = strdup(argv[i]);
            if (!copy) { gcu_perror(name, "memory"); return 1; }
            for (char *p = copy + (copy[0] == '/'); ; ++p) {
                if (*p != '/' && *p != '\0') continue;
                char saved = *p; *p = '\0';
                if (*copy && mkdir(copy, mode) < 0 && errno != EEXIST) {
                    gcu_perror(name, copy); status = 1; *p = saved; break;
                }
                *p = saved;
                if (!saved) break;
            }
            free(copy);
        } else if (mkdir(argv[i], mode) < 0) {
            gcu_perror(name, argv[i]); status = 1;
        }
    }
    return status;
}

int gcu_run_files(const char *name, int argc, char **argv)
{
    if (!strcmp(name, "see")) return cmd_list(name, argc, argv, false, false);
    if (!strcmp(name, "dir")) return cmd_list(name, argc, argv, false, true);
    if (!strcmp(name, "alodir")) return cmd_list(name, argc, argv, true, false);

    if (!strcmp(name, "wai")) {
        char path[PATH_MAX];
        if (!getcwd(path, sizeof path)) { gcu_perror(name, "current directory"); return 1; }
        puts(path); return 0;
    }
    if (!strcmp(name, "cop")) {
        if (argc != 3) return gcu_usage(name, "SOURCE DEST");
        return gcu_copy_file(argv[1], argv[2]);
    }
    if (!strcmp(name, "mdir")) return cmd_mkdir(name, argc, argv);
    if (!strcmp(name, "punch")) {
        if (argc < 2) return gcu_usage(name, "FILE...");
        int status = 0;
        for (int i = 1; i < argc; ++i) {
            int fd = open(argv[i], O_WRONLY | O_CREAT | O_CLOEXEC, 0666);
            if (fd < 0) { gcu_perror(name, argv[i]); status = 1; }
            else if (close(fd) < 0) { gcu_perror(name, argv[i]); status = 1; }
        }
        return status;
    }
    if (!strcmp(name, "rom")) {
        if (argc < 2) return gcu_usage(name, "FILE...");
        int status = 0;
        for (int i = 1; i < argc; ++i) {
            if (!strcmp(argv[i], "/") || !strcmp(argv[i], ".") || !strcmp(argv[i], "..")) {
                gcu_error(name, "refusing unsafe path"); status = 1; continue;
            }
            if (unlink(argv[i]) < 0) { gcu_perror(name, argv[i]); status = 1; }
        }
        return status;
    }
    if (!strcmp(name, "movef")) {
        if (argc != 3) return gcu_usage(name, "SOURCE DEST");
        if (rename(argv[1], argv[2]) == 0) return 0;
        if (errno != EXDEV) { gcu_perror(name, argv[1]); return 1; }
        if (gcu_copy_file(argv[1], argv[2])) return 1;
        if (unlink(argv[1]) < 0) { gcu_perror(name, argv[1]); return 1; }
        return 0;
    }
    if (!strcmp(name, "hlnk") || !strcmp(name, "lnkf")) {
        if (argc != 3) return gcu_usage(name, "TARGET LINK");
        int rc = !strcmp(name, "hlnk") ? link(argv[1], argv[2]) : symlink(argv[1], argv[2]);
        if (rc < 0) { gcu_perror(name, argv[2]); return 1; }
        return 0;
    }
    if (!strcmp(name, "rdem")) {
        if (argc < 2) return gcu_usage(name, "DIRECTORY...");
        int status = 0;
        for (int i = 1; i < argc; ++i) if (rmdir(argv[i]) < 0) { gcu_perror(name, argv[i]); status = 1; }
        return status;
    }
    if (!strcmp(name, "ulnk")) {
        if (argc != 2) return gcu_usage(name, "FILE");
        if (unlink(argv[1]) < 0) { gcu_perror(name, argv[1]); return 1; }
        return 0;
    }
    if (!strcmp(name, "mkq")) {
        if (argc < 2) return gcu_usage(name, "NAME...");
        int status = 0;
        for (int i = 1; i < argc; ++i) if (mkfifo(argv[i], 0666) < 0) { gcu_perror(name, argv[i]); status = 1; }
        return status;
    }
    if (!strcmp(name, "rlnk")) {
        if (argc < 2) return gcu_usage(name, "SYMLINK...");
        int status = 0;
        for (int i = 1; i < argc; ++i) {
            char buffer[PATH_MAX]; ssize_t n = readlink(argv[i], buffer, sizeof buffer - 1);
            if (n < 0) { gcu_perror(name, argv[i]); status = 1; continue; }
            buffer[n] = '\0'; puts(buffer);
        }
        return status;
    }
    if (!strcmp(name, "cpath")) {
        if (argc < 2) return gcu_usage(name, "PATH...");
        int status = 0;
        for (int i = 1; i < argc; ++i) {
            char resolved[PATH_MAX];
            if (!realpath(argv[i], resolved)) { gcu_perror(name, argv[i]); status = 1; }
            else puts(resolved);
        }
        return status;
    }
    if (!strcmp(name, "fstat")) {
        if (argc < 2) return gcu_usage(name, "FILE...");
        int status = 0;
        for (int i = 1; i < argc; ++i) {
            struct stat st;
            if (lstat(argv[i], &st) < 0) { gcu_perror(name, argv[i]); status = 1; continue; }
            printf("%s: size=%lld mode=%04o uid=%u gid=%u inode=%llu links=%llu\n",
                   argv[i], (long long)st.st_size, (unsigned)(st.st_mode & 07777),
                   (unsigned)st.st_uid, (unsigned)st.st_gid,
                   (unsigned long long)st.st_ino, (unsigned long long)st.st_nlink);
        }
        return status;
    }
    if (!strcmp(name, "fsync")) {
        if (argc == 1) { sync(); return 0; }
        int status = 0;
        for (int i = 1; i < argc; ++i) {
            int fd = open(argv[i], O_RDONLY | O_CLOEXEC);
            if (fd < 0) { gcu_perror(name, argv[i]); status = 1; continue; }
            if (fsync(fd) < 0) { gcu_perror(name, argv[i]); status = 1; }
            if (close(fd) < 0) { gcu_perror(name, argv[i]); status = 1; }
        }
        return status;
    }
    if (!strcmp(name, "dclr")) {
        puts("GCU directory colors: disabled by default"); return 0;
    }
    return -2;
}
