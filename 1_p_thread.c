
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>     /* usleep */
#include <pthread.h>
#include <semaphore.h>

/*
Prosess-
nummer
ˆ
5 |           S--------T
4 |  S--------T
3 |        S-----T
2 S--------T
1 |  S-----T
0 S--T
+-----------------------> tid i sekunder
  0  1  2  3  4  5  6  7
*/

#define THREAD_COUNT 6


sem_t active[THREAD_COUNT];

struct threadargs {
    int id;                     /* thread number */
    int sec;                    /* how many seconds to sleep */
    int signal[THREAD_COUNT];   /* which threads to signal when done */
};

void *tfunc(void *arg) {
    /* define args as threadargs struct */
    struct threadargs *targs=arg;

    /* wait for connected threads */
    printf("venter på aktivering %d\n", targs->id);
    sem_wait(&active[targs->id]);

    /* pretend to work */
    printf("thread %d kjører\n", targs->id);
    sleep(targs->sec);
    printf("Prosess %d kjørte i %d sekunder\n", targs->id, targs->sec);

    /* wake connected threads */
    int count = 0;
    while(targs->signal[count] != 0){
        sem_post(&active[targs->signal[count]]);
        count++;
    }

}

int main(void)
{
    int i,j;

    /* declare threads and args */
    pthread_t threads[THREAD_COUNT];
    struct threadargs *targs[THREAD_COUNT];

    /* allocate memory for threadargs and zero out semaphore signals */
    for (i=0;i<THREAD_COUNT;i++) {

        /* create pointers to args in stack and save data to heap */
        targs[i] = (struct threadargs*) malloc(sizeof(struct threadargs));
        targs[i]->id = i;
        for (j=0;j<THREAD_COUNT;j++) {
            targs[i]->signal[j]=0;
        }

        /* set all semaphores to zero and make them local to process */
        sem_init(&active[i], 0, 0);
    }

    /* define task time for each thread */
    targs[0]->sec = 1;
    targs[1]->sec = 2;
    targs[2]->sec = 3;
    targs[3]->sec = 2;
    targs[4]->sec = 3;
    targs[5]->sec = 3;

    /* create relations between threads */
    targs[0]->signal[0] = 1;
    targs[0]->signal[1] = 4;
    targs[2]->signal[0] = 3;
    targs[4]->signal[0] = 5;

    /* create threads with args and check for error */
    for (i=0;i<THREAD_COUNT;i++) {
        if(pthread_create(&threads[i], NULL, tfunc, targs[i]) != 0){
            printf("%s\n", "failed to create thread");
            return -1;
        }
    }

    /* start threads */
    sem_post(&active[0]);
    sem_post(&active[2]);

    /* let main wait for threads to finish and check for erros */
    for (i=0;i<THREAD_COUNT;i++) {
        if(pthread_join(threads[i], NULL)){
            printf("%s\n", "failed to join thread");
            return EXIT_FAILURE;
        }
    }

    return EXIT_SUCCESS;
}
