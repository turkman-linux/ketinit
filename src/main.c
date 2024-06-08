#include <string.h>
#include <stdio.h>
#include <unistd.h>

#include "init.h"

int main(int argc, char** argv){
    if(argc>2){
        if(strcmp(argv[2], "start") == 0){
            return service(argv[1], START);
        }else if(strcmp(argv[2], "stop") == 0){
            return service(argv[1], STOP);
        }else if(strcmp(argv[2], "status") == 0){
            return service(argv[1], STATUS);
        }
    }
    if(getpid() == 1){
        init_mount();
    }
    return 0;
}
