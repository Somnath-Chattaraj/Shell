#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h> 
#include <stdlib.h>

int main() {
    printf("Starting Shell \n");
    while(1) {
        char *line = NULL;
        size_t len = 0;
        printf("myshell> ");
        if (getline(&line, &len, stdin) == -1) {free(line); printf("\n"); exit(0);}
        if (strcmp(line,"exit\n") == 0) {
            free(line);
            exit(0);
        }

        char *args;
        char *my_args[64];
        args = strtok(line," \n");
        int i = 0;
        while(args != NULL) {
            my_args[i] = args;
            i++;
            args = strtok(NULL," \n");
        }
        my_args[i] = NULL;
        if (my_args[0] == NULL) {
            free(line);
            continue;
        }
        if (strcmp(my_args[0], "cd") == 0) {
            if (my_args[1] == NULL) {
                chdir(getenv("HOME"));
            } else {
                if (chdir(my_args[1]) != 0) {
                    perror("cd");
                }
            }
        }
        else{
            int pid = fork();
            if (pid < 0) {
                perror("Error!!!");
                exit(1);
            }
            if (pid == 0) {
                if (execvp(my_args[0],my_args)) {
                    perror("Command failed");
                    exit(1);
                }
            } else if (pid > 0) {
                wait(NULL);
            }
        }

    }
}