//build command: g++ -std=c++20 -o linuxProcess linuxProcess.cpp

#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

int main() {
  printf("PID\tPPID\tCMD\n");

// Open the "/proc" directory without root rights needed
  DIR* proc_dir = opendir("/proc");
  if (!proc_dir) {
    printf("Error occured.");
    return 1;
  }

  char status_path[4096], cmdline_path[4096], cmd[4096], line[4096];
  while (struct dirent* dir_entry = readdir(proc_dir)) {
    const char* pid_str = dir_entry->d_name;
	// Skip entries in the "/proc" directory that are not PIDs.
    if (!isdigit(*pid_str)) {
      continue;
    }
    
    snprintf(status_path, sizeof(status_path), "/proc/%s/status", pid_str);
    FILE* status_file = fopen(status_path, "r");
    if (!status_file) {
      continue;
    }

	//get pid
    int pid = stoi(pid_str), ppid = 0;
	 //get ppid by read each line
    while (fgets(line, sizeof(line), status_file)) {
      if (strncmp(line, "PPid:", 5) == 0) {
        ppid = stoi(line + 5);
      }
    }

    fclose(status_file);
	//get command line
    snprintf(cmdline_path, sizeof(cmdline_path), "/proc/%s/cmdline", pid_str);
    FILE* cmdline_file = fopen(cmdline_path, "r");
    if (!cmdline_file) {
      continue;
    }
	
    fgets(cmd, sizeof(cmd), cmdline_file);
    fclose(cmdline_file);
	
	//print PID info
    printf("%d\t%d\t%s\n", pid, ppid, cmd);
  }

  closedir(proc_dir);
  return 0;
}
