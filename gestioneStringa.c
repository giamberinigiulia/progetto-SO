#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

int potenza(int n, int power)
{
    int result = 1;
    while(power > 0)
    {
        result = result * n;
        power = power - 1;
    }
    return result;
}

int main()
{
    char stringa_itinerario[20];
    int int_itinerario[20];
    for(int i = 0; i < 20; i++)   // inizializza l'area di memoria a \0
        stringa_itinerario[i] = '\0';
    for(int i = 0; i < 20; i++)   // inizializza l'area di memoria a \0
        int_itinerario[i] = 0;
    strcpy(stringa_itinerario,"1 1-2-3-4-10 8");
    int element = 0;

    for(int i=0;i<20;i++)
    {
        int c;
        if(stringa_itinerario[i] == '\0') i = 20;
        else if(stringa_itinerario[i] == '-' || stringa_itinerario[i] == ' ') 
        {  
            c = 0;
            element++;
        }
        else
        {
            c++;
            int_itinerario[element] = (int_itinerario[element] * potenza(10,c-1)) + stringa_itinerario[i] - 48;
        }
    }
    for(int i=0;i<=element;i++)
    {
        printf("%d\n",int_itinerario[i]);
    }
} 
