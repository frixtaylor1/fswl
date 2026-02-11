/**
 * Copyright (c) 2025 Kevin Daniel Taylor
 * Licensed under the MIT License (see the LICENSE file in the project root).
 */
#ifndef postgre_db_connector_hpp
#define postgre_db_connector_hpp

#include "./i_db_connector.hpp"
#include <pthread.h>
#include <sys/time.h>
#include <errno.h>

class PostgreDBConnector: implements IDBConnector {
    typedef Collection< PooledConnection, DB_POOL_CAPACITY > PoolContainer;
public:
    PostgreDBConnector() : pool(), currentProps() {
        pthread_mutex_init(&poolMutex, nullptr);
        pthread_cond_init(&poolCond, nullptr);
    }

    ~PostgreDBConnector() {
        for (auto&& it = pool.begin(); it != pool.end(); ++it) {
            if (it->conn != nullptr) {
                PQfinish((PGconn*) it->conn);
                it->conn = nullptr;
            }
        }
        pthread_mutex_destroy(&poolMutex);
        pthread_cond_destroy(&poolCond); 
    }

    void initPool(const DBProps& dbProps) override {
        currentProps = dbProps;

        AnsiString< 512 > connectionStr = AnsiString< 512 >::format("host=%s port=%s dbname=%s user=%s password=%s",
            dbProps.host.cstr(),
            dbProps.port.cstr(),
            dbProps.dbName.cstr(),
            dbProps.user.cstr(),
            dbProps.password.cstr());

        for (uint32 i = 0; i < DB_POOL_CAPACITY; ++i) {
            PGconn *newConn = PQconnectdb(connectionStr.cstr());
            
            if (PQstatus(newConn) != CONNECTION_OK) {
                PQfinish(newConn);
            } else {
                pool.add({ .conn = newConn });
            }
        }
        
        if (pool.isEmpty()) {
        }
    }

    #define DB_POOL_TIMEOUT_MS 100

    void* getConnection() override {
        PGconn*           conn       = nullptr;
        PooledConnection* pooledItem = nullptr;
        
        pthread_mutex_lock(&poolMutex); 

        while (conn == nullptr) {
            findConnection(pooledItem, conn);

            if (conn == nullptr) {
                struct timespec timeout = getTimeout();
                int wait_status         = pthread_cond_timedwait(&poolCond, &poolMutex, &timeout);
                
                if (wait_status == ETIMEDOUT) {
                    break; 
                }
            }
        }
        
        pthread_mutex_unlock(&poolMutex); 

        if (conn != nullptr) {
        }

        if (conn == nullptr) {
        }

        return (void*) conn;
    }


    void releaseConnection(void* conn) override {
        if ((PGconn*) conn == nullptr) return;
        
        pthread_mutex_lock(&poolMutex);

        for (PoolContainer::Iterator&& it = pool.begin(); it != pool.end(); ++it) {
            if ((PGconn*) it->conn == (PGconn*) conn) {
                it->inUse = false;
                pthread_cond_signal(&poolCond);
                break;
            }
        }

        pthread_mutex_unlock(&poolMutex);
    }

private:
    struct timespec getTimeout(void) {
        struct timespec timeout;
        struct timeval  now;
        gettimeofday(&now, nullptr);                
            
        long nsec       = (now.tv_usec + DB_POOL_TIMEOUT_MS * 1000) * 1000;
        timeout.tv_sec  = now.tv_sec + (nsec / 1000000000L);
        timeout.tv_nsec = nsec % 1000000000L;

        return timeout;
    } 

    void findConnection(PooledConnection*& pooledItem, PGconn*& conn) {
        for (PoolContainer::Iterator&& it = pool.begin(); it != pool.end(); ++it) {
            if (it->conn != nullptr && !it->inUse) {
                pooledItem = &*it;
                pooledItem->inUse = true;
                conn = (PGconn*) pooledItem->conn;
                break;
            }
        }
    }

    PoolContainer    pool;
    DBProps          currentProps;
    pthread_mutex_t  poolMutex;
    pthread_cond_t   poolCond;
};

#endif // postre_db_connector_hpp