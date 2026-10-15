
#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fcntl.h"

#define MAX_LINE 256

// Doc mot dong tu file
// Tra ve: 1 = thanh cong, 0 = EOF, -1 = loi doc, -2 = dong qua dai
int readline(int fd, char *buf, int max)
{
    int i = 0;
    int n;
    char ch;

    if (max <= 0)
        return -2;

    while (1)
    {
        n = read(fd, &ch, 1);

        if (n < 0)
            return -1;

        if (n == 0)
        {
            if (i == 0)
                return 0;
            break;
        }

        if (ch == '\n')
            break;

        if (i >= max - 1)
            return -2;

        buf[i++] = ch;
    }

    buf[i] = '\0';
    return 1;
}

// So sanh hai file theo tung dong
// Tra ve: 0 = thanh cong, -1 = loi doc file

int compareFiles(int fd1, int fd2, char *file1, char *file2, int quiet)
{
    char line1[MAX_LINE];
    char line2[MAX_LINE];

    int result1, result2;
    int lineNumber = 1;
    int different;

    while (1)
    {
        result1 = readline(fd1, line1, MAX_LINE);
        result2 = readline(fd2, line2, MAX_LINE);

        if (result1 < 0 || result2 < 0)
        {
            if (result1 == -2 || result2 == -2)
                fprintf(2, "diff: line too long\n");
            else
                fprintf(2, "diff: read error\n");

            return -1;
        }

        if (result1 == 0 && result2 == 0)
            break;

        different = 0;

        if (result1 == 0 || result2 == 0)
            different = 1;
        else if (strcmp(line1, line2) != 0)
            different = 1;

        if (different)
        {
            if (quiet)
            {
                printf("diff: files differ\n");
                return 1;
            }

            if (result1 == 0)
            {
                printf("%s:%d: < EOF\n", file1, lineNumber);
                printf("%s:%d: > %s\n", file2, lineNumber, line2);
            }
            else if (result2 == 0)
            {
                printf("%s:%d: < %s\n", file1, lineNumber, line1);
                printf("%s:%d: > EOF\n", file2, lineNumber);
            }
            else
            {
                printf("%s:%d: < %s\n", file1, lineNumber, line1);
                printf("%s:%d: > %s\n", file2, lineNumber, line2);
            }
        }

        lineNumber++;
    }

    return 0;
}

int main(int argc, char *argv[])
{
    int fd1, fd2;
    int result;
    int quiet = 0;

    char *file1;
    char *file2;

    // Kiem tra va xu ly tham so
    if (argc == 3)
    {
        file1 = argv[1];
        file2 = argv[2];
    }
    else if (argc == 4 && strcmp(argv[1], "-q") == 0)
    {
        quiet = 1;
        file1 = argv[2];
        file2 = argv[3];
    }
    else if (argc == 4 && strcmp(argv[3], "-q") == 0)
    {
        quiet = 1;
        file1 = argv[1];
        file2 = argv[2];
    }
    else
    {
        fprintf(2, "Usage: diff [-q] file1 file2 [-q]\n");
        exit(1);
    }

    fd1 = open(file1, O_RDONLY);
    if (fd1 < 0)
    {
        fprintf(2, "diff: cannot open %s\n", file1);
        exit(1);
    }

    fd2 = open(file2, O_RDONLY);
    if (fd2 < 0)
    {
        fprintf(2, "diff: cannot open %s\n", file2);
        close(fd1);
        exit(1);
    }

    result = compareFiles(fd1, fd2, file1, file2, quiet);

    close(fd1);
    close(fd2);

    if (result < 0)
        exit(1);

    exit(0);
}
