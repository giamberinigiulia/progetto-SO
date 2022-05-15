#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h> /* For AF_UNIX sockets */

int main (int argc, char *argv[]) 
{

    int socket_descrittore; // Variabile che contiene il descrittore per il socket che andremo a creare
    struct sockaddr_un server_destinazione; // Struttura che contiene i parametri di connessione al server
    char *messaggio = "2 2"; //deve contenere id_treno e id_mappa
    char risposta[100];

    socket_descrittore = socket (AF_UNIX, SOCK_STREAM, 0);
    if (socket_descrittore == -1) {
        printf ("Errore di creazione socket.\n");
        return 1;
    }

    server_destinazione.sun_family = AF_UNIX;
    strcpy(server_destinazione.sun_path,"socketRegistro");
    connect(socket_descrittore, (struct sockaddr*)&server_destinazione, sizeof(server_destinazione));
    printf ("Connesso al server. Invio messaggio...\n");
    write(socket_descrittore, messaggio, strlen (messaggio));
    printf("Invio: %s\n",messaggio);

    read(socket_descrittore, risposta, 100);

    printf ("Risposta: %s\n", risposta);
    close (socket_descrittore);
    return 0;
}
