#include <stdio.h>
#include <dirent.h>
#include <sys/stat.h>
#include <string.h>
#include <stdlib.h> 
#include <ctype.h>
#include "pid_scanner.h"


int scanner(int *count, int *out){  
    
    struct dirent *de;
    struct stat info;

    DIR *procDir = opendir("/proc");
    if (!procDir || procDir == NULL) {
        perror("Erro");
        return 1;
    }
        int Localcount = 0;
        int MaxSize = 1000;
   while ((de = readdir(procDir)) != NULL){
            char caminho[1024];
            int pid = atoi(de->d_name);
            snprintf(caminho, sizeof(caminho), "/proc/%s", de->d_name);
            int res = stat(caminho, &info);
            if (S_ISDIR(info.st_mode) && isdigit(de->d_name[0])) {
                
                if (Localcount < MaxSize){
                    out[Localcount] = pid;
                    Localcount++;
                }
            }
            else {
                continue;
            }
   }

   closedir(procDir);
    *count = Localcount;
    return 0;
}