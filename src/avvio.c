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

void error()
{
    printf("Errore nell'inserimento dei parametri\n");
}

void handler(int signalNum)
{
    countEndTreni++;
}

// funzione che rappresenta i processi treni in modalita' ETCS1
int trenoETCS1(int id, int mappa)
{
    int logFd;
    int itinerario[20] = {0};  // inizializzo l'area di memoria a 0
    trenoRegistro(id, mappa, itinerario);
    logFd = inizializzazioneLogFile(id, itinerario);
    int i = 1;
    int fdMaPrecedente;

    while(itinerario[i+1] != -1)
    {
        i = viaggio(itinerario, logFd, &fdMaPrecedente, i);
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
    trenoRegistro(id, mappa, itinerario);
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
    //while(itinerario[i+1] != -1)
    while(itinerario[i+1] != -1)
    {
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

        buffer[1] = i;
        write(trenoFd, buffer, 8);
        read(trenoFd, autorizzazione, 4);
        if(autorizzazione[0] == 1)  // se RBC ha dato l'autorizzazione controllo che non ci sia una discordanza con il file MA corrispondente e scrivo nel file di log
        {
            i = viaggio(itinerario, logFd, &fdMaPrecedente, i);
        }
        else    // autorizzazione non concessa
        {
            date = time(NULL);
            sprintf(recordLog, "[ATTUALE: MA%d], [NEXT: MA%d], %s", itinerario[i-1], itinerario[i], ctime(&date));
            write(logFd, recordLog, strlen(recordLog));
        }
        close(trenoFd);
        sleep(2);
        
    }
    // richiesta di accesso alla stazione di arrivo
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
                //exit(0);
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
                //exit(0);
            }
        }
    }
    time_t date;
    date = time(NULL);
    printf("%s\n",ctime(&date));

    while((countEndTreni!=TRENI_MAPPA1 && mappa == 1)||(countEndTreni!=TRENI_MAPPA2 && mappa==2));

    for(int i=0;i<countEndTreni;i++)
    {
        printf("Children %d: %d\n",i,child_pids[i]);
        kill(child_pids[i],SIGINT);
    }
    date = time(NULL);
    printf("%s\n",ctime(&date));
    kill(pidRegistro,SIGINT);
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
    for(int i=0;i<16;i++)
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
        if(fork() == 0)
        {
            pidRegistro=getpid();
            registro(mappaSelezionata);
        }
        else if(fork() == 0)
        {
            printf("\n babo %d\n",getpid());
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
            if(fork() == 0)
            {
                execl("./RBC", "RBC", mappaSelezionata, NULL);  // avvio del server RBC con parametro la mappa selezionata
                exit(0);
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
            if(fork() == 0)
            {
                registro(mappaSelezionata);
                exit(0);
            }
            else if(fork() == 0)
            {
                padre_treni(mappaSelezionata, ETCS2);
                exit(0);
            }
        }
        else
        {
            error();
        }
    }
    return 0;
}