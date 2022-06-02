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
#include <time.h>

#include "registro.h"

#define MAPPA1 "MAPPA1"
#define DEFAULT_PROTOCOL 0


void itinerario(char buffer[100], int socket_client)
{
    int mappa1 [4][8]=
    {
        {1,1,2,3,8,6,-1},
        {2,5,6,7,3,8,6,-1},
        {7,13,12,11,10,9,3,-1}, 
        {4,14,15,16,12,8,-1}
    };
    int mappa2 [5][8]=
    {
        {2,5,6,7,3,8,6,-1},
        {3,9,10,11,12,8,-1}, 
        {4,14,15,16,12,8,-1},
        {6,8,3,2,1,1,-1},
        {5,4,3,2,1,1,-1}
    };
    int treno = buffer[0] - '0';//atoi(buffer[0]); //trasformo in intero l'id del treno
    int mappa = buffer[2] - '0';//atoi(buffer[2]); //trasformo in intero l'id della mappa
    if(mappa == 1)
        write(socket_client, mappa1[treno-1], 32);
    else 
        write(socket_client, mappa2[treno-1], 32);
    close(socket_client);
    exit(0);
}

int registro(char *inputMappa)
{
    int countTreni = 0;
    if(strcmp(inputMappa, MAPPA1) == 0)    // se la mappa selezionata e' MAPPA1, il padre crea 4 figli (treni)
        countTreni = 4;
    else countTreni = 5; //mappa = 2;

    int socket_server; // Variabile che contiene il descrittore per il socket che andremo a creare
    int socket_client, len; // Socket del client e dimensione della struttura del socket
    struct sockaddr_un mio_server; // Struttura che contiene i dettagli del server
    struct sockaddr_un client;
    int *risposta;
    char buffer_ricezione[100];

    socket_server = socket (AF_UNIX, SOCK_STREAM, DEFAULT_PROTOCOL);
    if (socket_server == -1) 
    {
        printf ("Errore di creazione socket.\n");
        return 1;
    }

    mio_server.sun_family = AF_UNIX;
    strcpy(mio_server.sun_path, "serverRegistro");
    unlink("serverRegistro");
    bind (socket_server, (struct sockaddr *)&mio_server, sizeof (mio_server));

    listen (socket_server, 10);
    printf ("In ascolto.\n");


    while(1)
    {
        int clientLen = sizeof(client);
        socket_client = accept(socket_server, (struct sockaddr *)&client, &clientLen);
        if(fork() == 0)
        {
            printf ("Connessione in arrivo.\n");

            read(socket_client, buffer_ricezione, 100);
            printf("Ricezione: %s\n",buffer_ricezione);
            itinerario(buffer_ricezione, socket_client);
            exit(0);
        }
        else 
        {
            close(socket_client);
        }
    }

    close(socket_client);
    close(socket_server);
    unlink("serverRegistro");
    return 0;
}