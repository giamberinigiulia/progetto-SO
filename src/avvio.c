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
#include "registro.h"

#define MAPPA1 "MAPPA1"
#define MAPPA2 "MAPPA2"
#define ETCS1 "ETCS1"
#define ETCS2 "ETCS2"
#define RBC "RBC"
#define TRENI_MAPPA1 4
#define TRENI_MAPPA2 5
#define DEFAULT_PROTOCOL 0

int countEndTreni = 0;
pid_t pidRegistro;
pid_t pidRBC;

void error() //gestione errore inserimento parametri
{
    printf("Errore nell'inserimento dei parametri\n");
}

void handler(int signalNum) //handler gestione segnali SIGUSR1
{
    countEndTreni++;
}

// funzione che rappresenta i processi treni in modalita' ETCS1
int trenoETCS1(int id, int mappa)
{
    int logFd;
    int itinerario[20] = {0};  // inizializzo l'area di memoria a 0
    trenoRegistro(id, mappa, itinerario); //gestione comunicazione treno-registro
    logFd = inizializzazioneLogFile(id, itinerario);
    int i = 1;
    int fdMaPrecedente;

    while(itinerario[i+1] != -1)
    {
        i = viaggio(itinerario, logFd, &fdMaPrecedente, i);
        printf("Treno %d richiede autorizzazione\n",id);
        sleep(2);
    }
    // libero il binario precedente alla stazione
    rilascioUltimoBinario(logFd, fdMaPrecedente, itinerario, i);

    kill(getppid(),SIGUSR1);
    pause();
    return 0;
}

// funzione che rappresenta i processi treni in modalita' ETCS2
int trenoETCS2(int id, int mappa)
{
    int itinerario[20] = {0};  // inizializzo l'area di memoria a 0
    trenoRegistro(id, mappa, itinerario); //gestione comunicazione treno-registro
    int logFd;
    logFd = inizializzazioneLogFile(id, itinerario);
    int i = 1;
    int fdMaPrecedente;
    int autorizzazione[1];
    int buffer[2];  // contiene idTreno e lo step al quale il treno e' arrivato (variabile i)
    int trenoFd, serverLen, connessione;
    struct sockaddr_un indirizzoServer;
    struct sockaddr* serverSockAddrPtr;
    char recordLog[60] = {0};
    time_t date;

    serverSockAddrPtr = (struct sockaddr*) &indirizzoServer;
    serverLen = sizeof (indirizzoServer);
    trenoFd = socket (AF_UNIX, SOCK_STREAM, DEFAULT_PROTOCOL);
    
    indirizzoServer.sun_family = AF_UNIX;
    strcpy (indirizzoServer.sun_path, "serverRBC");
    buffer[0] = id; 

    while(itinerario[i+1] != -1)
    {
        trenoFd = socket(AF_UNIX, SOCK_STREAM, DEFAULT_PROTOCOL);
        indirizzoServer.sun_family = AF_UNIX;
        do
        {
            connessione = connect (trenoFd, serverSockAddrPtr, serverLen);
            if (connessione == -1)
            {
                printf("Connessione non riuscita: riprovo in 1 secondo\n");
                sleep (1);
            }
        } while (connessione == -1);

        buffer[1] = i;
        write(trenoFd, buffer, 8);
        printf("Treno %d richiede autorizzazione... ",id);
        read(trenoFd, autorizzazione, 4);
        if(autorizzazione[0] == 1)  // se RBC ha dato l'autorizzazione controllo che non ci sia una discordanza con il file MA corrispondente e scrivo nel file di log
        {
            printf(" autorizzazione concessa\n");
            i = viaggio(itinerario, logFd, &fdMaPrecedente, i);
        }
        else    // autorizzazione non concessa
        {
            printf(" autorizzazione non concessa\n");
            date = time(NULL);
            sprintf(recordLog, "[ATTUALE: MA%d], [NEXT: MA%d], %s", itinerario[i-1], itinerario[i], ctime(&date));
            write(logFd, recordLog, strlen(recordLog));
        }
        close(trenoFd);
        sleep(2);
        
    }

    trenoFd = socket (AF_UNIX, SOCK_STREAM, DEFAULT_PROTOCOL);
    indirizzoServer.sun_family = AF_UNIX;
    do
    {
        connessione = connect (trenoFd, serverSockAddrPtr, serverLen);
        if (connessione == -1)
        {
            printf("Connessione non riuscita: riprovo in 1 secondo\n");
            sleep (1);
        }
    } while (connessione == -1);
    buffer[1]=i;
    write(trenoFd,buffer,8);

    // libero il binario precedente alla stazione
    rilascioUltimoBinario(logFd, fdMaPrecedente, itinerario, i);
    printf("Treno %d ha terminato la sua missione\n",id);
    close(trenoFd);
    kill(getppid(),SIGUSR1);
    pause();

    return 0;
}

