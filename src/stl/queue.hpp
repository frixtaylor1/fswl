#ifndef queue_hpp
#define queue_hpp

#include "collection.hpp"

template< class ItemType, uint CAPACITY = 128 >
struct Queue {
    Collection< ItemType, CAPACITY > items;

    uint head = 0;
    uint tail = 0;

    bool enqueue(const ItemType& item) {
        if (length() < items.DEFAULT_CAPACITY) {
            items.addAt(head, item); 
            
            head = (head + 1) % items.MAX_COLLECTION_CAPACITY;
            return true;
        }
        return isFull();
    }

    ItemType dequeue(void) {
        if (isEmpty()) return {};

        ItemType item = items.at(tail);
        tail = (tail + 1) % CAPACITY;
        items.length--;

        return item;
    }

    uint length() const {
        return items.length;
    }

    bool isEmpty(void) {
        return items.isEmpty();
    }

    bool isFull(void) {
        return items.isFull();
    }
};

#endif // queue_hpp