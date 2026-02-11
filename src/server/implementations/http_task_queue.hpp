/**
 * Copyright (c) 2025 Kevin Daniel Taylor
 * Licensed under the MIT License (see the LICENSE file in the project root).
 */
#ifndef http_task_queue_hpp
#define http_task_queue_hpp

#ifndef queue_hpp
#    include "../../stl/queue.hpp"
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

#endif // http_task_queue_hpp