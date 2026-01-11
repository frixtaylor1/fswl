/**
 * Copyright (c) 2025 Kevin Daniel Taylor
 * Licensed under the MIT License (see the LICENSE file in the project root).
 */
#ifndef irouter_hpp
#define irouter_hpp

#include "../../stl/common.hpp"
#include "irequest.hpp"
#include "iresponse.hpp"

typedef void (*RequestHandler)(IRequest* req, IResponse* res);

interface IRouter {
    virtual void add(const char* method, const char* path, RequestHandler handler) = 0;
    virtual bool handle(IRequest* req, IResponse* res) = 0;
};

#endif // irouter_hpp