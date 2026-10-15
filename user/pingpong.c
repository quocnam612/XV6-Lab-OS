#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(int argc, char *argv[]) {
    int p[2];
    int c[2];
    char buf = 'a';

    if (pipe(p) < 0 || pipe(c) < 0) {
        fprintf(2, "pipe failed\n");
        exit(1);
    }

    int pid = fork();
    if (pid < 0) {
        fprintf(2, "fork failed\n");
        exit(1);
    }

    if (pid == 0) {
        // Child process
        close(p[1]);
        close(c[0]);

        if (read(p[0], &buf, 1) != 1) {
            fprintf(2, "child read failed\n");
            exit(1);
        }
        printf("%d: received ping\n", getpid());

        if (write(c[1], &buf, 1) != 1) {
            fprintf(2, "child write failed\n");
            exit(1);
        }

        close(p[0]);
        close(c[1]);
        exit(0);
    } else {
        // Parent process
        close(p[0]);
        close(c[1]);

        if (write(p[1], &buf, 1) != 1) {
            fprintf(2, "parent write failed\n");
            exit(1);
        }

        if (read(c[0], &buf, 1) != 1) {
            fprintf(2, "parent read failed\n");
            exit(1);
        }
        printf("%d: received pong\n", getpid());

        close(p[1]);
        close(c[0]);
        wait(0);
        exit(0);
    }
}
