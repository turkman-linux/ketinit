#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/stat.h>

#include "init.h"

void cgroup_init(const char* cgroup_name) {
    char cgroup_path[256];
    snprintf(cgroup_path, sizeof(cgroup_path), "/sys/fs/cgroup/%s", cgroup_name);
    create_dir(cgroup_path);
}

void cgroup_add(const char* cgroup_name){
    char cgroup_path[256];
    snprintf(cgroup_path, sizeof(cgroup_path), "/sys/fs/cgroup/%s/cgroup.procs", cgroup_name);
    puts(cgroup_path);
    FILE* cg = fopen(cgroup_path, "a");
    if (cg == NULL) {
        perror("Error opening cgroup.procs file");
        exit(EXIT_FAILURE);
    }
    fprintf(cg,"%d", getpid());
    fclose(cg);
}

void cgroup_kill(const char* cgroup_name) {
    char cgroup_path[256];
    snprintf(cgroup_path, sizeof(cgroup_path), "/sys/fs/cgroup/%s/cgroup.kill", cgroup_name);
    FILE* cg = fopen(cgroup_path, "w");
    if (cg == NULL) {
        perror("Error opening cgroup.kill file");
        exit(EXIT_FAILURE);
    }
    fprintf(cg,"%d", 1);
    fclose(cg);
    remove(cgroup_path);
}

int cgroup_exists(const char* cgroup_name){
    char cgroup_path[256];
    snprintf(cgroup_path, sizeof(cgroup_path), "/sys/fs/cgroup/%s/", cgroup_name);
    return isdir(cgroup_path);
}

bool cgroup_check_running(const char* cgroup_name) {
    char cgroup_path[256];
    snprintf(cgroup_path, sizeof(cgroup_path), "/sys/fs/cgroup/%s/cgroup.procs", cgroup_name);
    FILE* file = fopen(cgroup_path, "r");
    char buffer[1];
    size_t bytes_read = fread(buffer, 1, sizeof(buffer), file);
    fclose(file);
    return bytes_read != 0;
}
