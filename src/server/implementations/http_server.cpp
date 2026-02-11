#include "http_server.hpp"
#include <errno.h>
#include <string.h>
#include <algorithm>
#include <cctype>
#include <exception>

void HttpServer::init(uint32 port) {
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
    ConnectionHandler handler(*this, router, clientSocket);
    handler.handle();
    handler.finalize();
}

void HttpServer::ensureMaxRequestBytesCapacity(String &fullRequest, int clientSocket)
{
    if (fullRequest.size() > MAX_REQUEST_BYTES)
    {
        throw std::runtime_error("Request exceeds allowed size");
    }
}

void HttpServer::debugRequestHeaders(String &headersPart, HttpRequest &req, String &fullRequest)
{
    // Debug: log parsed request line and path to help diagnose browser 404
/*     
    uint32 firstLineEndDbg = headersPart.find("\r\n");
    String requestLineDbg = (firstLineEndDbg != String::npos) ? headersPart.substr(0, firstLineEndDbg) : headersPart;
    fprintf(stderr, "Received request: %s\n", requestLineDbg.c_str());
    fprintf(stderr, "Parsed path: %s\n", req.path.c_str());
    fprintf(stderr, "Raw request (first %zu bytes): %s\n", (uint32)fullRequest.length(), fullRequest.c_str()); 
*/
}

Collection<String> split(const String& text, char delimiter, int limit = -1) {
    Collection<String> result;
    uint32 start = 0;
    uint32 end = text.find(delimiter);
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
    uint32 firstLineEnd = headersPart.find("\r\n");

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

void HttpServer::parseBody(int delimiterPos, uint32 firstDelimiterSize, String &fullRequest, String &bodyPart) {
    if (delimiterPos + firstDelimiterSize < fullRequest.length()) {
        String rawbody = fullRequest.substr(delimiterPos + firstDelimiterSize, fullRequest.length());
        bodyPart.append(rawbody);
    }
}

void HttpServer::parseHeaders(String &headersPart, HttpRequest &req) {
    uint32 lineStart = headersPart.find("\r\n");
    if (lineStart == String::npos) {
        return;
    }

    lineStart += 2;

    while (lineStart < headersPart.length()) {
        uint32 lineEnd = headersPart.find("\r\n", lineStart);
        uint32 length  = (lineEnd == String::npos) ? (headersPart.length() - lineStart) : (lineEnd - lineStart);
        String line    = headersPart.substr(lineStart, length);
        lineStart      = (lineEnd == String::npos) ? headersPart.length() : lineEnd + 2;

        if (line.empty()) {
            continue;
        }

        uint32 colonPos = line.find(':');
        if (colonPos == String::npos) {
            continue;
        }

        String key   = line.substr(0, colonPos);
        String value = line.substr(colonPos + 1);

        trim(key);
        trim(value);

        std::transform(key.begin(), key.end(), key.begin(), [](unsigned char ch) {
            return static_cast<char>(std::tolower(ch));
        });

        if (!key.empty()) {
            req.addHeader(key, value);
        }
    }
}

bool HttpServer::tryParseContentLength(HttpRequest &req, uint32 &contentLength) {
    static const String contentLengthKey("content-length");

    if (!req.hasHeader(contentLengthKey)) {
        contentLength = 0;
        return true;
    }

    const String& rawValue = req.get(contentLengthKey);
    if (rawValue.empty()) {
        return false;
    }

    try {
        contentLength = static_cast<uint32>(std::stoull(rawValue));
    } catch (const std::exception&) {
        return false;
    }

    return true;
}

void HttpServer::sendErrorAndClose(int clientSocket, int statusCode, const char* statusText, const char* message) {
    HttpResponse errRes;
    errRes.setStatus(statusCode, statusText);
    errRes.addHeader("Content-Type", "text/plain; charset=utf-8");
    errRes.setBody(message);

    String payload = errRes.serialize();
    send(clientSocket, payload.c_str(), payload.length(), 0);
    close(clientSocket);
}

HttpServer::ConnectionHandler::ConnectionHandler(HttpServer& server, IRouter* router, int clientSocket) : server(server), router(router), clientSocket(clientSocket) {
    initialize();
}

void HttpServer::ConnectionHandler::initialize() {
    fullRequest.reserve(sizeof(buffer));
    firstDelimiter = "\r\n\r\n";
    firstDelimiterSize = sizeof(firstDelimiter) - 1;
}

void HttpServer::ConnectionHandler::handle() {
    while (true) {
        int bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);
        if (bytesReceived <= 0) {
            break;
        }

        fullRequest.append(buffer, bytesReceived);

        try {
            server.ensureMaxRequestBytesCapacity(fullRequest, clientSocket);
        } catch (const std::exception& e) {
            SA_PRINT_ERR("Error processing request: %s\n", e.what());
            server.sendErrorAndClose(clientSocket, 413, "Payload Too Large", "Request exceeds allowed size");
            return;
        }

        if (!headersComplete) {
            delimiterPos = fullRequest.find(firstDelimiter);

            if (delimiterPos != String::npos) {
                headersComplete = true;
                headersPart     = fullRequest.substr(0, delimiterPos);

                server.parseMethodPathAndVersion(headersPart, req);
                server.parseHeaders(headersPart, req);

                if (req.hasHeader("transfer-encoding")) {
                    server.sendErrorAndClose(clientSocket, 501, "Not Implemented", "Transfer-Encoding is not supported");
                    return;
                }

                if (!server.tryParseContentLength(req, expectedBodyBytes)) {
                    server.sendErrorAndClose(clientSocket, 400, "Bad Request", "Invalid Content-Length header");
                    return;
                }

                if (expectedBodyBytes > MAX_BODY_BYTES) {
                    server.sendErrorAndClose(clientSocket, 413, "Payload Too Large", "Request body exceeds allowed size");
                    return;
                }
            } else if (fullRequest.size() > MAX_HEADER_BYTES) {
                server.sendErrorAndClose(clientSocket, 431, "Request Header Fields Too Large", "Request headers exceed allowed size");
                return;
            }
        }

        if (headersComplete) {
            uint32 headerEnd           = delimiterPos + firstDelimiterSize;
            uint32 bodyBytesAvailable  = (fullRequest.size() > headerEnd) ? (fullRequest.size() - headerEnd) : 0;

            if (bodyBytesAvailable >= expectedBodyBytes) {
                break;
            }
        }
    }
}

void HttpServer::ConnectionHandler::finalize() {
    if (!headersComplete) {
        server.sendErrorAndClose(clientSocket, 400, "Bad Request", "Malformed HTTP request");
        return;
    }

    uint32 requiredBytes = delimiterPos + firstDelimiterSize + expectedBodyBytes;
    if (fullRequest.size() < requiredBytes) {
        server.sendErrorAndClose(clientSocket, 400, "Bad Request", "Incomplete HTTP body");
        return;
    }

    String bodyPart;
    server.parseBody(static_cast<int>(delimiterPos), static_cast<uint32>(firstDelimiterSize), fullRequest, bodyPart);
    server.setBody(bodyPart, req);

    server.debugRequestHeaders(headersPart, req, fullRequest);

    router->handle(&req, &res);
    
    String responseStr = res.serialize();
    send(clientSocket, responseStr.c_str(), responseStr.length(), 0);
    
    close(clientSocket);
}