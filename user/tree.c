
#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/fs.h"
#include "kernel/fcntl.h"
#include "user/user.h"

#define MAX_PATH 512

void tree(char *path, int level, int maxDepth, int onlyDir)
{
    int fd;
    struct stat st;
    struct dirent de;
    char buf[MAX_PATH];
    char *p;

    fd = open(path, O_RDONLY);
    if (fd < 0)
    {
        fprintf(2, "tree: cannot open %s\n", path);
        return;
    }

    if (fstat(fd, &st) < 0)
    {
        fprintf(2, "tree: cannot stat %s\n", path);
        close(fd);
        return;
    }

    // In duong dan goc
    if (level == 0)
    {
        if (!onlyDir || st.type == T_DIR)
            printf("%s\n", path);
    }

    // Khong phai thu muc thi khong duyet tiep
    if (st.type != T_DIR)
    {
        close(fd);
        return;
    }

    // Dat gioi han do sau thi dung
    if (maxDepth >= 0 && level >= maxDepth)
    {
        close(fd);
        return;
    }

    // Kiem tra do dai duong dan
    if (strlen(path) + 1 + DIRSIZ + 1 > sizeof(buf))
    {
        fprintf(2, "tree: path too long\n");
        close(fd);
        return;
    }

    // Doc cac muc trong thu muc
    while (read(fd, &de, sizeof(de)) == sizeof(de))
    {
        if (de.inum == 0)
            continue;

        // Bo qua "." va ".."
        if (strcmp(de.name, ".") == 0 ||
            strcmp(de.name, "..") == 0)
            continue;

        // Tao duong dan den muc con
        strcpy(buf, path);
        p = buf + strlen(buf);
        *p++ = '/';
        memmove(p, de.name, DIRSIZ);
        p[DIRSIZ] = 0;

        // Lay thong tin muc con
        if (stat(buf, &st) < 0)
        {
            fprintf(2, "tree: cannot stat %s\n", buf);
            continue;
        }

        // Neu bat -d, chi hien thi thu muc
        if (onlyDir && st.type != T_DIR)
            continue;

        // Thut le theo do sau
        for (int i = 0; i < level; i++)
            printf("    ");

        printf("|-- %s\n", de.name);

        // De quy neu la thu muc
        if (st.type == T_DIR)
            tree(buf, level + 1, maxDepth, onlyDir);
    }

    close(fd);
}

int main(int argc, char *argv[])
{
    char *path = ".";
    int maxDepth = -1;
    int onlyDir = 0;
    int i;

    for (i = 1; i < argc; i++)
    {
        if (strcmp(argv[i], "-L") == 0)
        {
            if (i + 1 >= argc)
            {
                fprintf(2, "usage: tree [path] [-L depth] [-d]\n");
                exit(1);
            }

            maxDepth = atoi(argv[++i]);

            if (maxDepth < 1)
            {
                fprintf(2, "tree: depth must be positive\n");
                exit(1);
            }
        }
        else if (strcmp(argv[i], "-d") == 0)
        {
            onlyDir = 1;
        }
        else
        {
            path = argv[i];
        }
    }

    tree(path, 0, maxDepth, onlyDir);
    exit(0);
}
