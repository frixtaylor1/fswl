#ifndef associative_conatiner_hpp
#define associative_conatiner_hpp

#include "collection.hpp"

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
    bool       exists(const KeyType& key);
    ValueType& at(const KeyType& key);
    ValueType& getValue(const KeyType& key);
    KeyType&   end(void);
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
    int slot = keys.indexOf(key);
 
    if (slot == -1) {
        keys.add(key);
        uint keyIdx = keys.indexOf(key);
        values.addAt(keyIdx, value);
        slot = keyIdx; 
    }

    return values.at(slot);
}

template< class KeyType, class ValueType, uint CAPACITY >
bool AssociativeContainer< KeyType, ValueType, CAPACITY >::exists(const KeyType& key) {
    return keys.indexOf(key) != -1;
}

template< class KeyType, class ValueType, uint CAPACITY >
ValueType& AssociativeContainer< KeyType, ValueType, CAPACITY >::at(const KeyType& key) {
    return values.at(keys.indexOf(key));
}

template< class KeyType, class ValueType, uint CAPACITY >
ValueType& AssociativeContainer< KeyType, ValueType, CAPACITY >::getValue(const KeyType& key) {
    return at(key);
}

template< class KeyType, class ValueType, uint CAPACITY >
KeyType& AssociativeContainer< KeyType, ValueType, CAPACITY >::end(void) {
    return *keys.end().current;
}

#endif // associative_container_hpp