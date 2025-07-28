#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <utime.h>

#define checkError(cond, usrMsg)\
    do{\
        if(!(cond)){\
            fprintf(stderr, "Greska: %s\n", usrMsg);\
            exit(EXIT_FAILURE);\
        }\
    }while(0)
    
int main(int argc, char** argv){

    checkError(argc == 1, "...");

    char* buf;
    checkError((buf = getcwd(NULL, 0)) != NULL, "...");

    DIR* dir = opendir(buf);
    checkError(dir != NULL, "...");

    struct dirent* dirEntry = NULL;
    time_t aTime = 0, mTime = 0;

    while((dirEntry = readdir(dir)) != NULL){
        struct stat fInfo;
        checkError(stat(dirEntry->d_name, &fInfo) != -1, "...");
        if(fInfo.st_atime > aTime){
            aTime = fInfo.st_atime;
        }
        if(fInfo.st_mtime > mTime){
            mTime = fInfo.st_mtime;
        }
    }

    struct utimbuf utim;
    utim.actime = aTime;
    utim.modtime = mTime;

    checkError(closedir(dir) != -1, "...");
    dir = opendir(".");
    checkError(dir != NULL, "...");
    dirEntry = NULL;
    
    while((dirEntry = readdir(dir)) != NULL)
    {
        checkError(utime(dirEntry->d_name, &utim) != -1, "...");
    }
    checkError(closedir(dir) != -1, "...");

    exit(EXIT_SUCCESS);
}