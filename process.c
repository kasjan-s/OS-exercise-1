#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "err.h"

#define BUF_SIZE 80


int main (int argc, char *argv[]) {
    if (argc < 5) {
        fatal("Usage: ./process <n> <i> <read_fd> <write_fd>\n");
        exit(0);
    }

//    fprintf(stderr, "Odebralem argumenty: %s, %s, %s, %s, %s\n", argv[0], argv[1], argv[2], argv[3], argv[4]);

    int n = atoi(argv[1]);
    int i = atoi(argv[2]);

    int read_dsc, write_dsc, buf_len;
    char buf [BUF_SIZE];

    read_dsc = atoi(argv[3]);
    write_dsc = atoi(argv[4]);


    int value = 0; 
    if (i == 0)
        value = 1;
    int j;
    
    for (j = 0; j < n - i; ++j) {    
        if (i != 0) {
//            fprintf(stderr, "Jestem procesem %d i oczekuje na input po raz %d\n", i, j);
            if (fgets(buf, 10, stdin) == NULL)
                syserr("Failed to read");
//            int len = read(0, buf, 10);
//            if (len == -1) {
//                syserr("Failed to read");
//            }
//            fprintf(stderr, "Jestem procesem %d i odczytalem '%s'\n", i, buf);
            int aux = atoi(buf);
            value += aux;
        }
        if (j != n - i - 1) {
//            fprintf(stderr, "Jestem procesem %d i wysylam output po raz %d, tresc %d\n", i, j, value);
            int len = sprintf(buf, "%d\n", value);
//            write(1, buf, len);
            printf("%d\n", value);
        }
    }

    fprintf(stderr, "PROCES %d: Moja ostateczna wartosc: %d\n", i, value);
    /*
    for (j = 0; j < n - i + 1; ++j) {
        int len = sprintf(buf, "%d\n", value);
        fprintf(stderr, "Jestem procesem %d i wysylam output na lewo po raz %d\n", i, j);
        write(write_dsc, buf, len);
        if (j != n - i) {
            fprintf(stderr, "Jestem procesem %d i oczekuje na input z prawej po raz %d\n", i, j);
            int len = read(read_dsc, buf, sizeof(buf));
            value = atoi(buf);
        }
    }
*/
    return 0;
}
