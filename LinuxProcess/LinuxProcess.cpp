#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

int main() 
{
    printf("PID\tPPID\tCMD\n");

    DIR *dir = opendir("/proc");
    if (!dir) 
    {
        perror("opendir");
        exit(EXIT_FAILURE);
    }

    char path[128], cmd[256];
    while (struct dirent *ent = readdir(dir)) 
    {
        if (!isdigit(*ent->d_name))
            continue;

        snprintf(path, sizeof(path), "/proc/%s/status", ent->d_name);
        FILE *file = fopen(path, "r");
        if (!file)
            continue;

        int pid = atoi(ent->d_name), ppid = 0;
        char line[256];
        while (fgets(line, sizeof(line), file))
            if (strncmp(line, "PPid:", 5) == 0)
                ppid = atoi(line + 5);

        fclose(file);

        snprintf(path, sizeof(path), "/proc/%s/cmdline", ent->d_name);
        file = fopen(path, "r");
        if (!file)
            continue;

        fgets(cmd, sizeof(cmd), file);
        fclose(file);

        printf("%d\t%d\t%s\n", pid, ppid, cmd);
    }

    closedir(dir);
    return EXIT_SUCCESS;
}
