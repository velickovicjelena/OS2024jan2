#define _XOPEN_SOURCE 700
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <grp.h>
#include <pwd.h>

#define checkError(cond, usrMsg)\
    do{\
        if(!(cond)){\
            exit(EXIT_FAILURE);\
        }\
    }while(0)

int main(int argc, char** argv){

    checkError(argc == 2, "args");

    struct stat fInfo;
    checkError(lstat(argv[1], &fInfo) != -1, "stat");

    struct group* grp = getgrgid(fInfo.st_gid);
     checkError(grp != NULL, "getgrnam");

    struct passwd* usr = getpwuid(fInfo.st_uid);
    checkError(usr != NULL, "...");

    printf("%s %s\n", grp->gr_name, usr->pw_name);

    exit(EXIT_SUCCESS);
}