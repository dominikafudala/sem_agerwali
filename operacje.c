#include "operacje.h"

int alokujSemafor(key_t klucz, int number, int flagi)
{
   int semID;
   if ( (semID = semget(klucz, number, flagi)) == -1)
   {
      perror("Blad semget (alokujSemafor): ");
      exit(1);
   }
   return semID;
}

int zwolnijSemafor(int semID, int number)
{
   return semctl(semID, number, IPC_RMID, NULL);
}

void inicjalizujSemafor(int semID, int number, int val)
{
   
   if ( semctl(semID, number, SETVAL, val) == -1 )
   {
      perror("Blad semctl (inicjalizujSemafor): ");
      exit(1);
   }
}

int waitSemafor(int semID, int number, int flags)
{
   int result;
   struct sembuf operacje[1];
   operacje[0].sem_num = number;
   operacje[0].sem_op = -1;
   operacje[0].sem_flg = 0 | flags;//SEM_UNDO;
   
   if ( semop(semID, operacje, 1) == -1 )
   {
      perror("Blad semop (waitSemafor)");
      return -1;
   }
   
   return 1;
}

void signalSemafor(int semID, int number)
{
   struct sembuf operacje[1];
   operacje[0].sem_num = number;
   operacje[0].sem_op = 1;
   operacje[0].sem_flg = SEM_UNDO;

   if (semop(semID, operacje, 1) == -1 )
      perror("Blad semop (postSemafor): ");

   return;
}

int valueSemafor(int semID, int number)
{
   return semctl(semID, number, GETVAL, NULL);
}

int waitSemaforPE(int semID, int *tab1, int num1, int *tab2, int num2)
{
    struct sembuf operacje[2];
    int i, j, liczba;
    for(i = 0; i<num1; i++) //zawieszenie procesu do czasu aż si > 0 
    {
        operacje[i].sem_num = tab1[i];
        operacje[i].sem_op = -1;
        operacje[i].sem_flg = SEM_UNDO;
    }

    for(j = 0; j<num2; j++) // dla wszystkich sj  = 0
    {
        operacje[i].sem_num = tab2[j];
        operacje[i].sem_op = 0;
        operacje[i].sem_flg = SEM_UNDO;
        i++;
    }

    liczba = num1+num2;
    if(semop(semID, operacje, liczba) == -1)
    {
        perror("blad semop(PE):");
        return -1;
    }
    return 1;
}

int signalSemaforVE(int semID, int *tab, int num)
{
    struct sembuf operacje[2];
    int i;
    for(i = 0; i<num; i++)
    {
        operacje[i].sem_num = tab[i];
        operacje[i].sem_op = 1;
        operacje[i].sem_flg = SEM_UNDO;
    }

    if(semop(semID, operacje, num) == -1)
    {
        perror("blad semop(VE):");
        return -1;
    }
    return 1;
}
