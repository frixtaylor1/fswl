/**
 * Copyright (c) 2025 Kevin Daniel Taylor
 * Licensed under the MIT License (see the LICENSE file in the project root).
 */
#ifndef http_router_hpp
#define http_router_hpp

#include "http_request.hpp"
#include "http_response.hpp"

typedef void (*RequestHandler)(HttpRequest* req, HttpResponse* res);

struct Route {
    String         method;
    String         path;
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
    String reqPath = req->path;
    size_t qpos    = reqPath.find('?');
    if (qpos != String::npos) {
        reqPath = reqPath.substr(0, qpos);
    }
    if (reqPath.length() > 1 && reqPath.back() == '/') {
        reqPath.pop_back();
    }

    for (auto&& it = routes.begin(); it != routes.end(); ++it) {
        if (it->method == req->method && it->path == reqPath) {
            it->handler(req, res);
            return true;
        }
    }
    
    res->setStatus(404, "Not Found");
    res->setBody("Resource not found");
    return false;
}

#endif // http_router_hpp