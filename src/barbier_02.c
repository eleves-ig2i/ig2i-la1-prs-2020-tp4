/**
 * Fichier : barbier_02.c
 * Auteur : Nathan SALEZ
 */
#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <semaphore.h>
#include <fcntl.h>

//#define __DEBUG

#define CHECK(sts,msg) if ((sts) == -1 ) { perror(msg);exit(-1);}

#define CHECK_DIFF(sts,msg,code_err) if ((sts) == (code_err) ) { perror(msg);exit(-1);}

#define TEMPS_ATTENTE_CLIENT_MIN 4

#define TEMPS_ATTENTE_CLIENT_MAX 10

#define NB_SIEGES_SALLE_ATTENTE 8

#define NOM_SEMAPHORE_SALLE_ATTENTE "/semaphore.salle.attente"

sem_t * semaphore_salle_attente = NULL;

/**
 * Retourne un nombre aléatoire compris dans l'intervalle [a,b[
 * @param a
 * @param b
 * @return le nombre aléatoire décrit ci-dessus, 0 si a égal ou supérieur à b.
 */
long generateRandomNumber(int min, int max) // cf openclassroom
{
    long randomNumber = rand()%(max-min) + max;
    return (min<max)*randomNumber;
}


void * lire_magazine( void * arg)
{
    long temps_lecture_magazine = *(long * )(arg);
    fprintf(stdout,"Barbier 02 - client - j'attends dehors.\n");
    CHECK(sem_wait(semaphore_salle_attente),
          "Impossible de décrémenter le sémaphore de la salle d'attente.\n"
    );
    fprintf(stdout,"Barbier 02 - client - j'entre dans la salle d'attente.\n");
    sleep(temps_lecture_magazine);
    fprintf(stdout,"Barbier 02 - client - ce barbier est trop lent, je viendrai une autre fois.\n");
    sem_post(semaphore_salle_attente); // libération de la place
    pthread_exit(NULL);
}

void * afficher_infos_salle_attente( void * arg)
{
    while(1)
    {
        int nb_personnes = 0;
        sem_getvalue(semaphore_salle_attente,&nb_personnes);
        fprintf(stdout,"Barbier 02 - salle d'attente - il y a actuellement %d personnes dans la salle.\n",NB_SIEGES_SALLE_ATTENTE-nb_personnes);
        sleep(5);
    }
}

int main(int argc, char *argv[]) {
    srand(time(NULL));
    fprintf(stdout,"Barbier 02 - Appuyez sur entrée pour faire venir un client.\n");
#ifdef __DEBUG
    fprintf(stdout,"Debug - argc = %d\n",argc);
    for(size_t i =0; i < argc; i++)
    {
        fprintf(stdout,"Debug - argv[%lu] = '%s'\n",i,argv[i]);
    }
#endif

    pthread_t thread_client;
    pthread_t thread_salle_attente;

    CHECK_DIFF(semaphore_salle_attente= sem_open(NOM_SEMAPHORE_SALLE_ATTENTE,
            O_CREAT | O_RDWR,
            0600,
            NB_SIEGES_SALLE_ATTENTE),
            "Impossible de créer le sémaphore nommé.\n",
            SEM_FAILED
    );

    CHECK(
            pthread_create(&thread_salle_attente,NULL,afficher_infos_salle_attente,NULL),
            "Impossible de créer le thread de la salle d'attente.\n"
    );

    while(1)
    {
        while (getchar() != '\n');
        long temps_lecture_magazine = generateRandomNumber(
                TEMPS_ATTENTE_CLIENT_MIN,
                TEMPS_ATTENTE_CLIENT_MAX + 1);
        CHECK(pthread_create(&thread_client,NULL,lire_magazine, &temps_lecture_magazine),
                "Impossible de créer un thread.\n");
    }

    // Destruction du sémaphore
    sem_unlink(NOM_SEMAPHORE_SALLE_ATTENTE);
    sem_close(semaphore_salle_attente);

    return 0;
}