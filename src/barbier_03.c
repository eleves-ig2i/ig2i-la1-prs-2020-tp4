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
#include <string.h>

//#define __DEBUG

#define CHECK(sts,msg) if ((sts) == -1 ) { perror(msg);exit(-1);}

#define CHECK_DIFF(sts,msg,code_err) if ((sts) == (code_err) ) { perror(msg);exit(-1);}

#define NB_SIEGES_SALLE_ATTENTE 8

#define NOM_SEMAPHORE_SALLE_ATTENTE "/semaphore.salle.attente"

sem_t * semaphore_salle_attente = NULL;

#define NOM_SEMAPHORE_FAUTEUIL "/semaphore.salle.fauteuil"

sem_t * semaphore_fauteuil = NULL;

#define NOM_SEMAPHORE_ARRIVEE_CLIENT "/semaphore.sync.arriveeclient"

sem_t * semaphore_sync_arrivee_client = NULL;

#define NOM_SEMAPHORE_FIN_COIFFURE "/semaphore.sync.fincoiffure"

sem_t * semaphore_sync_fin_coiffure = NULL;

char nom_terminal_affichage_infos[30];
char nom_terminal_affichage_client[30];
char nom_terminal_affichage_barbier[30];

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


void * thread_client( void * arg)
{
    FILE * terminal_client = fopen(nom_terminal_affichage_client,"w");
    fprintf(terminal_client,"Barbier 03 - client - j'attends dehors.\n");
    sem_wait(semaphore_salle_attente);
    fprintf(terminal_client,"Barbier 03 - client - j'entre dans la salle d'attente.\n");
    sem_post(semaphore_sync_arrivee_client);
    sem_wait(semaphore_fauteuil);
    sem_post(semaphore_salle_attente);
    fprintf(terminal_client,"Barbier 03 - client - je vais sur le fauteuil.\n");
    sem_wait(semaphore_sync_fin_coiffure);
    fprintf(terminal_client,"Barbier 03 - client - j'ai été coiffé, je sors du salon.\n");
    sem_post(semaphore_fauteuil);
    pthread_exit(NULL);
}

void * thread_barbier( void * arg)
{
    FILE * terminal_barbier = fopen(nom_terminal_affichage_barbier,"w");
    while(1) {
        fprintf(terminal_barbier, "Barbier 03 - barbier - j'attends un client.\n");
        sem_wait(semaphore_sync_arrivee_client);
        fprintf(terminal_barbier, "Barbier 03 - barbier - un client est disponible, je vais le coiffer.\n");

        sleep(5); // le barbier coiffe le client.

        fprintf(terminal_barbier, "Barbier 03 - barbier - j'ai fini de coiffer le client.\n");
        sem_post(semaphore_sync_fin_coiffure);
    }
}

void * afficher_infos_salle_attente( void * arg)
{
    FILE * terminal_infos = fopen(nom_terminal_affichage_infos,"w");
    while(1)
    {
        int nb_personnes_salle_attente = 0, nb_personne_fauteuil = 0;
        sem_getvalue(semaphore_salle_attente,&nb_personnes_salle_attente);
        fprintf(terminal_infos,"Barbier 03 - infos: salle d'attente - il y a actuellement %d places dans la salle.\n", nb_personnes_salle_attente);
        sem_getvalue(semaphore_fauteuil,&nb_personne_fauteuil);
        fprintf(terminal_infos,"Barbier 03 - infos: fauteuil - il y a actuellement %d place sur le fauteuil.\n",nb_personne_fauteuil);
        sleep(5);
    }
}

int main(int argc, char *argv[]) {
    srand(time(NULL));
    fprintf(stdout,"Barbier 03 - Appuyez sur entrée pour faire venir un client.\n");
#ifdef __DEBUG
    fprintf(stdout,"Debug - argc = %d\n",argc);
    for(size_t i =0; i < argc; i++)
    {
        fprintf(stdout,"Debug - argv[%lu] = '%s'\n",i,argv[i]);
    }
#endif

    if( argc >= 4)
    {
        strcpy(nom_terminal_affichage_infos,argv[1]);
        strcpy(nom_terminal_affichage_client,argv[2]);
        strcpy(nom_terminal_affichage_barbier,argv[3]);
    } else
    {
        fprintf(stderr,"Veuillez appeler ce programme avec au moins 3 arguments (emplacements fichier des terminaux).\n");
        return EXIT_FAILURE;
    }


    pthread_t pthread_client;
    pthread_t pthread_salle_attente;
    pthread_t pthread_barbier;

    CHECK_DIFF(semaphore_salle_attente= sem_open(NOM_SEMAPHORE_SALLE_ATTENTE,
            O_CREAT | O_RDWR,
            0600,
            NB_SIEGES_SALLE_ATTENTE),
                    "Impossible de créer le sémaphore de la salle d'attente.\n",
                    SEM_FAILED
    );

    CHECK_DIFF(semaphore_fauteuil = sem_open(NOM_SEMAPHORE_FAUTEUIL,
            O_CREAT | O_RDWR,
            0600,
            1),
                    "Impossible de créer le sémaphore du fauteuil.\n",
               SEM_FAILED
    );

    CHECK_DIFF(semaphore_sync_fin_coiffure = sem_open(NOM_SEMAPHORE_FIN_COIFFURE,
            O_CREAT | O_RDWR,
            0600,
            0),
               "Impossible de créer le sémaphore de synchronisation de fin de coiffure.\n",
               SEM_FAILED
    );

    CHECK_DIFF(semaphore_sync_arrivee_client = sem_open(NOM_SEMAPHORE_ARRIVEE_CLIENT,
            O_CREAT | O_RDWR,
            0600,
            0),
               "Impossible de créer le sémaphore de synchronisation d'arrivée d'un client'.\n",
               SEM_FAILED
    );


    CHECK(
            pthread_create(&pthread_salle_attente,NULL,afficher_infos_salle_attente,NULL),
            "Impossible de créer le thread de la salle d'attente.\n"
    );

    CHECK(
            pthread_create(&pthread_barbier,NULL,thread_barbier,NULL),
            "Impossible de créer le thread du barbier.\n"
    );

    while(1)
    {
        while (getchar() != '\n');
        CHECK(pthread_create(&pthread_client,NULL,thread_client, NULL),
              "Impossible de créer un thread de client.\n");
    }
}