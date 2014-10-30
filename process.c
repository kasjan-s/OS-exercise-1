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

    int read_dsc, write_dsc, buf_len;
    char buf [BUF_SIZE];

    read_dsc = atoi(argv[3]);
    write_dsc = atoi(argv[4]);


    int value = 0; 
    int j;
    char str[80];

    for (j = 0; j < n - i + 1; ++j) {    
        fgets(str, 80, stdin);
        int aux = atoi(str);
        value += aux;
        if (j != n - i) {
            printf("%d", value);
        }
    }
    
    for (j = 0; j < n - i + 1; ++j) {
        int len = sprintf(buf, "%d", value);
        buf[len] = '\n';
        write(write_dsc, buf, len);
        if (j != n - i) {
            int len = read(read_dsc, buf, sizeof(buf));
            value = atoi(buf);
        }
    }

    return 0;
}
