#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAPPA1 "MAPPA1"
#define MAPPA2 "MAPPA2"
#define ETCS1 "ETCS1"
#define ETCS2 "ETCS2"
#define RBC "RBC"

void error()
{
    printf("\nErrore nell'inserimento dei parametri\n");
}


int main(int argc, char *param[])
{
    char *mappaSelezionata;
    if(argc<3 || argc>4) error();
    else if(strcmp(param[1],ETCS1)==0) //ETCS1
    {
        if(strcmp(param[2],MAPPA1)==0) //MAPPA1
        {
            mappaSelezionata = MAPPA1;
        }
        else if(strcmp(param[2],MAPPA2)==0) //MAPPA2
        {
            mappaSelezionata = MAPPA2;
        }
        if(fork() == 0)
        {
            registro();
        }
        else if(fork() == 0)
        {
            padre_treni(mappaSelezionata);
        }
    }
    else if(strcmp(param[1],ETCS2)==0) //ETCS2
    {
        printf("Modalità %s\t", param[1]);
        if(strcmp(param[2],MAPPA1)==0) //MAPPA1
        {
            printf("\nMappa %s\n", param[2]);
        }
        else if(strcmp(param[2],MAPPA2)==0) //MAPPA2
        {
            printf("\nMappa %s\n", param[2]);
        }
        else if(strcmp(param[2],RBC)==0) //RBC
        {
            printf("%s", param[2]);
            if(argc<4) error();
            else if(strcmp(param[3],MAPPA1)==0) //MAPPA1
            {
                printf("\nMappa %s\n", param[3]);
            }
            else if(strcmp(param[3],MAPPA2)==0) //MAPPA2
            {
                printf("\nMappa %s\n", param[3]);
            }
            else
            {
                error();
            }
        }
        else
        { 
            error(); 
        }
    }
    else
    {
        error();
    }
    return 0;
}