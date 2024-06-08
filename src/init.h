#include <stdbool.h>
int isdir(char *path);
void create_dir(char *dir);
size_t get_file_size(const char *filename);

void cgroup_init(const char* cgroup_name);
void cgroup_kill(const char* cgroup_name);
void cgroup_add(const char* cgroup_name);
bool cgroup_check_running(const char* cgroup_name);
int cgroup_exists(const char* cgroup_name);

int service(char* name, int status);

void init_mount();

void redirect_log(const char* name);

#define START 1
#define STOP 0
#define STATUS 2
