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

void error()
{
    printf("\nErrore nell'inserimento dei parametri\n");
}

void itinerario(char buffer[100], int socket_client)
{
    int mappa1 [4][8]=
    {
        {1,1,2,3,8,6,-1},
        {2,5,6,7,3,4,5,-1},
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

    int socket_descrittore; // Variabile che contiene il descrittore per il socket che andremo a creare
    int socket_client, len; // Socket del client e dimensione della struttura del socket
    struct sockaddr_un mio_server; // Struttura che contiene i dettagli del server
    struct sockaddr_un client;
    int *risposta;
    char buffer_ricezione[100];

    socket_descrittore = socket (AF_UNIX, SOCK_STREAM, DEFAULT_PROTOCOL);
    if (socket_descrittore == -1) 
    {
        printf ("Errore di creazione socket.\n");
        return 1;
    }

    mio_server.sun_family = AF_UNIX;
    strcpy(mio_server.sun_path, "serverRegistro");
    unlink("serverRegistro");
    bind (socket_descrittore, (struct sockaddr *)&mio_server, sizeof (mio_server));

    listen (socket_descrittore, 10);
    printf ("In ascolto.\n");


    while(1)
    {
        //socket_descrittore = socket (AF_UNIX, SOCK_STREAM, DEFAULT_PROTOCOL);
        printf("CREATO NUOVO SOCKET PER CLIENT\n");
        int clientLen = sizeof(client);
        socket_client = accept(socket_descrittore, (struct sockaddr *)&client, &clientLen);
        if(fork() == 0)
        {
            printf ("Connessione in arrivo.\n");

            read(socket_client, buffer_ricezione, 100);
            printf("Ricezione: %s\n",buffer_ricezione);
            itinerario(buffer_ricezione, socket_client);
        }
        else 
        {
            close(socket_client);
        }
    }

    close(socket_client);
    close(socket_descrittore);
    unlink("serverRegistro");
    exit(0);
    return 0;
}

// funzione che controlla se i segmenti sono liberi (contenuto del file MA = 0) e scrittura nel file di log
int viaggio(int itinerario[20], int logFd, int *fdMaPrecedente, int i)
{
    char fileMa[19];
    char flagFile[1];
    char recordLog[60] = {0};
    int fdMa;
    time_t date;
    date = time(NULL);

    sprintf (fileMa,"./directoryMA/MA%02d",itinerario[i]);
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
    char fileLog[13];
    sprintf (fileLog,"./log/T%1d.log",id);  // ogni treno crea il proprio file di log nella directory log
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

// funzione che rappresenta i processi treni in modalita' ETCS1
int treno(int id, int mappa)
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
    // libero il binario precedente alla stazione

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
    rilascioUltimoBinario(logFd, fdMaPrecedente, itinerario, i);
    close(trenoFd);
    exit(0);
    return 0;
}

// funzione che crea i processi treni in base alla mappa e alla modalita' selezionata
int creazione_treni(int numTreni, int mappa, char *modalita)
{
    if(strcmp(modalita, ETCS1) == 0)   // modalita' ETCS1
    {
        for(int i = 1; i <= numTreni; i++)
        {
            if(fork() == 0)
            {
                treno(i, mappa);   // ad ogni treno viene passato il proprio id
                exit(0);
            }
        }
    }
    else    // modalita ETCS2
    {
        for(int i = 1; i <= numTreni; i++)
        {
            if(fork() == 0)
            {
                trenoETCS2(i, mappa);   // ad ogni treno viene passato il proprio id
                exit(0);
            }
        }
    }
    return 0;
}

// funzione per creare la directory per i file MA e la directory per i file di log
int creazioneDirectory(char nome[16])
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
    int fd;
    creazioneDirectory("log");
    creazioneDirectory("directoryMA");
    char s[20]="./directoryMA/MA";
    for(int i=0;i<16;i++)
    {
        sprintf (s,"./directoryMA/MA%02d",i+1);
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
            registro(mappaSelezionata);
        }
        else if(fork() == 0)
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