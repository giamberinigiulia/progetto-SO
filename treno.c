#include <stdio.h>
#include <sys/socket.h>
#include <sys/un.h>

#define DEFAULT_PROTOCOL 0

int main()
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
    char arg[3] = "3 1";
    write(trenoFd, arg, 3);
    char itinerario[100];
    read(trenoFd, itinerario, 100);
    printf("Risposta: %s\n", itinerario);
    close(trenoFd);
    return 0;
}