/**
 * Copyright (c) 2025 Kevin Daniel Taylor
 * Licensed under the MIT License (see the LICENSE file in the project root).
 */
#ifndef http_task_queue_hpp
#define http_task_queue_hpp

#ifndef queue_hpp
#    include "../stl/queue.hpp"
#endif // queue_hpp

#include <pthread.h>
#include <unistd.h>
#include <netinet/in.h>

struct TaskQueue {
    Queue< int, 512 > sockets;
    pthread_mutex_t   mutex;
    pthread_cond_t    cond;
    
    void init(void);
    void destroy(void);
    void enqueue(int client_socket);
    int  dequeue(void);
    bool isFull(void);
    bool isEmpty(void);
};

void TaskQueue::init(void) {
    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&cond, NULL);
}

void TaskQueue::destroy(void) {
    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&cond);
}

void TaskQueue::enqueue(int client_socket) {
    pthread_mutex_lock(&mutex);
    if (!sockets.isFull()) {
        sockets.enqueue(client_socket);
        pthread_cond_signal(&cond);
    }
    pthread_mutex_unlock(&mutex);
}

int TaskQueue::dequeue(void) {
    int client_socket = -1;
    
    pthread_mutex_lock(&mutex);
    
    /**
     * Wait (block) if the queue is empty, temporarily unlocking the mutex.
     */
    while (isEmpty()) {
        pthread_cond_wait(&cond, &mutex);
    }
    
    /**
     * Obtain the first element in the collection (FIFO)
     */
    if (!isEmpty()) {
        client_socket = sockets.dequeue();
    }
    
    pthread_mutex_unlock(&mutex);
    return client_socket;
}

bool TaskQueue::isFull(void) {
    return sockets.isFull();
}

bool TaskQueue::isEmpty(void) {
    return sockets.isEmpty();
}

#endif // http_task_queue_hpp