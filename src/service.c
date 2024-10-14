#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mount.h>
#include <sys/wait.h>

#include "init.h"

void execute_service(char* name, char* arg){
    char command_path[256];
    snprintf(command_path, sizeof(command_path), "/etc/boot.d/%s", name);
    if(access(command_path, F_OK) != 0) {
        exit(127);
    }
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

void service_exit_event(char* name, int status){
    if(status == 0){
        printf("[OK] %s\n", name);
    } else {
        printf("[FAIL] %s (%d)\n", name, status / 256);
    }
}

int service(char* name, int status){
    int exit_code;
    int pid;
    switch(status) {
        case START:
            if(cgroup_check_running(name)){
                return 0;
            }
            int len;
            char** deps = get_value_array(name, "depends", &len);
            for(int i=0;i<len;i++){
                service(deps[i], START);
            }
            cgroup_init(name);
            pid = fork();
            if (pid == 0){
                cgroup_add(name);
                redirect_log(name);
                execute_service(name, "start");
            }
            waitpid(pid, &exit_code,0);
            service_exit_event(name, exit_code);
            return exit_code;
            break;
        case STOP:
            pid = fork();
            if (pid == 0){
                cgroup_add(name);
                redirect_log(name);
                execute_service(name, "stop");
            }
            waitpid(pid, &exit_code,0);
            cgroup_kill(name);
            service_exit_event(name, exit_code);
            return exit_code;
            break;
        case KILL:
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

char* get_value(char* name, char* variable){
    char path[256];
    snprintf(path, sizeof(path), "/etc/boot.d/%s", name);
    FILE *file = fopen(path,"r");
    char line[1024];
    if (file == NULL){
        return "";
    }
    while (fgets(line, sizeof(line), file)) {
        if (strlen(line) < 4 || line[0] == '#') {
            continue;
        }
        if (startswith(line, variable) && startswith(line+strlen(variable), "=\"")) {
            int s = strlen(line) - strlen(variable);
            int ss = strlen(variable);
            char* ret = calloc(s, sizeof(char*));
            strncpy(ret,  line+ss+2 , s*sizeof(char));
            fclose(file);
            for(int l=s-1;l>=0;l--){
                if(ret[l] == '\n'){
                    ret[l] = '\0';
                }
                if (ret[0] == '"'){
                    ret++;
                }
                if (ret[l] == '"'){
                    ret[l] = '\0';
                }
            }
            return ret;
        }
    }
    fclose(file);
    return "";
}

char** get_value_array(char* name, char* variable, int* len){
    char* value = get_value(name, variable);
    *len = 0;
    char **tokens = malloc(sizeof(char*));
    char *token = strtok(value, " ");
    while (token != NULL) {
        tokens = realloc(tokens, (*len + 1) * sizeof(char*));
        tokens[*len] = strdup(token);
        (*len)++;
        token = strtok(NULL, " ");
    }
    return tokens;
}
