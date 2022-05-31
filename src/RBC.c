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

#define MAPPA1 "MAPPA1"
#define MAPPA2 "MAPPA2"
#define ETCS1 "ETCS1"
#define ETCS2 "ETCS2"
#define RBC "RBC"
#define TRENI_MAPPA1 4
#define TRENI_MAPPA2 5
#define DEFAULT_PROTOCOL 0

char* checkAutorizzazione(int buffer[3], int socket_client, int MAs[16], int Stazioni[8], int itinerari[5][8])
{
    time_t date;
    date = time(NULL);

    int auth[1] = {1};
    char* recordLog;
    int idTreno = buffer[0] - 1;
    int posizioneSuccessiva = buffer[1];
                
    int binAttuale = itinerari[idTreno][posizioneSuccessiva-1]-1;
    int binSuccessivo = itinerari[idTreno][posizioneSuccessiva]-1;

    date = time(NULL);
    if(itinerari[idTreno][posizioneSuccessiva+1]==-1)
    {
        //sono nell'ultimo binario, accedo alla stazione
        MAs[binAttuale] = 0;
        Stazioni[binSuccessivo]++;
        //printf("[TRENO RICHIEDENTE AUTORIZZAZIONE: T%d],[SEGMENTO ATTUALE: MA%d],[SEGMENTO RICHIESTO: S%d],[AUTOIZZATO: SI], %s",idTreno+1,binAttuale+1,binSuccessivo+1,ctime(&date));
        sprintf(recordLog, "[TRENO RICHIEDENTE AUTORIZZAZIONE: T%d],[SEGMENTO ATTUALE: MA%d],[SEGMENTO RICHIESTO: S%d],[AUTOIZZATO: SI], %s",idTreno+1,binAttuale+1,binSuccessivo+1,ctime(&date));
    }
    else 
    {
        if(posizioneSuccessiva == 1 && MAs[binSuccessivo] == 0)
        {
            //sono in una stazione attualmente, la lascio e entronel segmento successivo
            Stazioni[binAttuale]--;
            MAs[binSuccessivo] = 1;
            //printf("[TRENO RICHIEDENTE AUTORIZZAZIONE: T%d],[SEGMENTO ATTUALE: S%d],[SEGMENTO RICHIESTO: MA%d],[AUTOIZZATO: SI], %s",idTreno+1,binAttuale+1,binSuccessivo+1,ctime(&date));
            sprintf(recordLog, "[TRENO RICHIEDENTE AUTORIZZAZIONE: T%d],[SEGMENTO ATTUALE: S%d],[SEGMENTO RICHIESTO: MA%d],[AUTOIZZATO: SI], %s",idTreno+1,binAttuale+1,binSuccessivo+1,ctime(&date));
        }
        else if(MAs[binSuccessivo] == 0)
        {
            //situazione da binario a binario
            MAs[binAttuale] = 0;
            MAs[binSuccessivo] = 1;
            //printf("[TRENO RICHIEDENTE AUTORIZZAZIONE: T%d],[SEGMENTO ATTUALE: MA%d],[SEGMENTO RICHIESTO: MA%d],[AUTOIZZATO: SI], %s",idTreno+1,binAttuale+1,binSuccessivo+1,ctime(&date));
            sprintf(recordLog, "[TRENO RICHIEDENTE AUTORIZZAZIONE: T%d],[SEGMENTO ATTUALE: MA%d],[SEGMENTO RICHIESTO: MA%d],[AUTOIZZATO: SI], %s",idTreno+1,binAttuale+1,binSuccessivo+1,ctime(&date));
        }
        else
        { 
            auth[0] = 0; 
            //printf("[TRENO RICHIEDENTE AUTORIZZAZIONE: T%d],[SEGMENTO ATTUALE: MA%d],[SEGMENTO RICHIESTO: MA%d],[AUTOIZZATO: NO], %s",idTreno+1,binAttuale+1,binSuccessivo+1,ctime(&date));
            sprintf(recordLog, "[TRENO RICHIEDENTE AUTORIZZAZIONE: T%d],[SEGMENTO ATTUALE: MA%d],[SEGMENTO RICHIESTO: MA%d],[AUTOIZZATO: NO], %s",idTreno+1,binAttuale+1,binSuccessivo+1,ctime(&date));
        }
        write(socket_client, auth, 4);
    }

    printf(" ...invio dei dati al treno %d avvenuto.\n",buffer[0]);
    close(socket_client);

    return recordLog;
}

