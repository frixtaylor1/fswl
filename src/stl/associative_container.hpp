/**
 * Copyright (c) 2025 Kevin Daniel Taylor
 * Licensed under the MIT License (see the LICENSE file in the project root).
 */
#ifndef associative_container_hpp
#define associative_container_hpp

#include "collection.hpp"

/**
 * AssociativeContainer - A map-like data structure with fixed capacity.
 * 
 * This container stores key-value pairs using parallel collections.
 * Current complexity: O(n) for lookups due to linear search.
 */
template< class KeyType, class ValueType, uint CAPACITY = 128 >
struct AssociativeContainer {
    Collection<KeyType,   CAPACITY > keys;
    Collection<ValueType, CAPACITY > values;

    struct Iterator {
        AssociativeContainer* self;
        KeyType*              currentKey;
        ValueType*            currentValue;

        void                  init(AssociativeContainer* itsAssociativeContainer);
        Iterator&             begin(void);
        Iterator              end(void);
        Iterator&             next(void);
        KeyType*              key(void);
        ValueType*            value(void);
    };

    ValueType& add(const KeyType& key, const ValueType& value);

    /** Query family functions... */
    bool             exists(const KeyType& key) const;
    const KeyType&   getKeyAt(uint idx) const;
    KeyType&         getKeyAt(uint idx);
    ValueType&       at(const KeyType& key);
    const ValueType& at(const KeyType& key) const;
    ValueType&       getValue(const KeyType& key);
    const ValueType& getValue(const KeyType& key) const;
    const ValueType& getValueAt(uint idx) const;
    ValueType&       getValueAt(uint idx);
    KeyType&         end(void);
    uint             length(void) const;
};

template< class KeyType, class ValueType, uint CAPACITY >
void AssociativeContainer< KeyType, ValueType, CAPACITY >::Iterator::init(AssociativeContainer< KeyType, ValueType, CAPACITY >* itsAssociativeContainer) {
    self = itsAssociativeContainer;
}

template< class KeyType, class ValueType, uint CAPACITY >
AssociativeContainer< KeyType, ValueType, CAPACITY >::Iterator& AssociativeContainer< KeyType, ValueType, CAPACITY >::Iterator::begin(void) {
    currentKey   = self->keys.begin().current;
    currentValue = self->values.begin().current;
    return *this;
}

template< class KeyType, class ValueType, uint CAPACITY >
AssociativeContainer< KeyType, ValueType, CAPACITY >::Iterator AssociativeContainer< KeyType, ValueType, CAPACITY >::Iterator::end(void) {
    AssociativeContainer< KeyType, ValueType, CAPACITY >::Iterator it;
    it.currentKey   = self->keys.end().current;
    it.currentValue = self->values.end().current;
    
    return it;
}

template< class KeyType, class ValueType, uint CAPACITY >
typename AssociativeContainer< KeyType, ValueType, CAPACITY >::Iterator& AssociativeContainer< KeyType, ValueType, CAPACITY >::Iterator::next(void) {
    currentKey++;
    currentValue++;
    return *this;
}

template< class KeyType, class ValueType, uint CAPACITY >
KeyType* AssociativeContainer<KeyType, ValueType, CAPACITY >::Iterator::key(void) {
    return currentKey;
}

template< class KeyType, class ValueType, uint CAPACITY >
ValueType* AssociativeContainer< KeyType, ValueType, CAPACITY >::Iterator::value(void) {
    return currentValue;
}

template< class KeyType, class ValueType, uint CAPACITY >
ValueType& AssociativeContainer< KeyType, ValueType, CAPACITY >::add(const KeyType& key, const ValueType& value) {
    int slotIdx = keys.indexOf(key);
 
    if (slotIdx == -1) {
        keys.add(key);
        uint keyIdx = keys.indexOf(key);
        values.addAt(keyIdx, value);
        slotIdx = keyIdx; 
    }

    return values.at(slotIdx);
}

template< class KeyType, class ValueType, uint CAPACITY >
bool AssociativeContainer< KeyType, ValueType, CAPACITY >::exists(const KeyType& key) const {
    return keys.indexOf(key) != -1;
}

template< class KeyType, class ValueType, uint CAPACITY >
KeyType& AssociativeContainer< KeyType, ValueType, CAPACITY >::getKeyAt(uint idx) {
    return keys.at(idx);
}

template< class KeyType, class ValueType, uint CAPACITY >
const KeyType& AssociativeContainer< KeyType, ValueType, CAPACITY >::getKeyAt(uint idx) const {
    return keys.at(idx);
}

template< class KeyType, class ValueType, uint CAPACITY >
ValueType& AssociativeContainer< KeyType, ValueType, CAPACITY >::at(const KeyType& key) {
    return values.at(keys.indexOf(key));
}

template< class KeyType, class ValueType, uint CAPACITY >
const ValueType& AssociativeContainer< KeyType, ValueType, CAPACITY >::at(const KeyType& key) const {
    return values.at(keys.indexOf(key));
}

template< class KeyType, class ValueType, uint CAPACITY >
ValueType& AssociativeContainer< KeyType, ValueType, CAPACITY >::getValue(const KeyType& key) {
    return at(key);
}

template< class KeyType, class ValueType, uint CAPACITY >
const ValueType& AssociativeContainer< KeyType, ValueType, CAPACITY >::getValue(const KeyType& key) const {
    return at(key);
}

template< class KeyType, class ValueType, uint CAPACITY >
const ValueType& AssociativeContainer< KeyType, ValueType, CAPACITY >::getValueAt(uint idx) const {
    return values.at(idx);
}

template< class KeyType, class ValueType, uint CAPACITY >
ValueType& AssociativeContainer< KeyType, ValueType, CAPACITY >::getValueAt(uint idx) {
    return values.at(idx);
}

template< class KeyType, class ValueType, uint CAPACITY >
KeyType& AssociativeContainer< KeyType, ValueType, CAPACITY >::end(void) {
    return *keys.end().current;
}

template< class KeyType, class ValueType, uint CAPACITY >
uint AssociativeContainer< KeyType, ValueType, CAPACITY >::length(void) const {
    return keys.length;
}

#endif // associative_container_hpp