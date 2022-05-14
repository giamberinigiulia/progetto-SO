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
            "2 5-6-7-3-8 6",
            "7 13-12-11-10-9 3",
            "4 14-15-16-12 8"
        },
        {
            "1 5-6-7-3-8 6",
            "3 9-10-11-12 8",
            "4 14-15-16-12 8",
            "6 8-3-2-1 1",
            "5 4-3-2-1 1"
        },
    }; //array bidimensionale che contiene le mappe in stringa
    printf("Il treno %s ha selezionato la mappa %s\n",argvs[1],argvs[2]);
    int treno = atoi(argvs[1]); //trasformo in intero l'id del treno
    int mappa = atoi(argvs[2]); //trasformo in intero l'id della mappa
    char *itinerario_selezionato = mappe[mappa-1][treno-1]; //seleziono percorso del treno x nella mappa y
    printf("Itinerario: %s\n",itinerario);
}
