
#include "queue.h"

#include <pthread.h>
#include <semaphore.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <assert.h>

#define handle_error_en(en, msg) \
        do { errno = en; perror(msg); exit(EXIT_FAILURE); } while (0)

#define handle_error(msg) \
        do { perror(msg); exit(EXIT_FAILURE); } while (0)

typedef struct QueueStruct {
	
	int head;
	int tail;
    int size;
    void **buffer;
    sem_t queue_space;
    sem_t item_in_queue;
    pthread_mutex_t mutex;

} Queue;


Queue *queue_alloc(int size) {
    
    Queue *queue = malloc(sizeof(Queue));
    queue->size = size;
    queue->buffer = calloc(size, sizeof(void*));;
    
	if (sem_init(&queue->queue_space, 0, 0) != 0) {
		handle_error("ERROR initialising semaphore queue_space");
	}
	if (sem_init(&queue->item_in_queue, 0, (unsigned) size) != 0) {
		handle_error("ERROR initialising semaphore item_in_queue");
	}
	
	if (pthread_mutex_init(&queue->mutex, NULL) != 0) {
        handle_error("ERROR initialising mutex");
    }
    
    return queue;
    
}

void queue_free(Queue *queue) {

    // free the queue and destroy the mutex lock and semaphore
    pthread_mutex_destroy(&queue -> mutex);
    sem_destroy(&queue->queue_space);
    sem_destroy(&queue->item_in_queue);
    free(queue->buffer);
    free(queue);

}

void queue_put(Queue *queue, void *item) {

	// wait till queue has space, lock semaphore and decrement item_in_queue
	sem_wait(&queue->item_in_queue);

	// lock to block the thread
    pthread_mutex_lock(&queue->mutex);
    
    queue->buffer[queue->tail] = item;

    if (queue->tail < queue->size - 1) {
        queue->tail++;
    }
    else {
        queue->tail = 0;
    }
	
	// unlock the thread
	pthread_mutex_unlock(&queue->mutex);

	// unlock semaphore and increment queue_space
	sem_post(&queue->queue_space);

}



void *queue_get(Queue *queue) {

    sem_wait(&queue->queue_space);

    pthread_mutex_lock(&queue->mutex);
    
    void *item = queue->buffer[queue->head];

    queue->buffer[queue->head] = NULL;

    if (queue->head < queue->size - 1) {
        queue->head++;
    }
    else {
        queue->head = 0;
    }
	
	pthread_mutex_unlock(&queue->mutex);

	sem_post(&queue->item_in_queue);
	
	// this function is 'void*' which requires a 'void*' type item to be returned
	return item;

}

