#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <string.h>


#define MAPPA1 "MAPPA1"
#define TRENI_MAPPA1 4
#define TRENI_MAPPA2 5

int creazione_treni(int numTreni)   // funzione che crea i processi treni in base alla mappa selezionata
{
    for(int i = 1; i <= numTreni; i++)
    {
        if(fork() == 0)
        {
            treno(i);   // ad ogni treno viene passato il proprio id
            exit(0);
        }
    }
    return 0;
}

int main(int argc, char **argv)
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
    if(strcmp(argv[1], MAPPA1) == 0)    // se la mappa selezionata e' MAPPA1, il padre crea 4 figli (treni)
    {
        creazione_treni(TRENI_MAPPA1);
    }
    else    // la mappa selezionata e' MAPPA2, il padre crea 5 figli (treni)
    {
        creazione_treni(TRENI_MAPPA2);
    }
    return 0;
}