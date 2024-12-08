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
char* get_value(char* name, char* variable);
char** get_value_array(char* name, char* variable, int* len);
void init_mount();

void redirect_log(const char* name);

int waitfile(char *fname);

void create_sandbox();

void socket_init();
void client_init();
char* socket_read();
void client_send(char* data);


#define START 1
#define STOP 2
#define STATUS 3
#define KILL 4
#define ENABLE 5
#define DISABLE 6

#include <string.h>
#define startswith(A,B) \
    strncmp(A, B, strlen(B)) == 0
