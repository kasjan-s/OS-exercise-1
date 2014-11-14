/*
 *  Autor: Kasjan Siwek
 *  Indeks: 306827
 *  Data: 02.11.2014
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "err.h"

#define BUF_SIZE 80

// Nazwa programu z procesem W(i)
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

    int pipe_d[4][2];

    int i;
    for (i = 0; i < 4; ++i) {
        if (pipe(pipe_d[i]) == -1) 
            syserr("Failed to create a pipe\n");
    }

    // Lacze z ktorego odbierac bedzie Pascal od procesu W(1)
    int src = pipe_d[2][0];

    for (i = 0; i < n; ++i) {
        switch (fork ()) {
            case -1: 
                syserr("Failed to fork\n");

            // Proces potomny
            case 0:
                // Podmiana stdin
                if (close(0) == -1) syserr("Process %i, failed to close(0)", i);
                if (dup(pipe_d[0][0]) != 0) syserr("Process %i, failed to dup", i);

                // Podmiana stdout
                if (close(1) == -1) syserr("Process %i, failed to close(1)", i);
                if (i != n-1) {
                    if (dup(pipe_d[1][1]) != 1) syserr("Process %i, failed to dup", i);
                }
                
                // Zamkniecie nieuzywanych lacz
                if (close(pipe_d[0][1]) != 0) syserr("Process %i, failed to close(0, 1)", i);
                if (close(pipe_d[1][0]) != 0) syserr("Process %i, failed to close(1, 0)", i);

                if (close(pipe_d[2][0]) != 0) syserr("Process %i, failed to close(2, 0)", i);
                if (close(pipe_d[3][1]) != 0) syserr("Process %i, failed to close(3, 1)", i);

                // Konwersja argumentow do stringow
                char n_str[10];
                sprintf(n_str,"%d", n);
                char i_str[10];
                sprintf(i_str, "%d", i);
                char read_dsc[10];
                // W(n) nie ma lacza do pisania
                sprintf(read_dsc, "%d", pipe_d[3][0]);
                char write_dsc[10];
                sprintf(write_dsc, "%d", pipe_d[2][1]);

                // Uruchomienie procesu W(i)
                execl(subprocess_cmd, subprocess, n_str, i_str, read_dsc, write_dsc, (char *) 0);
                syserr("Error in execl\n");

            default:
                // Zamkniecie nieuzywanych lacz
                if (close(pipe_d[0][0]) != 0) syserr("Pascal failed to close(0, 0) for %d", i);
                if (close(pipe_d[0][1]) != 0) syserr("Pascal failed to close(0, 1) for %d", i);
                if (i != 0)
                    if (close(pipe_d[2][0]) != 0) syserr("Pascal failed to close(2, 0) for %d", i);
                if (close(pipe_d[2][1]) != 0) syserr("Pascal failed to close(2, 1) for %d", i);

                // Przejscie krok dalej - podmiana zapisanych lacz
                pipe_d[0][0] = pipe_d[1][0];
                pipe_d[0][1] = pipe_d[1][1];
                pipe_d[2][0] = pipe_d[3][0];
                pipe_d[2][1] = pipe_d[3][1];

                // Otworzenie nowych lacz dla komunikacji z nastepnym subprocesem
                if (pipe(pipe_d[1]) == -1) syserr("Pascal failed to pipe");
                if (pipe(pipe_d[3]) == -1) syserr("Pascal failed to pipe");
        }
    }

    // Odbieranie danych od procesow W(i) - wspolczynniki trojkata Pascala
    char buf[BUF_SIZE];

    while(1) {
        int len = read(src, buf, 80);
        if (len == -1)
            syserr("Parent failed to read a value\n");

        // Jesli to ostatni wspolczynnik, to wypisz i przerwij petle
        if (buf[len - 1] == 'E') {
            buf[len - 1] = '\0';
            printf("%s\n", buf);
            break;
        } else {
            buf[len] = '\0';
            printf("%s", buf);
        }
    }

    // Czekamy na zakonczenie procesow potomnych
    for (i = 0; i < n; ++i) {
        if (wait (0) == -1) 
            syserr("Waiting failed");
    }
    exit (0);
}
