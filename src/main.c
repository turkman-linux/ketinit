#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <dirent.h>
#include <limits.h>
#include "init.h"

extern int start_service(char* name);

int main(int argc, char** argv){
    if(getpid() == 1){
        socket_init();
        init_mount();
    } else {
        client_init();
    }
    if(argc>2){
        char action[1024];
        memset(&action, 0, sizeof(action));
        if(strcmp(argv[2], "start") == 0){
            action[0] = START;
        }else if(strcmp(argv[2], "stop") == 0){
            action[0] = STOP;
        }else if(strcmp(argv[2], "status") == 0){
            service(argv[1], STATUS);
        }else if(strcmp(argv[2], "kill") == 0){
            action[0] = KILL;
        }else if(strcmp(argv[2], "enable") == 0){
            service(argv[1], ENABLE);
        }else if(strcmp(argv[2], "disable") == 0){
            service(argv[1], DISABLE);
        }
        memcpy(action+1, argv[1], strlen(argv[1]));
        client_send(action);
        return 0;
    }
    struct dirent *de;
    DIR *dr = opendir("/etc/boot.d");
    if (dr == NULL) {
        return 1;
    }
    fclose(stdin);
    while ((de = readdir(dr)) != NULL) {
        if(strlen(de->d_name) > 0 && de->d_name[0] == '.'){
            continue;
        }
        if(fork() == 0){
            service(de->d_name, START);
            return 0;
        }
    }
    closedir(dr);
    while(1){
        char* action = socket_read();
        int status = action[0];
        char* name = strdup(action+1);
        service(name, status);
    }
    return 0;
}
