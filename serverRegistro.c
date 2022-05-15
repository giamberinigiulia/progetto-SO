#include <stdio.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h> /* For AF_UNIX sockets */

char *itinerario(char buffer[100]) //si può cambiare in due char per ottenere id_treno e id_mappa
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
    int treno = buffer[0] - '0';//atoi(buffer[0]); //trasformo in intero l'id del treno
    int mappa = buffer[2] - '0';//atoi(buffer[2]); //trasformo in intero l'id della mappa
    //printf("treno: %d\nmappa: %d\n",treno,mappa);
    char *itinerario_selezionato = mappe[mappa-1][treno-1]; //seleziono percorso del treno x nella mappa y
    printf("Itinerario: %s\n",itinerario_selezionato);
    return itinerario_selezionato;
}

int main (int argc, char *argv[]) {

int socket_descrittore; // Variabile che contiene il descrittore per il socket che andremo a creare
int socket_client, len; // Socket del client e dimensione della struttura del socket
struct sockaddr_un mio_server; // Struttura che contiene i dettagli del server
struct sockaddr_un client;
char *risposta;
char buffer_ricezione[100];

socket_descrittore = socket (AF_UNIX, SOCK_STREAM, 0);
if (socket_descrittore == -1) {
printf ("Errore di creazione socket.\n");
return 1;
}

mio_server.sun_family = AF_UNIX;
strcpy(mio_server.sun_path, "socketRegistro");
unlink("socketRegistro");
bind (socket_descrittore, (struct sockaddr *)&mio_server, sizeof (mio_server));

listen (socket_descrittore, 1);
printf ("In ascolto.\n");
while(1)
{
    int clientLen = sizeof(client);
    socket_client = accept(socket_descrittore, (struct sockaddr *)&client, &clientLen);
    if(fork() == 0)
    {
        printf ("Connessione in arrivo.\n");

        read(socket_client, buffer_ricezione, 100);
        printf("Ricezione: %s\n",buffer_ricezione);

        risposta = itinerario(buffer_ricezione);

        write(socket_client, risposta, strlen (risposta));
        printf ("Invio della risposta: %s\n", risposta);

        close(socket_client);
        exit(0);
    }
    else close(socket_client);

}
return 0;
}