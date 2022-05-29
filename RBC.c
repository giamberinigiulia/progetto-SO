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

/*void checkAutorizzazione(int buffer[3], int socket_client, int* MAs[16], int* Stazioni[8], int* itinerari[5][8])
{
    int auth[1] = {1};
    int idTreno = buffer[0] - 1;
    int posizioneSuccessiva = buffer[1];
    int binAttuale = itinerari[idTreno][posizioneSuccessiva-1]-1;
    int binSuccessivo = itinerari[idTreno][posizioneSuccessiva]-1;
    printf("Segmento Attuale: %d\n", binAttuale);
    printf("Segmento Successivo: %d\n", binSuccessivo);
    
    if(posizioneSuccessiva == 1 && MAs[binSuccessivo] == 0)
    {
        //sono in una stazione attualmente, la lascio e entronel segmento successivo
        Stazioni[binAttuale]--;
        MAs[binSuccessivo] = 1;
    }
    else if(itinerari[idTreno][posizioneSuccessiva+1]==-1)
    {
        //sono nell'ultimo binario, accedo alla stazione
        MAs[binAttuale] = 0;
        Stazioni[binSuccessivo]++;
    }
    else if(MAs[binSuccessivo] == 0)
    {
        //situazione da binario a binario
        MAs[binAttuale] = 0;
        MAs[binSuccessivo] = 1;
    }
    else
    { 
        auth[0] = 0; 
    }
    stampa(MAs,Stazioni);

    write(socket_client, auth, 4);
    printf("Treno %d si trova nella posizione %d, ovvero nel segmento di binario %d e vuole andare nella posizione %d, cioè nel segmento %d\n",idTreno,posizioneSuccessiva-1, itinerari[idTreno][posizioneSuccessiva-1],posizioneSuccessiva, itinerari[idTreno][posizioneSuccessiva]);
    printf("--------------------------------\n");
    close(socket_client);
    exit(0);
}*/

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

int main(int argc, char* argv[])
{
    /*RBC CLIENT*/
    int mappa = 0;
    if (strcmp(argv[1],MAPPA1)==0)
        mappa = 1;
    else mappa = 2;

    int clientRBCFd, serverLen, connessione, logFd;
    struct sockaddr_un indirizzoServer;
    struct sockaddr* serverSockAddrPtr;

    serverSockAddrPtr = (struct sockaddr*) &indirizzoServer;
    serverLen = sizeof (indirizzoServer);

    int itinerari[5][8] ={0};
    
    char arg[3] = {'0', ' ' , '0'};
    //int itinerario[20] = {0};
    /*for(int i = 0; i < 20;i++)
    {
        printf("%d ",itinerario[i]);
    }*/
    //printf("\nMappa: %d\n",mappa);

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
    /*for(int i=0;i<(mappa+3);i++)
    {
        int k=0;
        while(itinerari[i][k]!=-1) 
        {
            printf("%d ",itinerari[i][k]);
            k++;
        }
        printf("\n");
    }
    printf("\nFINITO CLIENT RBC\n");*/
    
    close(clientRBCFd);


    /*RBC SERVER*/

    char fileLog[13]={"./RBC.log"};
    //sprintf (fileLog,"./log/RBC.log");  // ogni treno crea il proprio file di log nella directory log
    umask(000);
    remove(fileLog);
    logFd = open(fileLog,O_RDWR|O_CREAT, 0666);

    char recordLog[200] = {0};
    time_t date;
    date = time(NULL);

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
        return 1;
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

    /*for(int i=0;i<8;i++)
    {
        printf("Stazione[%d] = %d\n",i+1, Stazioni[i]);
    }
    printf("--------------------------------\n");

    for(int i=0;i<16;i++)
    {
        printf("Binario[%d] = %d\n",i+1, Stazioni[i]);
    }
    printf("--------------------------------\n");*/

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
                        
            //buffer[0] --> id
            //buffer[1] --> posizione successiva
            
            //checkAutorizzazione(buffer,socket_client, MAs, Stazioni, itinerari);

            int auth[1] = {1};
            int idTreno = buffer[0] - 1;
            int posizioneSuccessiva = buffer[1];
                        
            int binAttuale = itinerari[idTreno][posizioneSuccessiva-1]-1;
            int binSuccessivo = itinerari[idTreno][posizioneSuccessiva]-1;

            date = time(NULL);
            if(posizioneSuccessiva == 1 && MAs[binSuccessivo] == 0)
            {
                //sono in una stazione attualmente, la lascio e entronel segmento successivo
                Stazioni[binAttuale]--;
                MAs[binSuccessivo] = 1;
                //printf("[TRENO RICHIEDENTE AUTORIZZAZIONE: T%d],[SEGMENTO ATTUALE: S%d],[SEGMENTO RICHIESTO: MA%d],[AUTOIZZATO: SI], %s",idTreno+1,binAttuale+1,binSuccessivo+1,ctime(&date));
                sprintf(recordLog, "[TRENO RICHIEDENTE AUTORIZZAZIONE: T%d],[SEGMENTO ATTUALE: S%d],[SEGMENTO RICHIESTO: MA%d],[AUTOIZZATO: SI], %s",idTreno+1,binAttuale+1,binSuccessivo+1,ctime(&date));
            }
            else if(itinerari[idTreno][posizioneSuccessiva+1]==-1)
            {
                //sono nell'ultimo binario, accedo alla stazione
                MAs[binAttuale] = 0;
                Stazioni[binSuccessivo]++;
                //printf("[TRENO RICHIEDENTE AUTORIZZAZIONE: T%d],[SEGMENTO ATTUALE: MA%d],[SEGMENTO RICHIESTO: S%d],[AUTOIZZATO: SI], %s",idTreno+1,binAttuale+1,binSuccessivo+1,ctime(&date));
                sprintf(recordLog, "[TRENO RICHIEDENTE AUTORIZZAZIONE: T%d],[SEGMENTO ATTUALE: MA%d],[SEGMENTO RICHIESTO: S%d],[AUTOIZZATO: SI], %s",idTreno+1,binAttuale+1,binSuccessivo+1,ctime(&date));
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
            //stampa(MAs,Stazioni);
            write(logFd, recordLog, strlen(recordLog));
            write(socket_client, auth, 4);
            printf(" ...invio dei dati al treno %d avvenuto.\n",buffer[0]);
            //printf("Treno %d si trova nella posizione %d, ovvero nel segmento di binario %d e vuole andare nella posizione %d, cioè nel segmento %d\n",idTreno,posizioneSuccessiva-1, itinerari[idTreno][posizioneSuccessiva-1],posizioneSuccessiva, itinerari[idTreno][posizioneSuccessiva]);
            close(socket_client);
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
    exit(0);
    return 0;
}
