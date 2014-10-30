#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "err.h"

#define BUF_SIZE 80

char subprocess[] = "process";
char subprocess_cmd[] = "./process";

int main (int argc, char *argv[])
{
    if (argc != 2) {
        fatal("Usage: ./process <n>");
        exit(0);
    }

    int n = atoi(argv[1]);

    if (n < 1) {
        fatal("n has to be at least 1!");
        exit(0);
    }

    // 2n lacz, ze wzgledu na komunikacje dwukierunkowa
    int pipe_dsc[2*n][2];

    int i;
    for (i = 0; i < 2*n; ++i) {
        if (pipe (pipe_dsc[i]) == -1) 
            syserr("Failed to create a pipe\n");
    }


    for (i = 0; i < n; ++i) {
        switch (fork ()) {
            case -1: 
                syserr("Failed to fork\n");

            // Proces potomny
            case 0:
                // Podmiana stdin i stdout
                if (close (0) == -1)            syserr("Process %i, failed to close(0)", i);
                if (close (1) == -1)            syserr("Process %i, failed to close(1)", i);
                if (dup (pipe_dsc[i][0]) != 0)    syserr("child, dup (pipe_dsc [0])");
                // W(n) nie ma W(n+1) do ktorego moglby napisac
                if (i != n-1) {
                    if (dup (pipe_dsc[i+1][1]) != 1)    syserr("child, dup (pipe_dsc [1])");
                }

                int j;
                for (j = 0; j < n; ++j) {
                    if (j != i) {
                        if (close (pipe_dsc[j][0]) == -1) syserr("child %d, close (pipe_dsc[%d][0])", i, j);
                        if (close (pipe_dsc[n+j][1]) == -1) syserr("child %d, close (pipe_dsc[n+%d][0])", i, j);
                        // Ostatni proces nie ma lacz 'za soba', ktore moglby zamknac
                        if (j != n-1) {
                            if (close (pipe_dsc[j+1][1]) == -1) syserr("child %d, close (pipe_dsc[%d+1][1])", i, j);
                            if (close (pipe_dsc[n+j+1][0]) == -1) syserr("child %d, close (pipe_dsc[n+%d+1][1]", i, j);
                        }
                    }
                }

                // Konwersja argumentow do stringow
                char n_str[10];
                sprintf(n_str,"%d", n);
                char i_str[10];
                sprintf(i_str, "%d", i);
                char read_dsc[10];
                sprintf(read_dsc, "%d", pipe_dsc[n+i+1][0]);
                char write_dsc[10];
                sprintf(write_dsc, "%d", pipe_dsc[n+i][1]);

                // Uruchomienie procesu W(i)
                execl(subprocess_cmd, subprocess, n_str, i_str, read_dsc, write_dsc, (char *) 0);
                syserr("Error in execl\n");

        }
    }

    // Czekamy na zakonczenie procesow potomnych
    for (i = 0; i < n; ++i) {
        if (wait (0) == -1) 
            syserr("Waiting failed");
    }
    exit (0);
}
