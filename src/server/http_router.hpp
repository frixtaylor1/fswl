#ifndef http_router_hpp
#define http_router_hpp

#include "http_request.hpp"
#include "http_response.hpp"

typedef void (*RequestHandler)(HttpRequest* req, HttpResponse* res);

struct Route {
    SafeString     method;
    SafeString     path;
    RequestHandler handler;
};

#define MAX_ROUTES 20

struct HttpRouter {
    Collection< Route, MAX_ROUTES > routes;

    void addRoute(const char* method, const char* path, RequestHandler handler);
    bool routeRequest(HttpRequest* req, HttpResponse* res);
};

void HttpRouter::addRoute(const char* method, const char* path, RequestHandler handler) {
    routes.tryAdd({method, path, handler});
}

bool HttpRouter::routeRequest(HttpRequest* req, HttpResponse* res) {
    for (auto&& it = routes.begin(); it != routes.end(); ++it) {
        if (it->method == req-> method && it->path == req->path) {
            it->handler(req, res);
            return true;
        }
    }
    
    res->setStatus(404, "Not Found");
    res->setBody("Resource not found");
    return false;
}

#endif // http_router_hpp