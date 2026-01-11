/**
 * Copyright (c) 2025 Kevin Daniel Taylor
 * Licensed under the MIT License (see the LICENSE file in the project root).
 */
#ifndef http_router_hpp
#define http_router_hpp

#include "../interfaces/irouter.hpp"
#include "http_request.hpp"
#include "http_response.hpp"

struct Route {
    String         method;
    String         path;
    RequestHandler handler;
};

#define MAX_ROUTES 20

struct HttpRouter : implements IRouter {
    Collection< Route, MAX_ROUTES > routes;

    void add(const char* method, const char* path, RequestHandler handler) override;
    bool handle(IRequest* req, IResponse* res);
};

void HttpRouter::add(const char* method, const char* path, RequestHandler handler) {
    routes.tryAdd({method, path, handler});
}

bool HttpRouter::handle(IRequest* req, IResponse* res) {
    String reqPath = req->getPath();
    size_t qpos    = reqPath.find('?');
    if (qpos != String::npos) {
        reqPath = reqPath.substr(0, qpos);
    }
    if (reqPath.length() > 1 && reqPath.back() == '/') {
        reqPath.pop_back();
    }

    for (auto&& it = routes.begin(); it != routes.end(); ++it) {
        if (it->method == req->getMethod() && it->path == reqPath) {
            it->handler(req, res);
            return true;
        }
    }
    
    res->setStatus(404, "Not Found");
    res->setBody("Resource not found");
    return false;
}

#endif // http_router_hpp