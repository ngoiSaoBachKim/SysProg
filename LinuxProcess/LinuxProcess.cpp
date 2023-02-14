#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

int main() {
  printf("PID\tPPID\tCMD\n");

  DIR* proc_dir = opendir("/proc");
  if (!proc_dir) {
    printf("Error occured.");
    return 1;
  }

  char status_path[128], cmdline_path[128], cmd[256], line[256];
  while (struct dirent* dir_entry = readdir(proc_dir)) {
    const char* pid_str = dir_entry->d_name;
    if (!isdigit(*pid_str)) {
      continue;
    }

    snprintf(status_path, sizeof(status_path), "/proc/%s/status", pid_str);
    FILE* status_file = fopen(status_path, "r");
    if (!status_file) {
      continue;
    }

    int pid = atoi(pid_str), ppid = 0;
    while (fgets(line, sizeof(line), status_file)) {
      if (strncmp(line, "PPid:", 5) == 0) {
        ppid = atoi(line + 5);
      }
    }

    fclose(status_file);

    snprintf(cmdline_path, sizeof(cmdline_path), "/proc/%s/cmdline", pid_str);
    FILE* cmdline_file = fopen(cmdline_path, "r");
    if (!cmdline_file) {
      continue;
    }

    fgets(cmd, sizeof(cmd), cmdline_file);
    fclose(cmdline_file);

    printf("%d\t%d\t%s\n", pid, ppid, cmd);
  }

  closedir(proc_dir);
  return 0;
}
