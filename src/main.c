#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <dirent.h>
#include "init.h"

extern int start_service(char* name);

int main(int argc, char** argv){
    if(argc>2){
        if(strcmp(argv[2], "start") == 0){
            return service(argv[1], START);
        }else if(strcmp(argv[2], "stop") == 0){
            return service(argv[1], STOP);
        }else if(strcmp(argv[2], "status") == 0){
            return service(argv[1], STATUS) == 0;
        }else if(strcmp(argv[2], "kill") == 0){
            return service(argv[1], KILL) == 0;
        }
    }
    if(getpid() == 1){
        init_mount();
    }
    struct dirent *de;
    DIR *dr = opendir("/etc/boot.d");
    if (dr == NULL) {
        return 1;
    }
    while ((de = readdir(dr)) != NULL) {
        if(strlen(de->d_name) > 0 && de->d_name[0] == '.'){
            continue;
        }
        if(fork()){
            service(de->d_name, START);
        }
    }
    closedir(dr);
    return 0;
}
