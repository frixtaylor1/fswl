/**
 * Copyright (c) 2025 Kevin Daniel Taylor
 * Licensed under the MIT License (see the LICENSE file in the project root).
 */
#ifndef http_server_hpp
#define http_server_hpp

#include "../../stl/common.hpp"
#include "http_router.hpp"
#include "http_task_queue.hpp"
#include <string.h>

#include "../../stl/static_collection.hpp"
#include "../interfaces/iserver.hpp"
#include "../interfaces/irouter.hpp"

#include <errno.h>

#define MAX_THREADS 16
#define MAX_CONNECTIONS 1024

struct HttpServer : implements IServer {
    IRouter&     router;
    int          listenSocket;
    int          port;
    
    TaskQueue    taskQueue;
    pthread_t    threadPool[MAX_THREADS];
    int          threadCount;

    void         init(uint port);
    void         start(void);

    HttpServer(IRouter& routerImpl) : router(routerImpl) {}

private:
    static void* workerRoutine(void* arg);
    void         handleConnection(int clientSocket);
    void         debugRequestHeaders(String &headersPart, HttpRequest &req, String &fullRequest);
    void         parseMethodPathAndVersion(String &headersPart, HttpRequest &req);
    void         setBody(String &bodyPart, HttpRequest &req);
    void         parseBody(int delimiterPos, uint firstDelimiterSize, String &fullRequest, String &bodyPart);
    void         cleanup();
    void         handleAcceptError(int clientSocket, int &retFlag);
    void         setupStart(sockaddr_in &serverAddr, bool &retFlag);
    void         initServerAddress(struct sockaddr_in& serverAddr);
    void         listen(bool &retFlag);
    void         bind(sockaddr_in &serverAddr, bool &retFlag);
    void         recicleAddress();
    void         startThreadPool();
};

void HttpServer::init(uint port) {
    this->port   = port;
    listenSocket = -1;
    threadCount  = MAX_THREADS;
    taskQueue.init();
}

void HttpServer::start(void) {
    struct sockaddr_in serverAddr;

    bool retFlag;
    setupStart(serverAddr, retFlag);
    if (retFlag)
        return;

    /**
     * Create the thread pool
     */
    startThreadPool();
    
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
            handleAcceptError(clientSocket, retFlag);
            if (retFlag == 2)
                break;
            if (retFlag == 3)
                continue;

            taskQueue.enqueue(clientSocket);
        }
    }

    cleanup();
}

