#ifndef routes_hpp
#define routes_hpp

#include "controller.hpp"
#include "./server/interfaces/irouter.hpp"

static inline void assignRoutes(IRouter* router) {
    router->add("GET",  "/",          &handleHello);
    router->add("POST", "/something", &handlePost);
    router->add("GET",  "/status",    &handleStatus);
}

#endif // routes_hpp