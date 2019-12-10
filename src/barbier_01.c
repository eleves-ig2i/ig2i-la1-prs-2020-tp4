/**
 * Fichier : barbier_01.c
 * Auteur : Nathan SALEZ
 */
#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <semaphore.h>

//#define __DEBUG

#define CHECK(sts,msg) if ((sts) == -1 ) { perror(msg);exit(-1);}

#define TEMPS_ATTENTE_CLIENT_MIN 4

#define TEMPS_ATTENTE_CLIENT_MAX 10

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
    sleep(temps_lecture_magazine);
    fprintf(stdout,"Barbier 01 - client - ce barbier est trop lent, je viendrai une autre fois.\n");
    pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
    srand(time(NULL));
    fprintf(stdout,"Barbier 01 - Appuyez sur entrée pour faire venir un client.\n");
#ifdef __DEBUG
    fprintf(stdout,"Debug - argc = %d\n",argc);
    for(size_t i =0; i < argc; i++)
    {
        fprintf(stdout,"Debug - argv[%lu] = '%s'\n",i,argv[i]);
    }
#endif
    pthread_t thread_client;

    while(1)
    {
        while (getchar() != '\n');
        long temps_lecture_magazine = generateRandomNumber(
                TEMPS_ATTENTE_CLIENT_MIN,
                TEMPS_ATTENTE_CLIENT_MAX + 1);
        fprintf(stdout,"Barbier 01 - client - j'entre dans la salle d'attente.\n");
        pthread_create(&thread_client,NULL,lire_magazine, &temps_lecture_magazine);
    }

    return 0;
}