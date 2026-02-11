/**
 * Copyright (c) 2025 Kevin Daniel Taylor
 * Licensed under the MIT License (see the LICENSE file in the project root).
 */
#ifndef http_response_hpp
#define http_response_hpp

#include "../../stl/safe_string.hpp"
#include "../interfaces/iresponse.hpp"
#include "../types/http_header.hpp"

struct HttpResponse : implements IResponse{
    #define MaxResponseHeaders 10
    typedef HeaderContainer< MaxResponseHeaders > ResponseHeaderContainer; 

    int                     statusCode;
    SafeString              statusText;
    SafeString              body;
    ResponseHeaderContainer headers;

    HttpResponse();

    void                           init(void);
    void                           setStatus(int code, const char* text);
    void                           addHeader(const char* key, const char* value);
    void                           setBody(const char* data);
    const ResponseHeaderContainer& getHeaders(void);
    String                         serialize();
};

#endif // http_response_hpp