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

#endif // http_router_hpp