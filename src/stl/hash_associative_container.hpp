/**
 * Copyright (c) 2025 Kevin Daniel Taylor
 * Licensed under the MIT License (see the LICENSE file in the project root).
 */
#ifndef hash_associative_container_hpp
#define hash_associative_container_hpp

#include "common.hpp"
#include "./pool_allocator.hpp"

/**
 * HashAssociativeContainer - A hash-based map data structure with fixed capacity.
 * 
 * This container stores key-value pairs using hash table with separate chaining.
 * Uses PoolAllocator for efficient memory management.
 * Current complexity: O(1) average for lookups, O(n) worst case for hash collisions.
 * Better performance than linear search-based containers for large datasets.
 */
template< class KeyType, class ValueType, uint32 CAPACITY = 128 >
struct HashAssociativeContainer {
    
    struct Entry {
        KeyType*    key;
        ValueType*  value;
        Entry*      next;
    };
    
    Entry**           table;
    uint32              entryCount;
    uint32              bucketCount;
    uint32              capacity;
    PoolAllocator*    allocator;

    struct Iterator {
        HashAssociativeContainer* self;
        uint32                      currentBucket;
        Entry*                    currentEntry;

        void                  init(HashAssociativeContainer* container);
        Iterator&             begin(void);
        Iterator              end(void);
        Iterator&             next(void);
        KeyType*              key(void);
        ValueType*            value(void);
    };

    HashAssociativeContainer();
    HashAssociativeContainer(PoolAllocator* poolAlloc);
    ~HashAssociativeContainer();

    ValueType& add(const KeyType& key, const ValueType& value);

    /** Query family functions... */
    bool             exists(const KeyType& key) const;
    const KeyType&   getKeyAt(uint32 idx) const;
    KeyType&         getKeyAt(uint32 idx);
    ValueType&       at(const KeyType& key);
    const ValueType& at(const KeyType& key) const;
    ValueType&       getValue(const KeyType& key);
    const ValueType& getValue(const KeyType& key) const;
    const ValueType& getValueAt(uint32 idx) const;
    ValueType&       getValueAt(uint32 idx);
    KeyType&         end(void);
    uint32             length(void) const;
    
    /** Modify family functions... */
    bool             remove(const KeyType& key);
    void             clear(void);

private:
    Entry*            _findEntry(const KeyType& key) const;
    uint32              _hash(const KeyType& key) const;
    void              _deleteChain(Entry* entry);
};

template< class KeyType, class ValueType, uint32 CAPACITY >
HashAssociativeContainer< KeyType, ValueType, CAPACITY >::HashAssociativeContainer()
    : entryCount(0), bucketCount(CAPACITY), capacity(CAPACITY), allocator(nullptr) {
    table = new Entry*[CAPACITY]();
    for (uint32 i = 0; i < CAPACITY; ++i) {
        table[i] = nullptr;
    }
}

template< class KeyType, class ValueType, uint32 CAPACITY >
HashAssociativeContainer< KeyType, ValueType, CAPACITY >::HashAssociativeContainer(PoolAllocator* poolAlloc)
    : entryCount(0), bucketCount(CAPACITY), capacity(CAPACITY), allocator(poolAlloc) {
    if (allocator != nullptr) {
        table = (Entry**)allocator->alloc(CAPACITY * sizeof(Entry*));
    } else {
        table = new Entry*[CAPACITY]();
    }
    for (uint32 i = 0; i < CAPACITY; ++i) {
        table[i] = nullptr;
    }
}

template< class KeyType, class ValueType, uint32 CAPACITY >
HashAssociativeContainer< KeyType, ValueType, CAPACITY >::~HashAssociativeContainer() {
    clear();
    if (allocator != nullptr) {
        allocator->dealloc(table);
    } else {
        delete[] table;
    }
    table = nullptr;
}

template< class KeyType, class ValueType, uint32 CAPACITY >
uint32 HashAssociativeContainer< KeyType, ValueType, CAPACITY >::_hash(const KeyType& key) const {
    // Simple hash function for integer types
    // For custom types, specialize this function
    if constexpr (std::is_integral_v<KeyType>) {
        return (static_cast<uint32>(key) * 2654435761U) % bucketCount;
    } else {
        // Fallback for non-integral types
        // This is a basic implementation - can be optimized
        return 0;
    }
}

template< class KeyType, class ValueType, uint32 CAPACITY >
typename HashAssociativeContainer< KeyType, ValueType, CAPACITY >::Entry*
HashAssociativeContainer< KeyType, ValueType, CAPACITY >::_findEntry(const KeyType& key) const {
    uint32 hashIdx = _hash(key);
    Entry* entry = table[hashIdx];
    
    while (entry != nullptr) {
        if (*(entry->key) == key) {
            return entry;
        }
        entry = entry->next;
    }
    
    return nullptr;
}

