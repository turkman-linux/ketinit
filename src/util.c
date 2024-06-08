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

size_t get_file_size(const char *filename) {
    int file_descriptor;
    size_t file_size;

    // Open the file
    file_descriptor = open(filename, O_RDONLY);
    if (file_descriptor == -1) {
        perror("Error opening file");
        return -1;
    }

    // Move the file pointer to the end of the file
    file_size = lseek(file_descriptor, 0, SEEK_END);
    if (file_size == -1) {
        perror("Error getting file size");
        close(file_descriptor);
        return -1;
    }

    // Close the file
    close(file_descriptor);

    return file_size;
}
