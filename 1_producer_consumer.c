#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>     /* usleep */
#include <pthread.h>
#include <semaphore.h>

#define SHARED 0        /* process-sharing if !=0, thread-sharing if =0*/
#define BUF_SIZE 20
#define MAX_MOD 100000
#define NUM_ITER 20000
#define MAX_PC 200         /* max Producer and Consumer*/

void* Producer(void *); /* Producer thread */
void* Consumer(void *); /* Consumer thread */

sem_t empty;            /* empty: How many empty buffer slots */
sem_t full;             /* full: How many full buffer slots */
sem_t prod_iter;            /* empty: How many empty buffer slots */
sem_t cons_iter;            /* empty: How many empty buffer slots */
pthread_mutex_t mutex;  /* mutex: Mutex lock */


int g_data[BUF_SIZE];   /* shared finite buffer  */
int g_idx;              /* index to next available slot in buffer,
                           remember that globals are set to zero
                           according to C standard, so no init needed  */

int main(int argc, char* args[]) {
    int i;
    int cpCount = 5;

    // declare threads
	pthread_t pid[MAX_PC];
    pthread_t cid[MAX_PC];

    // check program arguments
    if(argc > 1){
        cpCount = atoi(args[1]);

        if(cpCount > MAX_PC){
            printf("max pc is %d setting pc to max\n", MAX_PC);
            cpCount = MAX_PC;
        }
    }

	// Initialie the semaphores
	sem_init(&empty, SHARED, BUF_SIZE);
	sem_init(&full, SHARED, 0);
    sem_init(&prod_iter, SHARED, 0);
    sem_init(&cons_iter, SHARED, 0);

	// Initialize the mutex
	pthread_mutex_init(&mutex,0);

	// Create the threads
	printf("main started\n");

    // loop all threads
    for(i = 0; i < cpCount; i++){

        // allocate id in heap
        int* id = malloc(sizeof(int));
        *id = i;

        // create requested threads
        if(pthread_create(&pid[i], NULL, Producer, id) != 0){
            printf("failed to create producer thread at i = %d\n", i);
        }
    	if(pthread_create(&cid[i], NULL, Consumer, id) != 0){
            printf("failed to create consumer thread at i = %d\n", i);
        }
    }

	// And wait for them to finish.
    for(i = 0; i < cpCount; i++){
    	if(pthread_join(pid[i], NULL) != 0){
            printf("failed to join producer thread at i = %d\n", i);
        }
        if(pthread_join(cid[i], NULL) != 0){
            printf("failed to join consumer thread at i = %d\n", i);
        }
    }
	printf("main done\n");

	return 0;
}


void *Producer(void *arg) {
    int *id = arg;

	int i=0, j;

    printf("\nProducer%d\n", *id);

    // check how much is produced
    sem_getvalue(&prod_iter, &i);
	while(i < NUM_ITER) {

		// pretend to generate an item by a random wait
        usleep(rand()%MAX_MOD);
		// Wait for at least one empty slot
		sem_wait(&empty);
		// Wait for exclusive access to the buffer
		pthread_mutex_lock(&mutex);

		// Check if there is content there already. If so, print
        // a warning and exit.
		if(g_data[g_idx] == 1) {
			printf("Producer%d overwrites!, idx er %d\n", *id, g_idx);
			exit(0);
		}

		// Fill buffer with "data" (ie: 1) and increase the index.
		g_data[g_idx]=1;
		g_idx++;

		// Print buffer status.
		j=0; printf("(Producer%d, idx is %d): ", *id, g_idx);
		while(j < g_idx) { j++; printf("="); } printf("\n");

		// Leave region with exlusive access
		pthread_mutex_unlock(&mutex);
		// Increase the counter of full bufferslots.
		sem_post(&full);
        sem_post(&prod_iter);

        sem_getvalue(&prod_iter, &i);

	}

	return 0;
}


void *Consumer(void *arg) {
    int* id = arg;

	int i=0, j;

    printf("\nConsumer%d\n", *id);

    // check how much is consumed
    sem_getvalue(&cons_iter, &i);
	while(i < NUM_ITER) {
		// Wait a random amount of time, simulating consuming of an item.
		usleep(rand()%MAX_MOD);

		// Wait for at least one slot to be full
		sem_wait(&full);
		// Wait for exclusive access to the buffers
		pthread_mutex_lock(&mutex);

		// Checked that the buffer actually contains some data
        // at the current slot.
		if(g_data[g_idx-1] == 0) {
			printf("Consumes nothing at id %d!, idx er %d\n", *id, g_idx);
			exit(0);
		}

		// Remove the data from the buffer (ie: Set it to 0)
		g_data[g_idx-1]=0;
		g_idx--;

		// Print the current buffer status
		j=0; printf("(Consumer%d, idx is %d): ", *id, g_idx);
		while(j < g_idx) { j++; printf("="); } printf("\n");

		// Leave region with exclusive access
		pthread_mutex_unlock(&mutex);
		// Increase the counter of empty slots.
		sem_post(&empty);
        sem_post(&cons_iter);
        sem_getvalue(&cons_iter, &i);

	}

	return 0;

}
