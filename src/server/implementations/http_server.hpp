/**
 * Copyright (c) 2025 Kevin Daniel Taylor
 * Licensed under the MIT License (see the LICENSE file in the project root).
 */
#ifndef http_server_hpp
#define http_server_hpp

#include "../../stl/common.hpp"
#include "http_router.hpp"
#include "http_task_queue.hpp"

#include "../../stl/static_collection.hpp"
#include "../interfaces/iserver.hpp"
#include "../interfaces/irouter.hpp"


#define MAX_THREADS 16
#define MAX_CONNECTIONS 1024

struct HttpServer : implements IServer {
    IRouter*     router;
    int          listenSocket;
    int          port;
    
    TaskQueue    taskQueue;
    pthread_t    threadPool[MAX_THREADS];
    int          threadCount;

    void         init(uint32 port = 8081);
    void         start(void);
    void         bindRouter(IRouter* routerImpl) {
        router = routerImpl;
    }

    class ConnectionHandler {
    public:
        ConnectionHandler(HttpServer& server, IRouter* router, int clientSocket);

        void initialize();
        void handle(); 
        void finalize();

    private:
        HttpServer&  server;
        IRouter*     router;
        int          clientSocket;

    private:
        char         buffer[2048];

        HttpRequest  req;
        HttpResponse res;

        String       fullRequest;

        bool         headersComplete      = false;
        uint32       delimiterPos         = String::npos;
        uint32       expectedBodyBytes    = 0;
        const char*  firstDelimiter;
        uint32       firstDelimiterSize;
        String       headersPart;
    };

private:
    static constexpr uint32 MAX_HEADER_BYTES  = 16 * 1024;
    static constexpr uint32 MAX_BODY_BYTES    = 1024 * 1024;
    static constexpr uint32 MAX_REQUEST_BYTES = MAX_HEADER_BYTES + MAX_BODY_BYTES;

    static void* workerRoutine(void* arg);
    void         handleConnection(int clientSocket);
    void         ensureMaxRequestBytesCapacity(String &fullRequest, int clientSocket);
    void         debugRequestHeaders(String &headersPart, HttpRequest &req, String &fullRequest);
    void         parseMethodPathAndVersion(String &headersPart, HttpRequest &req);
    void         setBody(String &bodyPart, HttpRequest &req);
    void         parseBody(int delimiterPos, uint32 firstDelimiterSize, String &fullRequest, String &bodyPart);
    void         cleanup();
    void         handleAcceptError(int clientSocket, int &retFlag);
    void         setupStart(sockaddr_in &serverAddr, bool &retFlag);
    void         initServerAddress(struct sockaddr_in& serverAddr);
    void         listen(bool &retFlag);
    void         bind(sockaddr_in &serverAddr, bool &retFlag);
    void         recicleAddress();
    void         startThreadPool();
    void         sendErrorAndClose(int clientSocket, int statusCode, const char* statusText, const char* message);
    void         parseHeaders(String &headersPart, HttpRequest &req);
    bool         tryParseContentLength(HttpRequest &req, uint32 &contentLength);
};

#endif // http_server_hpp