#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <dirent.h>
#include <limits.h>
#include <stdlib.h>
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
            return 0;
        }else if(strcmp(argv[2], "disable") == 0){
            service(argv[1], DISABLE);
            return 0;
        }
        memcpy(action+1, argv[1], strlen(argv[1]));
        client_send(action);
        return 0;
    }
    redirect_log("ketinit");
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
        service(de->d_name, START);
    }
    closedir(dr);
    while(1){
        char* action = socket_read();
        int status = action[0];
        char* name = strdup(action+1);
        if(fork() == 0){
            int ret = service(name, status);
            exit(ret);
        }
    }
    return 0;
}
