/**
 * Copyright (c) 2025 Kevin Daniel Taylor
 * Licensed under the MIT License (see the LICENSE file in the project root).
 */
#ifndef iserver_hpp
#define iserver_hpp

#include "../../stl/common.hpp"
#include "irouter.hpp"

interface IServer {
    virtual ~IServer() {}
    virtual void start(void) = 0;
    virtual void init(uint32 port = 8081) = 0;
    virtual void bindRouter(IRouter* router) = 0;
};

#endif // iserver_hpp