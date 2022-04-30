#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <string.h>

int main()
{
    int fd;
    char dirName[16] = "directoryMA";
    system("rm -rf directoryMA"); // se esiste di già
    char s[20]="./directoryMA/MA";
    mkdir(dirName, 0777);
    for(int i=0;i<16;i++)
    {
        sprintf (s,"./directoryMA/MA%02d",i+1);
        fd = open(s,O_RDWR|O_CREAT, 0666);
        write(fd,"0",1);
    }
    return 0;
}