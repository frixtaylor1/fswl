/**
 * Copyright (c) 2025 Kevin Daniel Taylor
 * Licensed under the MIT License (see the LICENSE file in the project root).
 */
#ifndef i_db_connector_hpp
#define i_db_connector_hpp

#ifndef LIBPQ_FE_H
#    include <libpq-fe.h>
#endif // LIBPQ_FE_H

#include "../../stl/static_collection.hpp"
#include "../../stl/common.hpp"
#include "../../stl/safe_string.hpp"

struct DBProps {
    String port;
    String host;
    String dbName;
    String user;
    String password; 
};

interface IDBConnector {
    virtual void  initPool(const DBProps& dbProps) = 0;
    virtual void* getConnection()                  = 0;
    virtual void  releaseConnection(void* conn)  = 0;
};

enum { DB_POOL_CAPACITY = 8 };

struct PooledConnection {
    void* conn  = nullptr;
    bool  inUse = false;
};

#endif // i_db_connector_hpp