template< class KeyType, class ValueType, uint32 CAPACITY >
void HashAssociativeContainer< KeyType, ValueType, CAPACITY >::Iterator::init(
    HashAssociativeContainer< KeyType, ValueType, CAPACITY >* container) {
    self = container;
    currentBucket = 0;
    currentEntry = nullptr;
}

template< class KeyType, class ValueType, uint32 CAPACITY >
HashAssociativeContainer< KeyType, ValueType, CAPACITY >::Iterator&
HashAssociativeContainer< KeyType, ValueType, CAPACITY >::Iterator::begin(void) {
    currentBucket = 0;
    currentEntry = nullptr;
    
    for (uint32 i = 0; i < self->bucketCount; ++i) {
        if (self->table[i] != nullptr) {
            currentBucket = i;
            currentEntry = self->table[i];
            return *this;
        }
    }
    
    return *this;
}

template< class KeyType, class ValueType, uint32 CAPACITY >
HashAssociativeContainer< KeyType, ValueType, CAPACITY >::Iterator
HashAssociativeContainer< KeyType, ValueType, CAPACITY >::Iterator::end(void) {
    Iterator it;
    it.self = self;
    it.currentBucket = self->bucketCount;
    it.currentEntry = nullptr;
    
    return it;
}

template< class KeyType, class ValueType, uint32 CAPACITY >
typename HashAssociativeContainer< KeyType, ValueType, CAPACITY >::Iterator&
HashAssociativeContainer< KeyType, ValueType, CAPACITY >::Iterator::next(void) {
    if (currentEntry == nullptr) {
        return *this;
    }
    
    if (currentEntry->next != nullptr) {
        currentEntry = currentEntry->next;
        return *this;
    }
    
    currentBucket++;
    currentEntry = nullptr;
    
    for (uint32 i = currentBucket; i < self->bucketCount; ++i) {
        if (self->table[i] != nullptr) {
            currentBucket = i;
            currentEntry = self->table[i];
            return *this;
        }
    }
    
    currentBucket = self->bucketCount;
    return *this;
}

template< class KeyType, class ValueType, uint32 CAPACITY >
KeyType* HashAssociativeContainer< KeyType, ValueType, CAPACITY >::Iterator::key(void) {
    return currentEntry != nullptr ? currentEntry->key : nullptr;
}

template< class KeyType, class ValueType, uint32 CAPACITY >
ValueType* HashAssociativeContainer< KeyType, ValueType, CAPACITY >::Iterator::value(void) {
    return currentEntry != nullptr ? currentEntry->value : nullptr;
}

template< class KeyType, class ValueType, uint32 CAPACITY >
ValueType& HashAssociativeContainer< KeyType, ValueType, CAPACITY >::add(
    const KeyType& key, const ValueType& value) {
    
    Entry* entry = _findEntry(key);
    
    if (entry != nullptr) {
        *(entry->value) = value;
        return *(entry->value);
    }
    
    // Create new entry
    if (entryCount >= capacity) {
        /**
         * @todo: Capacity exceeded - return reference to prevent crash.
         * Cosider resize or error handling for production.
         */
        static ValueType dummy;
        return dummy;
    }
    
    Entry* newEntry;
    if (allocator != nullptr) {
        newEntry = (Entry*)allocator->alloc(sizeof(Entry));
    } else {
        newEntry = new Entry();
    }
    
    if (newEntry == nullptr) {
        static ValueType dummy;
        return dummy;
    }
    
    if (allocator != nullptr) {
        newEntry->key = (KeyType*)allocator->alloc(sizeof(KeyType));
        newEntry->value = (ValueType*)allocator->alloc(sizeof(ValueType));
    } else {
        newEntry->key = new KeyType();
        newEntry->value = new ValueType();
    }
    
    if (newEntry->key == nullptr || newEntry->value == nullptr) {
        static ValueType dummy;
        return dummy;
    }
    
    new (newEntry->key) KeyType(key);
    new (newEntry->value) ValueType(value);
    
    uint32 hashIdx = _hash(key);
    
    newEntry->next = table[hashIdx];
    table[hashIdx] = newEntry;
    entryCount++;
    
    return *(newEntry->value);
}

template< class KeyType, class ValueType, uint32 CAPACITY >
bool HashAssociativeContainer< KeyType, ValueType, CAPACITY >::exists(const KeyType& key) const {
    return _findEntry(key) != nullptr;
}

template< class KeyType, class ValueType, uint32 CAPACITY >
KeyType& HashAssociativeContainer< KeyType, ValueType, CAPACITY >::getKeyAt(uint32 idx) {
    uint32 count = 0;
    
    for (uint32 i = 0; i < bucketCount; ++i) {
        Entry* entry = table[i];
        while (entry != nullptr) {
            if (count == idx) {
                return *(entry->key);
            }
            count++;
            entry = entry->next;
        }
    }
    
    static KeyType dummy;
    return dummy;
}

