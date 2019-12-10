#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <fcntl.h>
#include <semaphore.h>

/* Variable globale semaphore */
sem_t *monSemaphore;
int fonctionFils(int i);

/* ================================ */
/* Fonction main                    */
/* ================================ */
int main(int argc,char **argv)
{
    int i;
    int lesFils[11];
    int status;
    int unFils;

    if (argc != 2)
    {
        printf("Usage : %s nbthreadmax\n",argv[0]);
        exit(0);
    }

/* Creation du sémaphore avec pour valeur initiale le nombre max de process executant */
/* en même temps la parie critique */

    monSemaphore=sem_open("/ISABELLE.SEMAPHORE",O_CREAT | O_RDWR,0600,atoi(argv[1]));

/* Creation des processus fils */
    printf("Je suis le processus pere PID = %d\n",getpid());
    printf("Je vais créer 10 fils\n");

    for (i=0;i<10;i++)
    {
        if ((unFils=fork()) == 0)
        {
            fonctionFils(i);
            exit(EXIT_SUCCESS);
        }
        else
        {
            lesFils[i] = unFils;
        }
    }

/* Attente de la fin des fils */
    for (i=0;i< 10;i++)
    {
        waitpid(lesFils[i],&status,0);
        printf("Fin du processus %d %d\n",i,lesFils[i]);
    }

/* Destruction du sémaphore */
    sem_unlink("ISABELLE.SEMAPHORE");
    sem_close(monSemaphore);
    return 0;
}
/* ================================ */
/* Fonction exécutée par les fils   */
/* ================================ */
int fonctionFils(int i)
{
    int nbIterations,j,k,n;
    srand(getpid());
    nbIterations = (rand()%5)+1;
    j=0;
    printf("Je suis le processus no %d fils PID=%d , fils de PID=%d,nbIterations à faire = %d \n",i,getpid(), getppid(),nbIterations);
    for (j=0;j < nbIterations;j++)
    {
        /*Affichage de la valeur du semaphore */
        sem_getvalue(monSemaphore, &n);
        printf("PROCESSUS %d - Iteration N° %d / %d --- Valeur du semaphore monSempahore :(sem_getvalue) : %d\n",getpid(),j,nbIterations,n);

        /*Attente de disponibilité du sémaphore */
        sem_wait(monSemaphore);
        printf("Je suis le processus %d PID=%d , debut de ma section critique sur iteration %d \n",i,getpid(),j);
        sleep(5);
        printf("Je suis le processus %d PID=%d , fin de ma section critique sur iteration %d \n",i,getpid(),j);

        /* Liberation du semaphore */
        sem_post(monSemaphore);
    }
    printf("Je suis le processus fils %d %d, sortie de processus \n",i,getpid());
//return 0;
    exit(EXIT_SUCCESS);
}

