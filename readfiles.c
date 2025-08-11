#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <syslog.h>

typedef struct {
    int pid;
    char name[256];
    char state;
    long threads;
    float cpu_time;      // segundos
    float life_time;     // segundos
    float cpu_usage_pct;
} proc_info_t;

int readfiles(int count, int *pids, proc_info_t *proc_infos, int max_procs, int *filled_count)
{
    char path[256];
    char linha[1000];
    int tiques = sysconf(_SC_CLK_TCK);

    FILE *uptimeFile = fopen("/proc/uptime", "r");
    if (!uptimeFile)
    {
        perror("Erro abrindo /proc/uptime");
        return 1;
    }
    float upTime = 0.0f;
    if (fgets(linha, sizeof(linha), uptimeFile) != NULL)
        sscanf(linha, "%f", &upTime);
    fclose(uptimeFile);

    int index = 0;

    for (int i = 0; i < count && index < max_procs; i++)
    {
        snprintf(path, sizeof(path), "/proc/%d/stat", pids[i]);
        FILE *file = fopen(path, "r");
        if (!file)
        {
            continue;
        }

        if (fgets(linha, sizeof(linha), file) == NULL)
        {
            fclose(file);
            continue;
        }
        fclose(file);

        char *start = strchr(linha, '(');
        char *end = strrchr(linha, ')');
        if (!start || !end || end < start)
        {
            continue;
        }
        size_t len = end - start - 1;
        char comm[256];
        if (len >= sizeof(comm))
            len = sizeof(comm) - 1;
        strncpy(comm, start + 1, len);
        comm[len] = '\0';

        int offset = (end - linha) + 2;

        unsigned long utime, stime, startTime;
        long numThreads;
        char state;

        int res = sscanf(linha + offset,
                         "%c %*d %*d %*d %*d %*d %*u %*u %*u %*u %*u %lu %lu %*d %*d %*d %*d %ld %*d %lu",
                         &state, &utime, &stime, &numThreads, &startTime);

        if (res != 5)
        {
            continue;
        }

        float cpuTime = (float)(utime + stime) / tiques;
        float startTimeSecs = (float)startTime / tiques;
        float processLife = upTime - startTimeSecs;
        float cpuUsage = processLife > 0 ? (cpuTime / processLife) * 100.0f : 0.0f;

        if (cpuUsage < 5.0f)
            continue;

        proc_infos[index].pid = pids[i];
        strncpy(proc_infos[index].name, comm, sizeof(proc_infos[index].name));
        proc_infos[index].state = state;
        proc_infos[index].threads = numThreads;
        proc_infos[index].cpu_time = cpuTime;
        proc_infos[index].life_time = processLife;
        proc_infos[index].cpu_usage_pct = cpuUsage;

        index++;
    }

    *filled_count = index;

    return 0;
}
