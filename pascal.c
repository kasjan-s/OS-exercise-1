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
    int pipe_dsc[2*n][2];

    int i;
    for (i = 0; i < 2*n; ++i) {
        if (pipe (pipe_dsc[i]) == -1) syserr("Error in pipe\n");
    }


    for (i = 0; i < n; ++i) {
        switch (fork ()) {
            case -1: 
                syserr("Error in fork\n");

            case 0:
                if (close (0) == -1)            syserr("child, close (%d)", i);
                if (close (1) == -1)            syserr("child, close (%d)", i);
//                fprintf(stderr, "Jestem procesem %d i bede uzywal %d jako stdin (koniec pisany %d)\n", i, pipe_dsc[i][0],
//                        pipe_dsc[i][1]);
                if (dup (pipe_dsc[i][0]) != 0)    syserr("child, dup (pipe_dsc [0])");
                if (i != n-1) {
//                    fprintf(stderr, "Jestem procesem %d i bede uzywal %d jako stdout (koniec czytany %d)\n", i, pipe_dsc[i+1][1],
//                        pipe_dsc[i+1][0]);
                    if (dup (pipe_dsc[i+1][1]) != 1)    syserr("child, dup (pipe_dsc [1])");
                }

                int j;
                for (j = 0; j < n; ++j) {
                    if (j != i) {
                        if (close (pipe_dsc[j][0]) == -1) syserr("child %d, close (pipe_dsc[%d][0])", i, j);
                        if (close (pipe_dsc[n+j][1]) == -1) syserr("child %d, close (pipe_dsc[n+%d][0])", i, j);
                        if (j != n-1) {
                            if (close (pipe_dsc[j+1][1]) == -1) syserr("child %d, close (pipe_dsc[%d+1][1])", i, j);
                            if (close (pipe_dsc[n+j+1][0]) == -1) syserr("child %d, close (pipe_dsc[n+%d+1][1]", i, j);
                        }
                    }
                }
/*
                char str[10];
                fgets(str, 10, stdin);
                int aux = atoi(str);
                fprintf(stderr, "Jestem procesem %d i odebralem %d\n", i, aux);
                aux = aux + 1;
                if (i != n-1) {
                    printf("%d\n", aux);
                    fprintf(stderr, "Jestem procesem %d i wypisalem %d\n", i, aux);
                }

                if ( i != n-1) {
                    int len = read(pipe_dsc[n+i+1][0], str, 10);
                    if (len == -1)
                        syserr("child %d, failed read", i);
                    aux = atoi(str);
                    fprintf(stderr, "Jestem procesem %d i odebralem z prawej %d bajtow, %d\n", i, len, aux);
                }

                aux = aux + 1;
                sprintf(str,"%d\n", aux);
                if (i != 0) {
                    int len = write(pipe_dsc[n+i][1], str, 10);
                    if (len == -1)
                        syserr("child %d, failed write", i);
                    fprintf(stderr, "Jestem procesem %d i wyslalem na lewo %d bajtow, %d\n", i, len, aux);
                }


                return 0;
                */

                char n_str[10];
                sprintf(n_str,"%d", n);
                char i_str[10];
                sprintf(i_str, "%d", i);
                char read_dsc[10];
                sprintf(read_dsc, "%d", pipe_dsc[n+i+1][0]);
                char write_dsc[10];
                sprintf(write_dsc, "%d", pipe_dsc[n+i][1]);
                fprintf(stderr, "Startuje execl z argumentami : %s, %s, %s, %s, %s, %s\n", subprocess_cmd,
                        subprocess, n_str, i_str, read_dsc, write_dsc);
                execl(subprocess_cmd, subprocess, n_str, i_str, read_dsc, write_dsc, (char *) 0);
                syserr("Error in execl\n");

        } /* switch (fork ()) */
    }
/*
    char str[10] = "1\n";
    for (i = 0; i < n; ++i)
        fprintf(stderr, "Jestem procesem macierzystym i wysylam '1' po raz %d\n", i);
        write(pipe_dsc[0][1], str, 10);
*/
    for (i = 0; i < n; ++i) {
        if (wait (0) == -1) 
            syserr("wait");
    }
    exit (0);
}