// funzione che crea i processi treni in base alla mappa e alla modalita' selezionata
int creazione_treni(int numTreni, int mappa, char *modalita)
{
    int child_pids[5] = {0};
    if(strcmp(modalita, ETCS1) == 0)   // modalita' ETCS1
    {
        for(int i = 1; i <= numTreni; i++)
        {
            child_pids[i-1] = fork();
            if(child_pids[i-1] == 0)
            {
                trenoETCS1(i, mappa);   // ad ogni treno viene passato il proprio id
            }
        }
    }
    else    // modalita ETCS2
    {
        for(int i = 1; i <= numTreni; i++)
        {
            child_pids[i-1] = fork();
            if(child_pids[i-1] == 0)
            {
                trenoETCS2(i, mappa);   // ad ogni treno viene passato il proprio id
            }
        }
    }
    while((countEndTreni!=TRENI_MAPPA1 && mappa == 1)||(countEndTreni!=TRENI_MAPPA2 && mappa==2)); //attesa attiva per aspettare terminazione treni

    for(int i=0;i<countEndTreni;i++)
    {
        kill(child_pids[i],SIGINT);
    }
    printf("Tutti i treni hanno terminato la loro missione...\n");

    if(strcmp(modalita, ETCS2)==0)
    {
        // leggo il pid di RBC dal file
        FILE *fdPid = fopen("pidRBC.txt", "r");
        int pidRbc;
        fscanf(fdPid, "%d", &pidRbc);
        fclose(fdPid);
        kill(pidRbc, SIGUSR2);
        unlink("serverRBC");
    }
    unlink("serverRegistro");
    kill(pidRegistro, SIGINT);  // termino l'esecuzione del registro
    return 0;
}

// funzione per creare la directory per i file MA e la directory per i file di log
int creazioneDirectory(char nome[19])
{
    char rm[23];
    sprintf (rm,"rm -rf %s",nome);
    system(rm); // se esiste di gia'
    umask(000);
    mkdir(nome, 0777);
    return 0;
}


// funzione che crea la directory per i file MAx, la directory per i file log e i processi treni: prende come argomento la mappa selezionata e il tipo di modalita'
int padre_treni(char *mappa, char *modalita)
{
    if (signal(SIGUSR1, handler) == SIG_ERR) //attacco handler
        printf("errore\n");
    
    int fd;
    creazioneDirectory("../log");
    creazioneDirectory("../directoryMA");
    char s[20]="../directoryMA/MA";
    for(int i=0;i<16;i++) //inizializzazione file MA
    {
        sprintf (s,"../directoryMA/MA%02d",i+1);
        fd = open(s,O_RDWR|O_CREAT, 0666);
        write(fd,"0",1);
    }
    if(strcmp(mappa, MAPPA1) == 0)    // se la mappa selezionata e' MAPPA1, il padre crea 4 figli (treni)
    {
        creazione_treni(TRENI_MAPPA1, 1, modalita);
    }
    else    // la mappa selezionata e' MAPPA2, il padre crea 5 figli (treni)
    {
        creazione_treni(TRENI_MAPPA2, 2, modalita);
    }
    
    return 0;
}

int main(int argc, char *param[])
{
    char *mappaSelezionata;
    if(argc<3 || argc>4) 
        error();
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
        if(fork() == 0) //registro
        {
            pidRegistro = getpid();
            registro();
        }
        else if(fork() == 0) //padre_treni
        {
            padre_treni(mappaSelezionata, ETCS1);
        }
    }
    else
    {
        if(strcmp(param[2],RBC)==0) //RBC
        {
            if(argc<4) 
                error();
            else if(strcmp(param[3],MAPPA1)==0) //MAPPA1
            {
                mappaSelezionata = MAPPA1;
            }
            else if(strcmp(param[3],MAPPA2)==0) //MAPPA2
            {
                mappaSelezionata = MAPPA2;
            }
            if(fork() == 0) //RBC
            {
                execl("./RBC", "RBC", mappaSelezionata, NULL);  // avvio del server RBC con parametro la mappa selezionata
            }
        } else if(strcmp(param[1],ETCS2)==0) //ETCS2
        {
            if(strcmp(param[2],MAPPA1)==0) //MAPPA1
            {
                mappaSelezionata = MAPPA1;
            }
            else if(strcmp(param[2],MAPPA2)==0) //MAPPA2
            {
                mappaSelezionata = MAPPA2;
            }
            if(fork() == 0) //registro
            {
                pidRegistro = getpid();
                registro();
            }
            else if(fork() == 0) //padre_treni
            {
                padre_treni(mappaSelezionata, ETCS2);
            }
        }
        else
        {
            error();
        }
    }
    return 0;
}