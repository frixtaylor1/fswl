#ifndef http_request_hpp
#define http_request_hpp

#ifndef ansi_string_hpp
#    include "../stl/ansi_string.hpp"
#endif // ansi_string_hpp

#ifndef associative_container_hpp
#    include "../stl/associative_container.hpp"
#endif // associative_container_hpp

struct HttpRequest {
    SafeString method;
    SafeString path;
    SafeString version;
    
    #define MAX_HEADERS 10
    typedef AssociativeContainer< AnsiString< 20 >, AnsiString< 64 >, MAX_HEADERS> HeaderContainer;

    HeaderContainer headers;
    SafeString      body;
    
    void dump(void);
};

void HttpRequest::dump(void) {
    SA_PRINT("Method: %s\n",      method.cstr());
    SA_PRINT("Path: %s\n",        path.cstr());
    SA_PRINT("Version: %s\n",     version.cstr());
    SA_PRINT("Body Length: %u\n", body.length());
    SA_PRINT("Headers:\n");

    HeaderContainer::Iterator it;
    it.init(&headers);
    
    for (it.begin(); *it.key() != headers.end(); it.next()) {
        SA_PRINT("  %s: %s\n", it.key()->cstr(), it.value()->cstr());
    }
}

#endif // http_request_hpp