void stampa(int MAs[16], int Stazioni[8])
{
    printf("--------------------------------\n");
    for(int i=0;i<8;i++)
    {
        printf("Stazione[%d] = %d\n",i, Stazioni[i]);
    }
    printf("--------------------------------\n");
    for(int i=0;i<16;i++)
    {
        printf("MAs[%d] = %d\n",i, MAs[i]);
    }
    printf("--------------------------------\n");
}

void RBCclient(int mappa, int itinerari[5][8])
{
    int clientRBCFd, serverLen, connessione;
    struct sockaddr_un indirizzoServer;
    struct sockaddr* serverSockAddrPtr;

    serverSockAddrPtr = (struct sockaddr*) &indirizzoServer;
    serverLen = sizeof (indirizzoServer);

    char arg[3] = {'0', ' ' , '0'};

    for(int i = 0; i < (mappa+3); i++)
    {
        clientRBCFd = socket (AF_UNIX, SOCK_STREAM, DEFAULT_PROTOCOL);
        indirizzoServer.sun_family = AF_UNIX;
        strcpy (indirizzoServer.sun_path, "serverRegistro");
        do
        {
            printf("Tento la connessione al server registro...\n");
            connessione = connect (clientRBCFd, serverSockAddrPtr, serverLen);
            if(connessione == -1) sleep(1);
        }while(connessione == -1);
        
        arg[0] = (i+1) + '0';
        arg[2] = mappa + '0';
        
        write(clientRBCFd, arg, 3);
        printf("Invio dati al server registro avvenuta... \n");
        read(clientRBCFd, itinerari[i], 32);  
        printf("Ricezione dati dal server registro avvenuta... \n");     
    }

    close(clientRBCFd);
}

void RBCserver(int mappa, int itinerari[5][8])
{
    int logFd;
    char fileLog[16]={"../log/RBC.log"};
    //sprintf (fileLog,"./log/RBC.log");  // ogni treno crea il proprio file di log nella directory log
    umask(000);
    remove(fileLog);
    logFd = open(fileLog,O_RDWR|O_CREAT, 0666);

    //char recordLog[200] = {0};
    char * recordLog;

    int socket_server; // Variabile che contiene il descrittore per il socket che andremo a creare
    int socket_client, len; // Socket del client e dimensione della struttura del socket
    struct sockaddr_un serverRBC; // Struttura che contiene i dettagli del server
    struct sockaddr_un client;
    int *risposta;
    int buffer[3];

    socket_server = socket (AF_UNIX, SOCK_STREAM, DEFAULT_PROTOCOL);
    if (socket_server == -1) 
    {
        printf ("Errore di creazione socket.\n");
    }

    serverRBC.sun_family = AF_UNIX;
    strcpy(serverRBC.sun_path, "serverRBC");
    unlink("serverRBC");
    bind (socket_server, (struct sockaddr *)&serverRBC, sizeof (serverRBC));

    listen (socket_server, 15);
    printf ("In ascolto.\n");

    //Strutture dati RBC
    int MAs[16] = {0};
    int Stazioni[8] = {0};

    for(int i=0;i<mappa+3; i++)
    {
        Stazioni[itinerari[i][0]-1] = 1;
    }

    while(1)
    {
        //socket_server = socket (AF_UNIX, SOCK_STREAM, DEFAULT_PROTOCOL);
        //printf("SOno nel while\n");
        int clientLen = sizeof(client);
        socket_client = accept(socket_server, (struct sockaddr *)&client, &clientLen);
        if(fork() == 0)
        {
            printf ("Connessione in arrivo.\n");

            read(socket_client, buffer, 8);
            printf("Ricezione dati dal treno %d avvenuta... [%d]",buffer[0],buffer[1]);
                        
            recordLog = checkAutorizzazione(buffer,socket_client,MAs,Stazioni,itinerari);

            //stampa(MAs,Stazioni);
            write(logFd, recordLog, strlen(recordLog));
            exit(0);
        }
        else 
        {
            close(socket_client);
        }
        
    }
    close(logFd);
    close(socket_client);
    close(socket_server);
    printf("Chiusura canale di comunicazione.\n");
    unlink("serverRBC");
}

int main(int argc, char* argv[])
{
    /*RBC CLIENT*/
    int mappa = 0;
    if (strcmp(argv[1],MAPPA1)==0)
        mappa = 1;
    else mappa = 2;

    int itinerari[5][8] ={0};

    RBCclient(mappa,itinerari);
    /*RBC SERVER*/
    RBCserver(mappa,itinerari);

    exit(0);
    return 0;
}
