#include <stdio.h>
#include <stdlib.h>
#include "pid_scanner.h"
#include "readfiles.h"




        

int main()
{


    int pids[1000];
    int count = 0;
    if (scanner(&count, pids) != 0) {
        fprintf(stderr, "Erro no scanner\n");
        return 1;
    }

    if (readfiles(count, pids) != 0) {
        fprintf(stderr, "Erro lendo arquivos\n");
        return 1;
    }

    printf("Total de processos: %i\n", count);
    return 0;
}

