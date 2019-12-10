/**
 * Fichier : question_1.c
 * Auteur : Nathan SALEZ
 * Description : réalise les actions suivantes :
 * => récupère un paramètre de lancement lui ​ donnant le nombre maximum de threads
 * pouvant réaliser le traitement critique en même temp​s ​ (Cf + bas)
 * => lance 10 threads. Chaque thread exécute une même fonction.
 * =>Attend la fin des 10 threads pour se terminer.
 */
#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <semaphore.h>

//#define __DEBUG

#define MAX_THREADS 10

#define CHECK(sts,msg) if ((sts) == -1 ) { perror(msg);exit(-1);}

sem_t semaphore;

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


void * traitement_critique( void * arg)
{
    size_t num_thread = *(size_t * )(arg);
    fprintf(stdout,"Question_1 - thread %lu créé.\n",num_thread);
    long nb_iterations = generateRandomNumber(1,5+1);
    for(long i =0; i < nb_iterations; i++)
    {
        sem_wait(&semaphore); // décémentage du compteur de jetons
        fprintf(stdout,"Question_1 - thread %lu - début du traitement critique.\n",num_thread);
        long temps_endormi_sec = generateRandomNumber(1,5+1);
        sleep(temps_endormi_sec);
        fprintf(stdout,"Question_1 - thread %lu - fin du traitement critique.\n",num_thread);
        sem_post(&semaphore); // incrémentage du compteur de jetons
    }

    fprintf(stdout,"Question_1 - fin du thread %lu.\n",num_thread);

    pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
    srand(time(NULL));
#ifdef __DEBUG
    fprintf(stdout,"Debug - argc = %d\n",argc);
    for(size_t i =0; i < argc; i++)
    {
        fprintf(stdout,"Debug - argv[%lu] = '%s'\n",i,argv[i]);
    }
#endif
    if( argc < 2)
    {
        fprintf(stderr,"Question_1 - le programme doit contenir au moins 1 argument entier.\n");
        return EXIT_FAILURE;
    }
    long max_threads_critical = 0; // maximum de threads pouvant réaliser le traitement critique en même temps.
    max_threads_critical = strtol(argv[1], NULL, 10);

    fprintf(stdout, "Question_1 - max_threads_critical = %ld\n", max_threads_critical);
    if(max_threads_critical > MAX_THREADS || max_threads_critical <= 0)
    {
        fprintf(stderr,"Question_1 - Nombre de threads critiques en dehors de l'intervalle souhaité.\n");
    }

    pthread_t tab_threads[MAX_THREADS];
    sem_init(
            &semaphore,
            0, // on autorise tous les threads du processus appelant à utiliser le sémaphore
            max_threads_critical // nombre de jetons compris dans le sémaphore
            );
    for(size_t i = 0; i < MAX_THREADS; i++ )
    {
        CHECK(
                pthread_create(&tab_threads[i],NULL,traitement_critique,(void *)&i),
                "Impossible de créer un thread."
        );
        sleep(1);
    }
    for(size_t i = 0; i < MAX_THREADS; i++)
    {
        CHECK(
                pthread_join(tab_threads[i],NULL),
                "Impossible d'attendre le thread."
        );
    }

    sem_destroy(&semaphore);

    return 0;
}