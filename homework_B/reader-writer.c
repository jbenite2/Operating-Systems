/*
Name: Jose Benitez
Class: Operating Systems
Date: March 4, 2023
Assignment: Homework B
*/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <semaphore.h>

sem_t read_mutex;
sem_t write_mutex;
int nreaders = 0;
int buffer = 0;

void read_lock() {
    sem_wait(&read_mutex);
    nreaders++;
    if (nreaders == 1) {
        sem_wait(&write_mutex);
    }
    sem_post(&read_mutex);
}

void read_unlock() {
    sem_wait(&read_mutex);
    nreaders--;
    if (nreaders == 0) {
        sem_post(&write_mutex);
    }
    sem_post(&read_mutex);
}

void write_lock() {
    sem_wait(&write_mutex);
}

void write_unlock() {
    sem_post(&write_mutex);
}

void * reader_writer_thread(void *arg) {
    int thread_id = *(int*)arg;
    int counter= 0;
    while(counter != 6) {
        if(rand() % 2) {
            read_lock();
            printf("Round %d:  Thread %d read buffer: %d\n", counter,thread_id, buffer);
            read_unlock();
        } else {
            write_lock();
            buffer++;
            printf("Round %d:                                     Thread %d wrote buffer: %d\n", counter,thread_id, buffer);
            write_unlock();
        }
        // sleep for one second
        counter++;
        sleep(1); 

    }
    return NULL;
}

int main() {
    //Initialize the threads and their integer identifiers that will be passed in as arguments
    pthread_t thread1, thread2, thread3, thread4, thread5;
    int t1 = 1, t2 = 2, t3 = 3, t4 = 4, t5 = 5;

    //The reader and writer semaphores are initialized to 1 indicating the mutex is unlocked
    sem_init(&read_mutex, 0, 1);
    sem_init(&write_mutex, 0, 1);

    //The 5 threads are created and passed to the function  reader_writer_thread
    pthread_create(&thread1, NULL,  reader_writer_thread, (void*)&t1);
    pthread_create(&thread2, NULL,  reader_writer_thread, (void*)&t2);
    pthread_create(&thread3, NULL,  reader_writer_thread, (void*)&t3);
    pthread_create(&thread4, NULL,  reader_writer_thread, (void*)&t4);
    pthread_create(&thread5, NULL,  reader_writer_thread, (void*)&t5);

    //Join the threads to prevent the program (main thread) from exiting prematurely yet in this case the threads will keep reading and writing until the user sends the interrupt signal
    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);
    pthread_join(thread3, NULL);
    pthread_join(thread4, NULL);
    pthread_join(thread5, NULL);

    sem_destroy(&read_mutex);
    sem_destroy(&write_mutex);

    return 0;
}
