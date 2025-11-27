/**
 * Copyright (c) 2025 Kevin Daniel Taylor
 * Licensed under the MIT License (see the LICENSE file in the project root).
 */
#ifndef http_server_hpp
#define http_server_hpp

#include "http_router.hpp"
#include "http_task_queue.hpp"

#include "../stl/static_collection.hpp"

#include <errno.h>

#define MAX_THREADS 16
#define MAX_CONNECTIONS 1024

struct HttpServer {
    HttpRouter   router;
    int          listenSocket;
    int          port;
    
    TaskQueue    taskQueue;
    pthread_t    threadPool[MAX_THREADS];
    int          threadCount;

    void         init(int p);
    void         start(void);
    
    static void* _workerRoutine(void* arg);
    void         _handleConnection(int clientSocket);
    void         _cleanup();
    void         _handleAcceptError(int clientSocket, int &retFlag);
    void         _setupStart(sockaddr_in &serverAddr, bool &retFlag);
    void         _initServerAddress(struct sockaddr_in& serverAddr);
    void         _listen(bool &retFlag);
    void         _bind(sockaddr_in &serverAddr, bool &retFlag);
    void         _recicleAddress();
    void         _startThreadPool();
};

void HttpServer::init(int portNumber) {
    port         = portNumber;
    listenSocket = -1;
    threadCount  = MAX_THREADS;
    taskQueue.init();
}

void HttpServer::start(void) {
    struct sockaddr_in serverAddr;

    bool retFlag;
    _setupStart(serverAddr, retFlag);
    if (retFlag)
        return;

    /**
     * Create the thread pool
     */
    _startThreadPool();
    
    SA_PRINT("HTTP Server | Thread pool (%d workers) listening the port %d...\n", threadCount, port);

    /**
     * Main thread -> Acceptation loop...
     */
    while (true) {
        struct sockaddr_in clientAddr;
        socklen_t          clientLen = sizeof(clientAddr);

        if (!taskQueue.isFull()) {
            int clientSocket = accept(listenSocket, (struct sockaddr*) &clientAddr, &clientLen);

            int retFlag;
            _handleAcceptError(clientSocket, retFlag);
            if (retFlag == 2)
                break;
            if (retFlag == 3)
                continue;

            taskQueue.enqueue(clientSocket);
        }
    }

    _cleanup();
}

void HttpServer::_handleAcceptError(int clientSocket, int &retFlag) {
    retFlag = 1;
    if (clientSocket < 0) {
        int currentErrno = errno;

        if (currentErrno == EINTR) {
            /** interrupt: retry */
            {
                retFlag = 3;
                return;
            };
        }
        else if (currentErrno == EBADF) {
            /** If EBADF occurs here, is a fatal error. (the socket was closed outside the loop) */
            {
                retFlag = 2;
                return;
            }; /** end accepting loop */
        }
        else {
            /** other errors */
            SA_PRINT_ERR("Accept error: System error (%d)\n", currentErrno);
        }
        retFlag = 3;
        return;
    }
}

void HttpServer::_setupStart(sockaddr_in &serverAddr, bool &retFlag) {
    retFlag = true;

    listenSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (listenSocket < 0) {
        SA_PRINT_ERR("Error: Fail creating first listen socket.\n");
        return;
    }

    _initServerAddress(serverAddr);
    _recicleAddress();

    _bind(serverAddr, retFlag);
    if (retFlag)
        return;

    _listen(retFlag);
    if (retFlag)
        return;
    retFlag = false;
}

void HttpServer::_listen(bool &retFlag) {
    retFlag = true;
    if (listen(listenSocket, MAX_CONNECTIONS) < 0) {
        SA_PRINT_ERR("Error: listen fail.\n");
        close(listenSocket);
        return;
    }
    retFlag = false;
}

void HttpServer::_bind(sockaddr_in &serverAddr, bool &retFlag) {
    retFlag = true;
    if (bind(listenSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0) {
        SA_PRINT_ERR("Error: Bind fail. Port nb: %d its occupied.\n", port);
        close(listenSocket);
        return;
    }
    retFlag = false;
}

void HttpServer::_recicleAddress() {
    int opt = 1;
    setsockopt(listenSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
}

/** member function that executes each thread worker... */
void* HttpServer::_workerRoutine(void* arg) {
    HttpServer* server = (HttpServer*) arg;
    int clientSocket;
    
    /** while true: each worker waits for jobs */
    while (true) {
        /** blocks until there is an available task */
        clientSocket = server->taskQueue.dequeue();
        
        if (clientSocket > 0) {
            /** handle the connection if there is a task */
            server->_handleConnection(clientSocket);
            /** the socket is closed inside of _handleConnection */
        }
    }
    return NULL;
}

void HttpServer::_cleanup() {
    close(listenSocket);
    taskQueue.destroy();
}

void HttpServer::_initServerAddress(struct sockaddr_in& serverAddr) {
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family      = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port        = htons(port);
}

void HttpServer::_startThreadPool() {
    for (int i = 0; i < threadCount; i++) {
        if (pthread_create(&threadPool[i], NULL, HttpServer::_workerRoutine, (void*) this) != 0) {
            perror("Error creating worker thread.");
            /** Error handler... */
        }
    }
}

void HttpServer::_handleConnection(int clientSocket) {
    char buffer[1024];
    uint bytesRead;
    
    bytesRead = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
    if (bytesRead <= 0) {
        close(clientSocket);
        return;
    }
    buffer[bytesRead] = '\0';

    HttpRequest  req;
    HttpResponse res;

    AnsiString< 1024 > fullRequest { buffer };
    
    const char firstDelimiter[5]  = "\r\n\r\n";
    uint       firstDelimiterSize = sizeof(firstDelimiter);
    int        delimiterPos       = fullRequest.pos(firstDelimiter);
    
    if (delimiterPos > 0) {
        AnsiString< 256 > headersPart = fullRequest.subStr< 256 >(0, delimiterPos);
        AnsiString< 256 > bodyPart;
        if (delimiterPos + firstDelimiterSize < fullRequest.length()) {
            AnsiString< 256 > rawbody = fullRequest.subStr< 256 >(delimiterPos + firstDelimiterSize, fullRequest.length());
            bodyPart
                .concat("{")
                    .concat(rawbody)
                .concat("}");
        }

        int firstLineEnd = headersPart.pos("\r\n");
        AnsiString< 256 > requestLine;

        if (firstLineEnd > 0) {
            requestLine = headersPart.subStr< 256 >(0, firstLineEnd);
            
            char spaceDelimiter = ' ';
            Collection< AnsiString< 256 > > tokens = requestLine.split(spaceDelimiter);

            if (tokens.length >= 1) req.method  = tokens.at(0);
            if (tokens.length >= 2) req.path    = tokens.at(1);
            if (tokens.length >= 3) req.version = tokens.at(2);

        } else {
            requestLine = headersPart;
        }

        if (bodyPart.length() > 0) {
            req.body.init(bodyPart.cstr(), bodyPart.length());
        }
    }

    router.routeRequest(&req, &res);
    
    AnsiString< 256 > responseStr = res.toString();
    send(clientSocket, responseStr.cstr(), responseStr.length(), 0);
    
    close(clientSocket);
}

#endif // http_server_hpp