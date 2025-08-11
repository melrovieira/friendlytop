#ifndef READFILES_H
#define READFILES_H

typedef struct {
    int pid;
    char name[256];
    char state;
    long threads;
    float cpu_time;      // segundos
    float life_time;     // segundos
    float cpu_usage_pct;
} proc_info_t;


int readfiles(int count, int *pids, proc_info_t *proc_infos, int max_procs, int *filled_count);

#endif // READFILES_H
