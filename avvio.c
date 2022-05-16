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

#define MAPPA1 "MAPPA1"
#define MAPPA2 "MAPPA2"
#define ETCS1 "ETCS1"
#define ETCS2 "ETCS2"
#define RBC "RBC"
#define TRENI_MAPPA1 4
#define TRENI_MAPPA2 5
#define DEFAULT_PROTOCOL 0

void error()
{
    printf("\nErrore nell'inserimento dei parametri\n");
}

char *itinerario(char buffer[100]) //si può cambiare in due char per ottenere id_treno e id_mappa
{
    char *mappe[2][5]=
    {
        {
            "1 1-2-3-8 6",
            "2 5-6-7-3-4 5",
            "7 13-12-11-10-9 3",
            "4 14-15-16-12 8"
        },
        {
            "2 5-6-7-3-8 6",
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

int registro()
{
    int socket_descrittore; // Variabile che contiene il descrittore per il socket che andremo a creare
    int socket_client, len; // Socket del client e dimensione della struttura del socket
    struct sockaddr_un mio_server; // Struttura che contiene i dettagli del server
    struct sockaddr_un client;
    char *risposta;
    char buffer_ricezione[100];

    socket_descrittore = socket (AF_UNIX, SOCK_STREAM, DEFAULT_PROTOCOL);
    if (socket_descrittore == -1) 
    {
        printf ("Errore di creazione socket.\n");
        return 1;
    }

    mio_server.sun_family = AF_UNIX;
    strcpy(mio_server.sun_path, "registroTreni");
    unlink("registroTreni");
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

int treno(int id, int mappa)    // funzione che rappresenta i processi treni: prende come parametri l'id del treno e il tipo di mappa selezionata
{
    int trenoFd, serverLen, connessione;
    struct sockaddr_un indirizzoServer;
    struct sockaddr* serverSockAddrPtr;

    serverSockAddrPtr = (struct sockaddr*) &indirizzoServer;
    serverLen = sizeof (indirizzoServer);
    trenoFd = socket (AF_UNIX, SOCK_STREAM, DEFAULT_PROTOCOL);
    indirizzoServer.sun_family = AF_UNIX;
    strcpy (indirizzoServer.sun_path, "registroTreni");
    do
    {
        connessione = connect (trenoFd, serverSockAddrPtr, serverLen);
        if (connessione == -1)
        {
            printf("Connessione non riuscita: riprovo in 1 secondo\n");
            sleep (1);
        }
    } while (connessione == -1);
    char arg[3] = {id + '0', ' ', mappa + '0'};
    write(trenoFd, arg, 3);
    char itinerario[20] = {0};  // inizializzo l'area di memoria a 0
    read(trenoFd, itinerario, 100);
    printf("Risposta per il treno %d: %s\n", id, itinerario);
    close(trenoFd);
    return 0;
}

int creazione_treni(int numTreni, int mappa)   // funzione che crea i processi treni in base alla mappa selezionata
{
    for(int i = 1; i <= numTreni; i++)
    {
        if(fork() == 0)
        {
            treno(i, mappa);   // ad ogni treno viene passato il proprio id
            exit(0);
        }
    }
    return 0;
}

int padre_treni(char *mappa)    // funzione che crea i processi treni: prende come argomento la mappa selezionata
{
    int fd;
    char dirName[16] = "directoryMA";
    system("rm -rf directoryMA"); // se esiste di già
    char s[20]="./directoryMA/MA";
    umask(000);
    mkdir(dirName, 0777);
    for(int i=0;i<16;i++)
    {
        sprintf (s,"./directoryMA/MA%02d",i+1);
        fd = open(s,O_RDWR|O_CREAT, 0666);
        write(fd,"0",1);
    }
    if(strcmp(mappa, MAPPA1) == 0)    // se la mappa selezionata e' MAPPA1, il padre crea 4 figli (treni)
    {
        creazione_treni(TRENI_MAPPA1, 1);
    }
    else    // la mappa selezionata e' MAPPA2, il padre crea 5 figli (treni)
    {
        creazione_treni(TRENI_MAPPA2, 2);
    }
    return 0;
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