#include <stdio.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

int main(char argc, char *argvs[]) //funzione registro
{
    char *mappe[2][5]=
    {
        {
            "1 1-2-3-8 6",
            "da s2 a s5",
            "da s7 a s3",
            "da 24 a s8"
        },
        {
            "da s2 a s6",
            "da s3 a s8",
            "da s4 a s8",
            "da s6 a s1",
            "da s5 a s1"
        },
    }; //array bidimensionale che contiene le mappe in stringa
    printf("Il treno %s ha selezionato la mappa %s\n",argvs[1],argvs[2]);
    int treno = atoi(argvs[1]); //trasformo in intero l'id del treno
    int mappa = atoi(argvs[2]); //trasformo in intero l'id della mappa
    char *itinerario_selezionato = mappe[mappa-1][treno-1]; //seleziono percorso del treno x nella mappa y
    printf("Itinerario: %s\n",itinerario);
}
