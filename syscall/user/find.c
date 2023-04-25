#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

#define NULL (void *)0

void find(const char *path, const char *target)
{
    int fd;
    struct stat st;
    struct dirent de;
    char fullpath[512];
    char *cur;

    fd = open(path, 0);
    if (fd < 0) {
        fprintf(2, "find: No such file or directory %s\n", path);
        return;
    }

    if (fstat(fd, &st) < 0) {
        fprintf(2, "find: Cannot stat %s\n", path);
        close(fd);
        return;
    }

    switch (st.type) {
    case T_DEVICE:
    case T_FILE:
        if (!target || !strcmp(path, target)) {
            write(1, path, strlen(path));
            write(1, "\n", 1);
        }
            
        break;

    case T_DIR:
        if(strlen(path) + 1 > sizeof(fullpath)) {
            fprintf(2, "find: path too long\n");
            break;
        }
        strcpy(fullpath, path);
        cur = fullpath + strlen(fullpath);
        *cur++ = '/';

        while (read(fd, &de, sizeof(de)) == sizeof(de)){
            if(de.inum == 0 || !strcmp(de.name, ".") || !strcmp(de.name, ".."))
                continue;
            
            if (strlen(path) + 1 + strlen(de.name) + 1 > sizeof(fullpath)) {
                fprintf(2, "find: path too long\n");
                break;
            }
            strcpy(cur, de.name);

            if (!strcmp(de.name, target)){
                write(1, fullpath, strlen(fullpath));
                write(1, "\n", 1);
            }
                
            if (stat(fullpath, &st) < 0)
                fprintf(2, "find: Cannot stat %s\n", path);

            if (st.type == T_DIR) 
                find(fullpath, target);
        }   
        break;
    }

    close(fd);
}

int main(int argc, char *argv[])
{
    if (argc < 2) {
        find(".", NULL);
        exit(0);
    }
    else if (argc == 2) {
        find(argv[1], NULL);
        exit(0);
    }

    for (int i = 0; i < argc - 2; i++)
        find(argv[1], argv[i + 2]);

    exit(0);
}