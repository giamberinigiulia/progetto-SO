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

int main(int argc, char* argv[])
{
    int mappa = 0;
    if (strcmp(argv[1],"MAPPA1")==0)
        mappa = 1;
    else mappa = 2;

    int clientRBCFd, serverLen, connessione, logFd;
    struct sockaddr_un indirizzoServer;
    struct sockaddr* serverSockAddrPtr;

    serverSockAddrPtr = (struct sockaddr*) &indirizzoServer;
    serverLen = sizeof (indirizzoServer);

    int itinerari[5][8] ={0};
    
    char arg[3] = {'0', ' ' , '0'};
    int itinerario[20] = {0};
    for(int i = 0; i<20;i++)
    {
        printf("%d ",itinerario[i]);
    }
    printf("\n");
    for(int i = 0; i < 4 ; i++)
    {
        clientRBCFd = socket (AF_UNIX, SOCK_STREAM, DEFAULT_PROTOCOL);
        indirizzoServer.sun_family = AF_UNIX;
        strcpy (indirizzoServer.sun_path, "serverRegistro");
        connessione = connect (clientRBCFd, serverSockAddrPtr, serverLen);
        
        arg[0] = (i+1) + '0';
        arg[2] = mappa + '0';
        
        write(clientRBCFd, arg, 3);
        read(clientRBCFd, itinerari[i], 32);    
        /*
        for(int j=0;itinerario[j]!=0;j++)
        {
            itinerari[i][j] = itinerario[j];
        }
        for(int k = 0; k <10 ; k++)
        {
            printf("%d ",itinerari[i][k]);
            if(itinerari[i][k] == -1) k = 10;
        }
        printf("\n");*/
    }
    close(clientRBCFd);

    /*for(int i=0;i<4;i++)
    {
        int k=0;
        while(itinerari[i][k]!=-1)
        {
            printf("%d ", itinerari[i][k]);
            k++;
        }
        printf("\n----------------------------------------------------\n");
    }*/
}