#include <ncurses.h>
#include <unistd.h>
#include "pid_scanner.h"

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

int main()
{
    int pids[1000];
    int count = 0;
    proc_info_t procs[1000];
    int proc_count = 0;

    initscr();
    cbreak();
    noecho();
    curs_set(FALSE);
    nodelay(stdscr, TRUE);

    while (1)
    {
        count = 0;

        if (scanner(&count, pids) != 0)
            break;

        if (readfiles(count, pids, procs, 1000, &proc_count) != 0)
            break;

        clear();

        mvprintw(0, 0, "Total processos: %d (exibindo %d com CPU>=5%%)", count, proc_count);
        int line = 2;

        for (int i = 0; i < proc_count && line < LINES - 1; i++)
        {
            int h_cpu = (int)(procs[i].cpu_time / 3600);
            int m_cpu = ((int)procs[i].cpu_time % 3600) / 60;
            float s_cpu = procs[i].cpu_time - (h_cpu * 3600 + m_cpu * 60);

            int h_life = (int)(procs[i].life_time / 3600);
            int m_life = ((int)procs[i].life_time % 3600) / 60;
            float s_life = procs[i].life_time - (h_life * 3600 + m_life * 60);

            mvprintw(line++, 0, "PID: %d - %s (State: %c) Threads: %ld", procs[i].pid, procs[i].name, procs[i].state, procs[i].threads);
            mvprintw(line++, 2, "CPU: %dh %dm %.2fs  Life: %dh %dm %.2fs  Uso CPU: %.2f%%", h_cpu, m_cpu, s_cpu, h_life, m_life, s_life, procs[i].cpu_usage_pct);
        }

        mvprintw(LINES - 1, 0, "Pressione Q para sair");

        refresh();

        int ch = getch();
        if (ch == 'q' || ch == 'Q')
            break;

        sleep(1);
    }

    endwin();

    return 0;
}
