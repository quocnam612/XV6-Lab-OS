#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/fcntl.h"
#include "kernel/fs.h"
#include "user/user.h"

long du(char *path, int flag_a, int flag_s, int is_root) {
    long total_size = 0;
    int fd;
    char buf[512], *p;
    struct dirent de;
    struct stat st;

    if ((fd = open(path, O_RDONLY)) < 0) {
        fprintf(2, "du: cannot open %s\n", path);
        return 0;
    }

    if (fstat(fd, &st) < 0) {
        fprintf(2, "du: cannot stat %s\n", path);
        close(fd);
        return 0;
    }

    switch (st.type) {
    case T_FILE:
    case T_DEVICE:
        total_size = st.size;
        if (flag_a && (!flag_s || is_root)) {
            printf("%ld\t%s\n", total_size, path);
        }
        break;

    case T_DIR:
        if (strlen(path) + 1 + DIRSIZ + 1 > sizeof(buf)) {
            fprintf(2, "du: path too long\n");
            close(fd);
            return 0;
        }

        strcpy(buf, path);
        p = buf + strlen(buf);
        *p++ = '/';

        while (read(fd, &de, sizeof(de)) == sizeof(de)) {
            if (de.inum == 0)
                continue;

            if (strcmp(de.name, ".") == 0 || strcmp(de.name, "..") == 0)
                continue;

            memmove(p, de.name, DIRSIZ);
            p[DIRSIZ] = 0;

            total_size += du(buf, flag_a, flag_s, 0);
        }

        if (!flag_s || is_root) {
            printf("%ld\t%s\n", total_size, path);
        }
        break;
    }

    close(fd);
    return total_size;
}

int main(int argc, char *argv[]) {
    char *path = ".";
    int flag_a = 0;
    int flag_s = 0;

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-a") == 0) {
            flag_a = 1;
        } else if (strcmp(argv[i], "-s") == 0) {
            flag_s = 1;
        } else {
            path = argv[i];
        }
    }

    du(path, flag_a, flag_s, 1);

    exit(0);
}