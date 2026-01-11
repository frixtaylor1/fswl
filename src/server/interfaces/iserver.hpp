#ifndef iserver_hpp
#define iserver_hpp

#include "../../stl/common.hpp"

interface IServer {
    virtual ~IServer() {}
    virtual void start(void) = 0;
    virtual void init(uint port) = 0;
};

#endif // iserver_hpp