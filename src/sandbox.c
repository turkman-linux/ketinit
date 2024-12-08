#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <sys/mount.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <sched.h>

void create_sandbox() {
    // Unshare the mount namespace
    if (unshare(CLONE_NEWNS) == -1) {
        perror("unshare");
        exit(EXIT_FAILURE);
    }

    // Create the necessary directories for the overlay filesystem
    mkdir("/run/ket/", 0755);
    mount("tmpfs", "/run/ket/", "tmpfs", 0, NULL);
    mkdir("/run/ket/root", 0755);
    mkdir("/run/ket/upper", 0755);
    mkdir("/run/ket/work", 0755);

    // Set up the overlay filesystem
    if (mount("overlay", "/run/ket/root/", "overlay", 0,
               "lowerdir=/,upperdir=/run/ket/upper,workdir=/run/ket/work") == -1) {
        perror("mount");
        exit(EXIT_FAILURE);
    }

    // Bind mount /run to the new root
    mount("/run", "/run/ket/root/run/", NULL, MS_SILENT | MS_BIND | MS_REC, NULL);

    // Change root to the new directory
    if (chroot("/run/ket/root") == -1) {
        perror("chroot");
        exit(EXIT_FAILURE);
    }

    // Mount necessary filesystems
    mount("proc", "/proc", "proc", 0, NULL);
    mount("sysfs", "/sys", "sysfs", 0, NULL);
    mount("devtmpfs", "/dev", "devtmpfs", 0, NULL);

    // Change the working directory
    chdir("/");
    unshare(CLONE_NEWNET);

}
