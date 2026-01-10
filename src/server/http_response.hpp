#ifndef http_response_hpp
#define http_response_hpp

#include "../stl/safe_string.hpp"
#include "http_header.hpp"

struct HttpResponse {
    #define MaxResponseHeaders 10
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
    
    String serialize() {
        String res;
        
        res += "HTTP/1.1 " + std::to_string(statusCode) + " " + statusText + "\r\n";

        res += "Content-Length: " + std::to_string(body.length()) + "\r\n";
        
        for (uint i = 0; i < headers.length(); ++i) {
            res += headers.keys.at(i) + ": " + headers.values.at(i) + "\r\n";
        }

        res += "\r\n";

        res += body;

        return res;
    }
};

HttpResponse::HttpResponse() {
    init();
}

void HttpResponse::init(void) {
    statusCode  = 200;
    statusText  = "OK";
    body        = "";
    // No agregues headers aquí si los vas a calcular dinámicamente en serialize
}

void HttpResponse::setStatus(int code, const char* text) {
    statusCode = code;
    statusText = text;
}

void HttpResponse::addHeader(const char* key, const char* value) {
    headers.add(key, value);
}

void HttpResponse::setBody(const char* data) {
    body = data;
}
#endif