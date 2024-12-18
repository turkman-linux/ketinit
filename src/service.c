#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mount.h>
#include <sys/wait.h>
#include <sys/stat.h>

#include <pwd.h>

#define SLEEP_TIME 300

#include "init.h"

void execute_service(char* name, char* arg){
    char command_path[1024];
    snprintf(command_path, sizeof(command_path), "/etc/ket/%s", name);
    if(access(command_path, F_OK) != 0) {
        exit(127);
    }
    chmod(command_path, 0755);
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
    mount("cgroup2", "/sys/fs/cgroup", "cgroup2", 0, NULL);
}

void service_exit_event(char* name, int status){
    if(status == 0){
        printf("[DONE] %s\n", name);
    } else {
        cgroup_kill(name);
        printf("[FAIL] %s (%d)\n", name, status / 256);
    }
}

int service(char* name, int status){
    int exit_code;
    int pid;
    char source_path[1024];
    char target_path[1024];
    switch(status) {
        case START:
            if(cgroup_exists(name)){
                return 0;
            }
            /* Dependencies */
            int len;
            char** deps = get_value_array(name, "depends", &len);
            char* user = get_value(name, "user");
            char* priv = get_value(name, "privileged");
            int status = 0;
            for(int i=0;i<len;i++){
                status = service(deps[i], START);
                if (status != 0) {
                    return status;
                }
            }
            /* wait until file ready */
            char* wfile = get_value(name, "waitfile");
            if (strcmp(wfile, "") != 0) {
                waitfile(wfile);
            }
            printf("[START] %s\n", name);
            cgroup_init(name);
            pid = fork();
            if (pid == 0){
                cgroup_add(name);
                redirect_log(name);
                if(strcmp(priv, "true") != 0) {
                    create_sandbox();
                }
                if(strcmp(user, "") != 0) {
                    struct passwd *pw = getpwnam(user);
                    if (pw == NULL) {
                        fprintf(stderr, "User %s not found.\n", user);
                        exit(1);
                    }
                    if (setuid(pw->pw_uid) != 0) {
                        perror("setuid failed");
                        return 1;
                    }
                }
                execute_service(name, "start");
            }
            if(strcmp(get_value(name, "daemon"), "true") == 0) {
                return 0;
            } else {
                waitpid(pid, &exit_code,0);
                service_exit_event(name, exit_code);
                return exit_code;
            }
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
        case ENABLE:
            snprintf(source_path, sizeof(source_path), "../ket/%s", name);
            snprintf(target_path, sizeof(target_path), "/etc/boot.d/%s", name);
            symlink(source_path, target_path);
            break;
        case DISABLE:
            snprintf(target_path, sizeof(target_path), "/etc/boot.d/%s", name);
            if(access(target_path, F_OK) == 0) {
                remove(target_path);
            }
            break;
    }
    return 1;
}

/* wait until file exists */
int waitfile(char *fname){
    while(1){
       if(access( fname, F_OK ) == 0 ){
           return 0; /* Found */
       }
       usleep(SLEEP_TIME*1000);
    }
}


char* get_value(char* name, char* variable){
    char path[1024];
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
