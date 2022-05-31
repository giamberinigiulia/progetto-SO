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

#include "logicaTreni.h"

#define DEFAULT_PROTOCOL 0

// funzione che controlla se i segmenti sono liberi (contenuto del file MA = 0) e scrittura nel file di log
int viaggio(int itinerario[20], int logFd, int *fdMaPrecedente, int i)
{
    char fileMa[22];
    char flagFile[1];
    char recordLog[60] = {0};
    int fdMa;
    time_t date;
    date = time(NULL);

    sprintf (fileMa,"../directoryMA/MA%02d",itinerario[i]);
    fdMa = open(fileMa, O_RDWR);
    read(fdMa, flagFile, 1);
    if(flagFile[0] == '0')  // binario libero
    {
        flagFile[0] = '1';
        lseek(fdMa, SEEK_SET, 0);
        write(fdMa, flagFile, 1);
        if(i != 1)  // se non e' il primo binario acceduto, libero il binario precedente
        {
            flagFile[0] = '0';
            lseek(*fdMaPrecedente, SEEK_SET, 0);
            write(*fdMaPrecedente, flagFile, 1);
            close(*fdMaPrecedente);
        }
        if(itinerario[i+2] == -1)   // se la prossima tappa e' l'ultima stazione modifico il testo del recordLod
            sprintf(recordLog, "[ATTUALE: MA%d], [NEXT: S%d], %s", itinerario[i], itinerario[i+1], ctime(&date));
        else
            sprintf(recordLog, "[ATTUALE: MA%d], [NEXT: MA%d], %s", itinerario[i], itinerario[i+1], ctime(&date));
        write(logFd, recordLog, strlen(recordLog));
        i++;
        *fdMaPrecedente = dup(fdMa);
        close(fdMa);
    }
    else    // binario occupato
    {
        sprintf(recordLog, "[ATTUALE: MA%d], [NEXT: MA%d], %s", itinerario[i-1], itinerario[i], ctime(&date));
        write(logFd, recordLog, strlen(recordLog));
    }
    //printf("I prima del ritorno da Viaggio: %d\n",i);
    return i;
}

// funzione che si occupa della comunicazione tra il treno e il registro
void trenoRegistro(int id, int mappa, int itinerario[20])
{
    int trenoFd, serverLen, connessione, logFd;
    struct sockaddr_un indirizzoServer;
    struct sockaddr* serverSockAddrPtr;

    serverSockAddrPtr = (struct sockaddr*) &indirizzoServer;
    serverLen = sizeof (indirizzoServer);
    trenoFd = socket (AF_UNIX, SOCK_STREAM, DEFAULT_PROTOCOL);
    indirizzoServer.sun_family = AF_UNIX;
    strcpy (indirizzoServer.sun_path, "serverRegistro");
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
    read(trenoFd, itinerario, 32);

    int i = 0;
    printf("Risposta per il treno %d: ", id);
    while(itinerario[i] != -1)
    {
        printf("%d ", itinerario[i]);
        i++;
    }
    printf("\n");
    close(trenoFd);
}

// funzione per l'inserimento del primo record nel file di log
int inizializzazioneLogFile(int id, int itinerario[20])
{
    int logFd;
    char fileLog[16];
    sprintf (fileLog,"../log/T%1d.log",id);  // ogni treno crea il proprio file di log nella directory log
    umask(000);
    logFd = open(fileLog,O_RDWR|O_CREAT, 0666);

    char recordLog[60] = {0};
    time_t date;
    date = time(NULL);
    sprintf(recordLog, "[ATTUALE: S%d], [NEXT: MA%d], %s", itinerario[0], itinerario[1], ctime(&date));
    write(logFd, recordLog, strlen(recordLog));
    return logFd;
}

// funzione che aggiorna il file Ma corrispondente all'ultimo binario (viene reimpostato a 0 = libero) e scrittura dell'ultimo record nel file di log
void rilascioUltimoBinario(int logFd, int fdMaPrecedente, int itinerario[20], int i)
{
    char flagFile[1] = {'0'};
    char recordLog[60] = {0};
    lseek(fdMaPrecedente, SEEK_SET, 0);
    write(fdMaPrecedente, flagFile, 1);
    close(fdMaPrecedente);
    time_t date;
    date = time(NULL);
    sprintf(recordLog, "[ATTUALE: S%d], [NEXT: --], %s", itinerario[i], ctime(&date));
    write(logFd, recordLog, strlen(recordLog));
    close(logFd);
}