void HttpServer::handleAcceptError(int clientSocket, int &retFlag) {
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

void HttpServer::setupStart(sockaddr_in &serverAddr, bool &retFlag) {
    retFlag = true;

    listenSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (listenSocket < 0) {
        SA_PRINT_ERR("Error: Fail creating first listen socket.\n");
        return;
    }

    initServerAddress(serverAddr);
    recicleAddress();

    bind(serverAddr, retFlag);
    if (retFlag)
        return;

    listen(retFlag);
    if (retFlag)
        return;
    retFlag = false;
}

void HttpServer::listen(bool &retFlag) {
    retFlag = true;
    if (::listen(listenSocket, MAX_CONNECTIONS) < 0) {
        SA_PRINT_ERR("Error: listen fail.\n");
        close(listenSocket);
        return;
    }
    retFlag = false;
}

void HttpServer::bind(sockaddr_in &serverAddr, bool &retFlag) {
    retFlag = true;
    if (::bind(listenSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0) {
        SA_PRINT_ERR("Error: Bind fail. Port nb: %d its occupied.\n", port);
        close(listenSocket);
        return;
    }
    retFlag = false;
}

void HttpServer::recicleAddress() {
    int opt = 1;
    setsockopt(listenSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
}

/** member function that executes each thread worker... */
void* HttpServer::workerRoutine(void* arg) {
    HttpServer* server = (HttpServer*) arg;
    int clientSocket;
    
    /** while true: each worker waits for jobs */
    while (true) {
        /** blocks until there is an available task */
        clientSocket = server->taskQueue.dequeue();
        
        if (clientSocket > 0) {
            /** handle the connection if there is a task */
            server->handleConnection(clientSocket);
            /** the socket is closed inside of handleConnection */
        }
    }
    return NULL;
}

void HttpServer::cleanup() {
    close(listenSocket);
    taskQueue.destroy();
}

void HttpServer::initServerAddress(struct sockaddr_in& serverAddr) {
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family      = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port        = htons(port);
}

void HttpServer::startThreadPool() {
    for (int i = 0; i < threadCount; i++) {
        if (pthread_create(&threadPool[i], NULL, HttpServer::workerRoutine, (void*) this) != 0) {
            perror("Error creating worker thread.");
            /** Error handler... */
        }
    }
}

void HttpServer::handleConnection(int clientSocket) {
    char buffer[2048];

    HttpRequest  req;
    HttpResponse res;

    String fullRequest;

    while (true) {
        int n = recv(clientSocket, buffer, sizeof(buffer), 0);
        if (n <= 0) break;

        fullRequest.append(buffer, n);

        if (fullRequest.find("\r\n\r\n") != String::npos)
            break;
    }

    const char firstDelimiter[5]  = "\r\n\r\n";
    int        delimiterPos       = fullRequest.find(firstDelimiter);
    if (delimiterPos > 0) {
        uint      firstDelimiterSize = (uint) strlen(firstDelimiter);
        String    headersPart        = fullRequest.substr(0, delimiterPos);
        String bodyPart;

        parseBody(delimiterPos, firstDelimiterSize, fullRequest, bodyPart);
        setBody(bodyPart, req);
        parseMethodPathAndVersion(headersPart, req);

        debugRequestHeaders(headersPart, req, fullRequest);
    }

    router.handle(&req, &res);
    
    String responseStr = res.serialize();
    send(clientSocket, responseStr.c_str(), responseStr.length(), 0);
    
    close(clientSocket);
}

void HttpServer::debugRequestHeaders(String &headersPart, HttpRequest &req, String &fullRequest)
{
    // Debug: log parsed request line and path to help diagnose browser 404
/*     
    size_t firstLineEndDbg = headersPart.find("\r\n");
    String requestLineDbg = (firstLineEndDbg != String::npos) ? headersPart.substr(0, firstLineEndDbg) : headersPart;
    fprintf(stderr, "Received request: %s\n", requestLineDbg.c_str());
    fprintf(stderr, "Parsed path: %s\n", req.path.c_str());
    fprintf(stderr, "Raw request (first %zu bytes): %s\n", (size_t)fullRequest.length(), fullRequest.c_str()); 
*/
}

Collection<String> split(const String& text, char delimiter, int limit = -1) {
    Collection<String> result;
    size_t start = 0;
    size_t end = text.find(delimiter);
    int count = 0;

    while (end != String::npos && (limit == -1 || count < limit)) {
        result.add(text.substr(start, end - start));
        start = end + 1;
        end = text.find(delimiter, start);
        count++;
    }
    
    result.add(text.substr(start));
    return result;
}

void HttpServer::parseMethodPathAndVersion(String &headersPart, HttpRequest &req) {
    String requestLine;
    size_t firstLineEnd = headersPart.find("\r\n");

    if (firstLineEnd != String::npos) {
        requestLine = headersPart.substr(0, firstLineEnd);
        
        Collection<String> tokens = split(requestLine, ' ');

        if (tokens.length >= 1) req.method  = tokens.at(0);
        if (tokens.length >= 2) req.path    = tokens.at(1);
        if (tokens.length >= 3) req.version = tokens.at(2);

    } else {
        requestLine = headersPart; 
    }
}

void HttpServer::setBody(String &bodyPart, HttpRequest &req) {
    if (bodyPart.length() > 0) {
        req.body = bodyPart;
    }
}

void HttpServer::parseBody(int delimiterPos, uint firstDelimiterSize, String &fullRequest, String &bodyPart) {
    if (delimiterPos + firstDelimiterSize < fullRequest.length()) {
        String rawbody = fullRequest.substr(delimiterPos + firstDelimiterSize, fullRequest.length());
        bodyPart
            .append("{")
            .append(rawbody);
    }
}

#endif // http_server_hpp