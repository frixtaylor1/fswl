#ifndef message_queue_hpp
#define message_queue_hpp

#include "../stl/collection.hpp"

template< class PayloadType, uint32 CAPACITY >
class MessageQueue {
public:
    Collection< PayloadType, CAPACITY > queue;
    pthread_mutex_t                     queueMutex;
    pthread_cond_t                      poolCond;

    bool add(const PayloadType& payload) {
        pthread_mutex_lock(&queueMutex);
        bool success = queue.tryAdd(payload); 

        if (success) {
            pthread_cond_signal(&poolCond);
        }
        
        pthread_mutex_unlock(&queueMutex);
        return success;
    }

    MessageQueue() {
        pthread_mutex_init(&queueMutex, nullptr);
        pthread_cond_init(&poolCond, nullptr);
    }

    ~MessageQueue() {
        pthread_mutex_destroy(&queueMutex);
        pthread_cond_destroy(&poolCond);
    }
};

#endif // message_queue_hpp