#define _POSIX_C_SOURCE (200809L)
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>

#define checkError(cond, usrMsg)\
    do{\
        if(!(cond)){\
            exit(EXIT_FAILURE);\
        }\
    }while(0)

extern char** environ;

#define MAX_SIZE (1024)

#define RD_END (0)
#define WR_END (1)

int main(int argc, char** argv){

    checkError(argc == 1, "...");

    char* fajl = getenv("REG_FAJL");
    checkError(fajl != NULL, "getenv");

    int childToPar[2];
    checkError(pipe(childToPar) != -1, "...");

    pid_t childPid = fork();

    if(childPid > 0){ // r
        checkError(wait(NULL) != -1, "...");

        close(childToPar[WR_END]);

        FILE* f = fdopen(childToPar[RD_END], "r");
        checkError(f != NULL, "...");

        char* buffer = NULL;
        int max = 0;
        size_t n = MAX_SIZE;
        while(getline(&buffer, &n, f) != -1){
            int size = strlen(buffer);
            if(size > max){
                max = size;
            }
        }

        fclose(f);
        free(buffer);
        close(childToPar[RD_END]);

        printf("%d\n", max);
    }
    else if(childPid == 0){ // d
        close(childToPar[RD_END]);

        checkError(dup2(childToPar[WR_END], STDOUT_FILENO) != -1, "...");
        checkError(execlp("cat", "cat", fajl, NULL) != -1, "...");
    }

    exit(EXIT_SUCCESS);
}