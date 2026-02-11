/**
 * Copyright (c) 2025 Kevin Daniel Taylor
 * Licensed under the MIT License (see the LICENSE file in the project root).
 */
#ifndef queue_hpp
#define queue_hpp

#include "collection.hpp"

template< class ItemType, uint32 CAPACITY = 128 >
struct Queue {
    bool     enqueue(const ItemType& item);
    ItemType dequeue(void);
    uint32     length() const;
    bool     isEmpty(void);
    bool     isFull(void);
    
    uint32                             head = 0;
    uint32                             tail = 0;
    Collection< ItemType, CAPACITY > items;
};

template< class ItemType, uint32 CAPACITY > 
bool Queue< ItemType, CAPACITY >::enqueue(const ItemType& item) {
    if ((head + 1) % CAPACITY == tail) {
        return false;
    }
    
    items.items[head] = item;

    head = (head + 1) % CAPACITY;

    if (items.length < CAPACITY) items.length++;

    return true;
}

template< class ItemType, uint32 CAPACITY > 
ItemType Queue< ItemType, CAPACITY >::dequeue(void) {
    if (items.length == 0) return {};

    ItemType item = items.items[tail];

    tail = (tail + 1) % CAPACITY;
    items.length--;

    return item;
}

template< class ItemType, uint32 CAPACITY > 
uint32 Queue< ItemType, CAPACITY >::length() const {
    return items.length;
}

template< class ItemType, uint32 CAPACITY > 
bool Queue< ItemType, CAPACITY >::isEmpty(void) {
    return items.isEmpty();
}

template< class ItemType, uint32 CAPACITY > 
bool Queue< ItemType, CAPACITY >::isFull(void) {
    return (head + 1) % CAPACITY == tail;
}

#endif // queue_hpp