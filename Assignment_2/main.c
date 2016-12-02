#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include "buffer.h"

#define true 1

/*
 * Pthread Tasks
 *
 * void* producer(void *param) : producer pthread will do this
 * void* consumer(void *param) : consumer pthread will do this
 * */
void* producer(void *param);
void* consumer(void *param);

/* the buffer */

buffer_item buffer[BUFFER_SIZE];
int header, footer; // buffer is manipulated as circular queue, header and footer is needed

/* 
 * mutex
 * pthread_mutex_t mutex    : for mutex lock buffer
 * sem_t full               : count buffer is full
 * sem_t empty              : count buffer is empty
 * */
pthread_mutex_t mutex;
sem_t full, empty;

/*
 * Pthreads
 * pthread_t *consumers      : array for consumer threads
 * pthread_t *producers      : array for producer threads 
 * */
pthread_t *consumers, *producers;

int main(int argc, char const *argv[])
{

    /*
     * Variable declare
     * 
     * int sleepTime        : How long to sleep before terminating
     * int producerNum      : The number of producer threads
     * int consumerNum      : The numver of consumer threads
     * */
    int sleepTime, producerNum, consumerNum;

    /* 1. Get command line arguments argv[1],argv[2],argv[3] */
    sleepTime = atoi(argv[1]);
    producerNum = atoi(argv[2]);
    consumerNum = atoi(argv[3]);


    // init mutex and semaphore
    // print 0 means initializes of mutex and semaphores are successed
    if (pthread_mutex_init(&mutex, NULL) != 0)  {
        fprintf(stderr, "mtex_init get error\n");
        return -1;
    }
    printf("0\n");

    if (sem_init(&full, 0, 0) != 0) { 
        fprintf(stderr, "sem_init(&full) get error\n");
        return -1;
    } 
    printf("0\n");

    if (sem_init(&empty, 0, 5) != 0) {
        fprintf(stderr, "sem_init(&empty) get error\n");
        return -1; 
    } 

    printf("0\n");

    /* 2. Initialize buffer */

    // default header, footer : start at first : empty queue
    memset(buffer, 0, sizeof(buffer_item) * BUFFER_SIZE); // fill with zero
    header = 0;
    footer = 0;

    /* 3. Create producer thread(s) */
    producers = (pthread_t *) malloc(sizeof(pthread_t) * producerNum);

    for (int i = 0; i < producerNum; i++) {
        pthread_create(&(producers[i]), NULL, &producer, (void *)0);
    }


    /* 4. Create consumer thread(s) */
    consumers = (pthread_t *) malloc(sizeof(pthread_t) * consumerNum);

    for (int i = 0; i < consumerNum; i++) {
        pthread_create(&(consumers[i]), NULL, &consumer, (void *)0);
    }

    /* 5. Sleep */
    sleep(sleepTime);

    /* 6. Exit */
    pthread_mutex_destroy(&mutex);
    sem_destroy(&full);
    sem_destroy(&empty);
    free(producers);
    free(consumers);

    return 0;
}

int insert_item(buffer_item item) {
    /* insert item into buffer
       return 0 if successful, otherwise
       return -1 indicating an error condition */

    // insert_item in bufffer[footer]
    buffer[footer] = item;

    // move footer 
    footer = (footer + 1) % BUFFER_SIZE;



    return 0;

}
int remove_item(buffer_item *item){
    /* remove an object from buffer
       placing it in item
       return 0 if successful, otherwise
       return -1 indicating an error condition */


    // remove item by circular queue via strategy
    // assign buffer[header] to item and header increased
    *item = buffer[header];
    header = (header + 1) % BUFFER_SIZE;


    return 0;
}
void* producer(void *param) {
    buffer_item item;

    srand(time(NULL));
    while (true) {
        /* sleep for a random period of time*/
        sleep((rand() % 5));
        /* generate a random number */
        item = rand();
        // producer wait for buffer has empty entry
        sem_wait(&empty);

        // mutex lock for producing   
        pthread_mutex_lock(&mutex);

        if (insert_item(item))
            fprintf(stderr, "report error condition");
        else
            printf("producer produced %d\n", item);
        // mutex unlock
        pthread_mutex_unlock(&mutex);

        // add one full semaphore
        sem_post(&full);
    }

}
void* consumer(void *param) {

    buffer_item item;

    srand(time(NULL));
    while (true) {
        /* sleep for a random period of time*/
        sleep((rand() % 5));
        // consumer wait for buffer has some value (not empty)
        sem_wait(&full);

        // mutex lock for consuming
        pthread_mutex_lock(&mutex);
        if (remove_item(&item))
            fprintf(stderr, "report error condition");
        else
            printf("consumer consumed %d\n", item);

        // mutex unlock
        pthread_mutex_unlock(&mutex);

        // add one empty semaphore
        sem_post(&empty);
    }

}
