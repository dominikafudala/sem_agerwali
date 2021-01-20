#include "operacje.h"

#include <stdio.h>
#include <string.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define P 14 //ilosc procesow
#define MAX 10 //ilosc buforow
#define MAX2 12 //dwa ostatni indeks na wskaznik zapisu
#define SEM_NUM 3 //ilosc semaforow
#define WRITE_SEM 0
#define READ_SEM 2
#define MUTEX_SEM 1

int main()
{
    key_t kluczSemafor, kluczPamiec;
    int semID, shmID; 
    int i;

    //tworzenie zestawu semaforow
    if( (kluczSemafor = ftok(".", 'A')) == -1) //tworzenie klucza
    {
        printf("Blad ftok A (main)\n");
        exit(1);
    }
    semID = semget(kluczSemafor, SEM_NUM, IPC_CREAT | IPC_EXCL | 0666);
    if(semID == -1)
    {
        printf("blad semafora(main)\n");
        exit(1);
    }

    inicjalizujSemafor(semID, WRITE_SEM, 0); // A
    inicjalizujSemafor(semID, MUTEX_SEM, 1); // R
    inicjalizujSemafor(semID, READ_SEM, 0); // M
    
    //tworzenie pamieci dzielonej
    if((kluczPamiec = ftok(".", 'B')) == -1)
    {
        printf("blad ftok B(main)\n");
        exit(2);
    }
    shmID = shmget(kluczPamiec, MAX2*sizeof(int), IPC_CREAT | IPC_EXCL | 0666);
    if(shmID == -1)
    {
        printf("Blad pamieci dzielonej(main)\n");
        exit(1);
    }

    //procesy
    for(i = 0; i < P; i++)
    {
        switch (fork())
        {
            case -1:
                perror("Blad fork write (main)\n");
                exit(2);
            case 0:
                execl("./write", "write", NULL);
        }
    }

    for(i = 0; i < P; i++)
    {
        switch (fork())
        {
            case -1:
                perror("Blad fork read (main)\n");
                exit(2);
            case 0:
                execl("./read", "read", NULL);
        }
    }

    //wait
    for(i = 0; i< 2*P; i++)
    {
        wait(NULL);
    }

    //zwalnianie zasobów
    shmctl(shmID, IPC_RMID, NULL); //pam. dzielona
    semctl(semID, SEM_NUM, IPC_RMID, NULL); // semafory
}