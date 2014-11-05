/*
 *  Autor: Kasjan Siwek
 *  Indeks: 306827
 *  Data: 02.11.2014
 */

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

    int n = atoi(argv[1]);
    int i = atoi(argv[2]);
    int read_dsc = atoi(argv[3]);
    int write_dsc = atoi(argv[4]);

    char buf[BUF_SIZE];

    unsigned long long value = 0; 
    if (i == 0)
        value = 1;

    // Wyliczenie wspolczynnika trojkata Pascala
    int j;
    for (j = 0; j < n - i; ++j) {
        // Odebranie poprzedniej wartosci od procesu W(i-1)
        if (i != 0) {
            long long int aux;
            if (fscanf(stdin, "%lld", &aux) < 0)
                syserr("Process %d failed to read a value\n", i+1);
            value += aux;
        }

        // Wyslanie wyliczonej wartosci do procesu W(i+1)
        if (j != n - i - 1) {
            printf("%lld\n", value);
            if (fflush(stdout) != 0)
                syserr("Flushing failed");
        }
    }
    
    // Przeslanie wyliczonej wartosci do procesu Pascal
    if (i == n-1) {
        int len = sprintf(buf, "%lldE", value);
        write(write_dsc, buf, len);

        return 0;
    } else {
        int len = sprintf(buf, "%lld ", value);
        write(write_dsc, buf, len);

        // Procesy musza tez przekazac odebrane wartosci dalej
        while(1) {
            int len = read(read_dsc, buf, BUF_SIZE);
            if (len == -1)
                syserr("Process %d failed to read a value from %d\n", i, i+1);
            int len2 = write(write_dsc, buf, len);
            if (len2 == -1)
                syserr("Process %d failed to write to %d\n", i, i-1);
            // Koniec danych
            if (buf[len - 1] == 'E') {
                break;
            }
        }
    }

    return 0;
}
