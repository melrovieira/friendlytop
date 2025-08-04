#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <syslog.h>


int readfiles(int count, int *pids)
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

    int horas = (int)(upTime / 3600);
    int minutos = ((int)upTime % 3600) / 60;
    float segundos = upTime - (horas * 3600 + minutos * 60);

    printf("Uptime do sistema: %dh %dm %.2fs\n\n", horas, minutos, segundos);

    for (int i = 0; i < count; i++)
    {
        snprintf(path, sizeof(path), "/proc/%d/stat", pids[i]);
        FILE *file = fopen(path, "r");
        if (!file)
        {
            perror("Erro abrindo /proc/[pid]/stat");
            continue;
        }

        if (fgets(linha, sizeof(linha), file) == NULL)
        {
            fclose(file);
            continue;
        }
        fclose(file);

        // Extrair o comm entre parênteses:
        char *start = strchr(linha, '(');
        char *end = strrchr(linha, ')');
        if (!start || !end || end < start)
        {
            fprintf(stderr, "Erro ao parsear comm para PID %d\n", pids[i]);
            continue;
        }
        size_t len = end - start - 1;
        char comm[256];
        if (len >= sizeof(comm))
            len = sizeof(comm) - 1;
        strncpy(comm, start + 1, len);
        comm[len] = '\0';

        // Agora pegamos o resto dos campos depois do ')'
        int offset = (end - linha) + 2; // pula ')' e o espaço seguinte

        // Definição dos tipos corretos
        unsigned long utime, stime, startTime;
        long numThreads;
        char state;

        int res = sscanf(linha + offset,
                         "%c %*d %*d %*d %*d %*d %*u %*u %*u %*u %*u %lu %lu %*d %*d %*d %*d %ld %*d %lu",
                         &state, &utime, &stime, &numThreads, &startTime);

        if (res != 5)
        {
            fprintf(stderr, "Erro ao parsear stat para PID %d\n", pids[i]);
            continue;
        }

        
        float cpuTime = (float)(utime + stime) / tiques;

        float startTimeSecs = (float)startTime / tiques;
        float processLife = upTime - startTimeSecs;
        float cpuUsage = processLife > 0 ? (cpuTime / processLife) * 100.0f : 0.0f;

        int cpu_h = (int)(cpuTime / 3600);
        int cpu_m = ((int)cpuTime % 3600) / 60;
        float cpu_s = cpuTime - (cpu_h * 3600 + cpu_m * 60);

        int life_h = (int)(processLife / 3600);
        int life_m = ((int)processLife % 3600) / 60;
        float life_s = processLife - (life_h * 3600 + life_m * 60);

        if (cpuUsage >= 5)
        {

            printf("PID: %d\n", pids[i]);
            printf("Name: %s\n", comm);
            printf("State: %c\n", state);
            printf("Threads: %ld\n", numThreads);
            printf("CPU: %dh %dm %.2fs\n", cpu_h, cpu_m, cpu_s);
            printf("Tempo desde start: %dh %dm %.2fs\n", life_h, life_m, life_s);
            printf("Uso CPU: %.2f%%\n\n", cpuUsage);
        }
    }
    return 0;
}
