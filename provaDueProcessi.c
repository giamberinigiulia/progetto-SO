#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <time.h>

int main ( int argc, char *argv[] )
{
    printf("Padre: %d\n", getpid());
    int i, pid, ran;

    for(i = 0; i < atoi(argv[1]); i++) {
        pid = fork();
        ran = 3 ;

         if (pid < 0) {
            printf("Error");
            exit(1);
         } else if (pid == 0) {
            printf("Child (%d): %d\n", i + 1, getpid());
            printf("Sleep for = %d\n", ran);
            sleep(ran*10);
            exit(ran*10); 
         }
    }

    for(i = 0; i < atoi(argv[1]); i++) {
        int status = 0;
        pid_t childpid = wait(&status);
        printf("Parent knows child %d is finished. \n", (int)childpid);
    }
}