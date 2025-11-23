#ifndef http_response_hpp
#define http_response_hpp

#ifndef ansi_string_hpp
#    include "../stl/ansi_string.hpp"
#endif // ansi_string_hpp

#include "http_header.hpp"

struct HttpResponse {
    #define MaxResponseHeaders 5
    typedef HeaderContainer< MaxResponseHeaders > ResponseHeaderContainer; 

    int                     statusCode;
    SafeString              statusText;
    SafeString              body;
    ResponseHeaderContainer headers;

    HttpResponse();

    void       init(void);
    void       setStatus(int code, const char* text);
    void       addHeader(const char* key, const char* value);
    void       setBody(const char* data);
    SafeString toString(void);
};

HttpResponse::HttpResponse() {
    init();
}

void HttpResponse::init(void) {
    statusCode  = 200;
    statusText  = "OK";

    addHeader("Content-Type", "text/plain");
    addHeader("Connection", "Close");
}

void HttpResponse::setStatus(int code, const char* text) {
    statusCode = code;
    statusText = text;
}

/**
 * @TODO: Create an Associative Container...
 */
void HttpResponse::addHeader(const char* key, const char* value) {
    headers.add(key, value);
}

void HttpResponse::setBody(const char* data) {
    body = data;
}

SafeString HttpResponse::toString(void) {
    SafeString responseStr;
    
    /** Status line: HTTP/1.1 200 OK */
    SafeString statusLine = SafeString::format("HTTP/1.1 %d %s\r\n", statusCode, statusText.cstr());
    responseStr.concat(statusLine);

    addHeader("Content-Length", SafeString::toString(body.length()).cstr()); 
    
    ResponseHeaderContainer::Iterator it;
    it.init(&headers);

    for (it.begin(); *it.key() != headers.end(); it.next()) {
        responseStr.concat(it.key()->cstr());
        responseStr.concat(": ");
        responseStr.concat(it.value()->cstr());
        responseStr.concat("\r\n");
    }

    responseStr.concat("\r\n");
    responseStr.concat(body);

    return responseStr;
}

#endif // http_response_hpp