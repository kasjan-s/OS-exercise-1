#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "err.h"

#define BUF_SIZE 1024


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

    int j;

    for (j = 0; j < n - i; ++j) {    

        if (i != 0) {
            if (fscanf(stdin, "%s", buf) < 0)
                syserr("Process %d failed to read a value\n", i+1);
            int aux = atoi(buf);
            value += aux;
        }

        if (j != n - i - 1) {
            printf("%lld\n", value);
            if (fflush(stdout) != 0)
                syserr("Flushing failed");
        }
    }

    if (i == n-1) {
        int len = sprintf(buf, "%lldE", value);
        write(write_dsc, buf, len);

        return 0;
    } else {
        int len = sprintf(buf, "%lld ", value);
        write(write_dsc, buf, len);

        while(1) {
            int len = read(read_dsc, buf, BUF_SIZE);
            if (len == -1)
                syserr("Process %d failed to read a value from %d\n", i, i+1);
            int len2 = write(write_dsc, buf, len);
            if (len2 == -1)
                syserr("Process %d failed to write to %d\n", i, i-1);
            if (buf[len - 1] == 'E') {
                break;
            }
        }
    }

    return 0;
}
