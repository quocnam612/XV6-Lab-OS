#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/fcntl.h"
#include "user/user.h"

int main(int argc, char *argvp[]) {
    if(argc != 3) {
        fprintf(2, "usage: cp src dst\n");
        exit(1);
    }

    int src = open(argvp[1], O_RDONLY);
    if(src < 0) {
        fprintf(2, "cp: cannot open %s\n", argvp[1]);
        exit(1);
    }

    int dst = open(argvp[2], O_WRONLY | O_CREATE | O_TRUNC);
    if(dst < 0) {
        fprintf(2, "cp: cannot create %s\n", argvp[2]);
        close(src);
        exit(1);
    }

    char buf[512];
    int n;
    while((n = read(src, buf, sizeof(buf))) > 0) {
        if(write(dst, buf, n) != n) {
            fprintf(2, "cp: write error\n");
            close(src);
            close(dst);
            exit(1);
        }
    }
    if(n < 0) {
        fprintf(2, "cp: read error\n");
        close(src);
        close(dst);
        exit(1);
    }

    close(src);
    close(dst);
    exit(0);
}