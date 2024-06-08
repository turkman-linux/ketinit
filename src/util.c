#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#define c_mkdir(A, B) \
    if (mkdir(A, B) < 0) { \
        fprintf(stderr, "Error: %s %s\n", "failed to create directory.", A); \
}

int isdir(char *path){
    if(path == NULL){
        return 0;
    }
    DIR* dir = opendir(path);
    if(dir){
        closedir(dir);
        return 1;
    }else{
        return 0;
    }
}

void create_dir(char *dir) {
    char tmp[1024];
    char *p = NULL;
    size_t len;

    snprintf(tmp, sizeof(tmp),"%s",dir);
    len = strlen(tmp);
    if (tmp[len - 1] == '/')
        tmp[len - 1] = 0;
    for (p = tmp + 1; *p; p++)
        if (*p == '/') {
            *p = 0;
            if(!isdir(tmp))
                c_mkdir(tmp, 0755);
            *p = '/';
        }
    if(!isdir(tmp))
        c_mkdir(tmp, 0755);
}

void redirect_log(const char* name){
    FILE *out_file;
    char log_path[256];
    create_dir("/var/log/services/");
    snprintf(log_path, sizeof(log_path), "/var/log/services/%s.log", name);
    
    // Open the file to redirect both stdout and stderr
    out_file = freopen(log_path, "w", stdout);
    if (out_file == NULL) {
        perror("freopen");
        exit(EXIT_FAILURE);
    }

    // Redirect stderr to the same file as stdout
    if (dup2(fileno(out_file), STDERR_FILENO) == -1) {
        perror("dup2");
        exit(EXIT_FAILURE);
    }
}