template< class KeyType, class ValueType, uint32 CAPACITY >
const KeyType& HashAssociativeContainer< KeyType, ValueType, CAPACITY >::getKeyAt(uint32 idx) const {
    uint32 count = 0;
    
    for (uint32 i = 0; i < bucketCount; ++i) {
        Entry* entry = table[i];
        while (entry != nullptr) {
            if (count == idx) {
                return *(entry->key);
            }
            count++;
            entry = entry->next;
        }
    }
    
    static KeyType dummy;
    return dummy;
}

template< class KeyType, class ValueType, uint32 CAPACITY >
ValueType& HashAssociativeContainer< KeyType, ValueType, CAPACITY >::at(const KeyType& key) {
    Entry* entry = _findEntry(key);
    
    if (entry != nullptr) {
        return *(entry->value);
    }
    
    static ValueType dummy;
    return dummy;
}

template< class KeyType, class ValueType, uint32 CAPACITY >
const ValueType& HashAssociativeContainer< KeyType, ValueType, CAPACITY >::at(const KeyType& key) const {
    Entry* entry = _findEntry(key);
    
    if (entry != nullptr) {
        return *(entry->value);
    }
    
    static ValueType dummy;
    return dummy;
}

template< class KeyType, class ValueType, uint32 CAPACITY >
ValueType& HashAssociativeContainer< KeyType, ValueType, CAPACITY >::getValue(const KeyType& key) {
    return at(key);
}

template< class KeyType, class ValueType, uint32 CAPACITY >
const ValueType& HashAssociativeContainer< KeyType, ValueType, CAPACITY >::getValue(const KeyType& key) const {
    return at(key);
}

template< class KeyType, class ValueType, uint32 CAPACITY >
const ValueType& HashAssociativeContainer< KeyType, ValueType, CAPACITY >::getValueAt(uint32 idx) const {
    uint32 count = 0;
    
    for (uint32 i = 0; i < bucketCount; ++i) {
        Entry* entry = table[i];
        while (entry != nullptr) {
            if (count == idx) {
                return *(entry->value);
            }
            count++;
            entry = entry->next;
        }
    }
    
    static ValueType dummy;
    return dummy;
}

template< class KeyType, class ValueType, uint32 CAPACITY >
ValueType& HashAssociativeContainer< KeyType, ValueType, CAPACITY >::getValueAt(uint32 idx) {
    uint32 count = 0;
    
    for (uint32 i = 0; i < bucketCount; ++i) {
        Entry* entry = table[i];
        while (entry != nullptr) {
            if (count == idx) {
                return *(entry->value);
            }
            count++;
            entry = entry->next;
        }
    }
    
    static ValueType dummy;
    return dummy;
}

template< class KeyType, class ValueType, uint32 CAPACITY >
KeyType& HashAssociativeContainer< KeyType, ValueType, CAPACITY >::end(void) {
    static KeyType dummy;
    return dummy;
}

template< class KeyType, class ValueType, uint32 CAPACITY >
uint32 HashAssociativeContainer< KeyType, ValueType, CAPACITY >::length(void) const {
    return entryCount;
}

template< class KeyType, class ValueType, uint32 CAPACITY >
bool HashAssociativeContainer< KeyType, ValueType, CAPACITY >::remove(const KeyType& key) {
    uint32 hashIdx = _hash(key);
    Entry* entry = table[hashIdx];
    Entry* prev = nullptr;
    
    while (entry != nullptr) {
        if (*(entry->key) == key) {
            if (prev != nullptr) {
                prev->next = entry->next;
            } else {
                table[hashIdx] = entry->next;
            }
            
            entry->key->~KeyType();
            entry->value->~ValueType();
            
            if (allocator != nullptr) {
                allocator->dealloc(entry->key);
                allocator->dealloc(entry->value);
                allocator->dealloc(entry);
            } else {
                delete entry->key;
                delete entry->value;
                delete entry;
            }
            
            entryCount--;
            return true;
        }
        
        prev = entry;
        entry = entry->next;
    }
    
    return false;
}

template< class KeyType, class ValueType, uint32 CAPACITY >
void HashAssociativeContainer< KeyType, ValueType, CAPACITY >::_deleteChain(Entry* entry) {
    while (entry != nullptr) {
        Entry* next = entry->next;
        
        entry->key->~KeyType();
        entry->value->~ValueType();
        
        if (allocator != nullptr) {
            allocator->dealloc(entry->key);
            allocator->dealloc(entry->value);
            allocator->dealloc(entry);
        } else {
            delete entry->key;
            delete entry->value;
            delete entry;
        }
        
        entry = next;
    }
}

template< class KeyType, class ValueType, uint32 CAPACITY >
void HashAssociativeContainer< KeyType, ValueType, CAPACITY >::clear(void) {
    for (uint32 i = 0; i < bucketCount; ++i) {
        if (table[i] != nullptr) {
            _deleteChain(table[i]);
            table[i] = nullptr;
        }
    }
    entryCount = 0;
}

#endif // hash_associative_container_hpp
