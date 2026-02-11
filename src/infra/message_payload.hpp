#ifndef message_payload
#define message_payload

#include "../stl/pool_allocator.hpp"

static PoolAllocator g_payloadAllocator;

enum ContentType {
    MSG_TYPE_USER_CREATE,
    MSG_TYPE_ORDER_INSERT,
};

struct MessagePayload {
    void*       content;
    uint32        size;
    ContentType type;
};

#endif // message_payload