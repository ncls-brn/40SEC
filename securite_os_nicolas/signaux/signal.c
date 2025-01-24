#define _XOPEN_SOURCE 700 // request all POSIX features, even with -std=c11
#include <stdlib.h>       // for EXIT_SUCCESS, NULL, abort
#include <stdio.h>        // for getline
#include <time.h>         // for nanosleep 
#include <unistd.h>       // for getpid

pid_t other_pid = 0;

int main(void) {
    printf("This process's ID: %ld\n", (long) getpid());
    char *line = NULL; size_t line_length = 0;
    do {
        printf("Enter other process ID: ");
        if (-1 == getline(&line, &line_length, stdin)) {
            perror("getline");
            abort();
        }
    } while ((other_pid = strtol(line, NULL, 10)) == 0);
    free(line);
    return EXIT_SUCCESS;
}