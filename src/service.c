#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mount.h>

#include "init.h"

static void execute_service(char* name, char* arg){
    char command_path[256];
    snprintf(command_path, sizeof(command_path), "/etc/services/%s", name);
    char* args[] = {command_path, arg, NULL};
    execvp(command_path, args);
}

void init_mount(){
    mount("", "/", "", MS_REMOUNT, NULL);
    mount("devtmpfs", "/dev", "devtmpfs", 0, NULL);
    mount("devpts", "/dev/pts", "devpts", 0, NULL);
    mount("sysfs", "/sys", "sysfs", 0, NULL);
    mount("proc", "/proc", "proc", 0, NULL);
    mount("tmpfs", "/run", "tmpfs", 0, NULL);
    mount("none", "/sys/fs/cgroup", "cgroup2", 0, NULL);
}


int service(char* name, int status){
    switch(status) {
        case START:
            cgroup_init(name);
            if (fork()){
                cgroup_add(name);
                execute_service(name, "start");
            }
            break;
        case STOP:
            cgroup_kill(name);
            break;
        case STATUS:
            if(cgroup_check_running(name)){
                printf("Service %s : RUNNING\n", name);
                return 1;
            } else if(cgroup_exists(name)){
                printf("Service %s : EXITED\n", name);
            } else {
                printf("Service %s : STOP\n", name);
            }
            return 0;
    }
    return 1;
}
