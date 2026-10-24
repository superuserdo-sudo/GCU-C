#ifndef GCU_H
#define GCU_H

#include <stddef.h>

#define GCU_VERSION "0.4.0"
#define GCU_NAME "GCU"

int gcu_run_text(const char *name, int argc, char **argv);
int gcu_run_files(const char *name, int argc, char **argv);
int gcu_run_system(const char *name, int argc, char **argv);
int gcu_run_compat(const char *name, int argc, char **argv);

void gcu_error(const char *name, const char *message);
void gcu_perror(const char *name, const char *path);
int gcu_usage(const char *name, const char *usage);
int gcu_copy_file(const char *src, const char *dst);
const char *gcu_basename(const char *path);

#endif
