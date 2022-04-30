#include <stdio.h>
#include <stdlib.h>
#include <string.h>
void error()
{
    printf("Errore nell'inserimento dei parametri\n");
}


int main(int argc, char *param[])
{
    char *str[] = {"ETCS1","ETCS2","MAPPA1","MAPPA2","RBC"};
    if(argc<3 || argc>4) error();
    else if(strcmp(param[1],str[0])==0) //ETCS1
    {
        printf("Modalità %s\n", param[1]);
        if(strcmp(param[2],str[2])==0) //MAPAP1
        {
            printf("Mappa %s\n", param[2]);
        }
        else if(strcmp(param[2],str[3])==0) //MAPPA2
        {
            printf("Mappa %s\n", param[2]);
        }
    }
    else if(strcmp(param[1],str[1])==0) //ETCS2
    {
        printf("Modalità %s\t", param[1]);
        if(strcmp(param[2],str[2])==0) //MAPPA1
        {
            printf("\nMappa %s\n", param[2]);
        }
        else if(strcmp(param[2],str[3])==0) //MAPPA2
        {
            printf("\nMappa %s\n", param[2]);
        }
        else if(strcmp(param[2],str[4])==0) //RBC
        {
            printf("%s", param[2]);
            if(argc<4) error();
            else if(strcmp(param[3],str[2])==0) //MAPPA1
            {
                printf("\nMappa %s\n", param[3]);
            }
            else if(strcmp(param[3],str[3])==0) //MAPPA2
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