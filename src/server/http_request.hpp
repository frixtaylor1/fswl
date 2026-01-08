/**
 * Copyright (c) 2025 Kevin Daniel Taylor
 * Licensed under the MIT License (see the LICENSE file in the project root).
 */
#ifndef http_request_hpp
#define http_request_hpp

#ifndef ansi_string_hpp
#    include "../stl/ansi_string.hpp"
#endif // ansi_string_hpp

#ifndef associative_container_hpp
#    include "../stl/associative_container.hpp"
#endif // associative_container_hpp

struct HttpRequest {
    String method;
    String path;
    String version;
    
    #define MAX_HEADERS 30
    typedef AssociativeContainer<String, String, MAX_HEADERS> HeaderContainer;

    HeaderContainer headers;
    String          body;
    
    void dump(void);
};

void HttpRequest::dump(void) {
    SA_PRINT("Method: %s\n",      method.c_str());
    SA_PRINT("Path: %s\n",        path.c_str());
    SA_PRINT("Version: %s\n",     version.c_str());
    SA_PRINT("Body Length: %u\n", body.length());
    SA_PRINT("Headers:\n");

    HeaderContainer::Iterator it;
    it.init(&headers);
    
    for (it.begin(); *it.key() != headers.end(); it.next()) {
        SA_PRINT("  %s: %s\n", it.key()->c_str(), it.value()->c_str());
    }
}

#endif // http_request_hpp