#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <semaphore.h>

#define checkError(cond, usrMsg)\
    do{\
        if(!(cond)){\
            fprintf(stderr, "%s\n", usrMsg);\
            exit(EXIT_FAILURE);\
        }\
    }while(0)

#define MAX_ARRAY (1024)

typedef struct{
    sem_t inDataReady;
    sem_t averageCalculated;
    float array[MAX_ARRAY];
    unsigned arrayLen;
    float average;
}Data;

void* getMemBlock(char* path, unsigned* size){

    int memFd = shm_open(path, O_RDWR, 0);
    checkError(memFd != -1, "shm_open");

    struct stat fInfo;
    checkError(fstat(memFd, &fInfo) != -1, "stat");

    *size = fInfo.st_size;

    void* addr = mmap(NULL, *size, PROT_READ | PROT_WRITE, MAP_SHARED, memFd, 0);
    checkError(addr != MAP_FAILED, "mmap");

    return addr;
}

int main(int argc, char** argv){

    checkError(argc == 2, "...");

    unsigned size = 0;
    Data* niz = getMemBlock(argv[1], &size);
    checkError(niz != NULL, "...");

    int n = niz->arrayLen;
    float sum = 0;

    checkError(sem_wait(&niz->inDataReady) != -1, "sem");

    for(int i = 0; i < n; i++){
        sum += niz->array[i];
    }
    niz->average = sum / n;
    printf("%.1f\n", niz->average);

    checkError(sem_post(&niz->inDataReady) != -1, "...");

    checkError(munmap(niz, size) != -1, "munmap");

    exit(EXIT_SUCCESS);
}