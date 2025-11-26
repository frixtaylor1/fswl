/**
 * Copyright (c) 2025 Kevin Daniel Taylor
 * Licensed under the MIT License (see the LICENSE file in the project root).
 */
#ifndef collection_hpp
#define collection_hpp

#include "common.hpp"

/**
 * Collection of items...
 */
template< class ItemType, uint CAPACITY = 128 >
struct Collection {
    enum { DEFAULT_CAPACITY = CAPACITY, MAX_COLLECTION_CAPACITY = DEFAULT_CAPACITY + 1 };
    ItemType items[MAX_COLLECTION_CAPACITY];
    uint     length;
    uint     currentItemPos;
    bool     initialized;

    struct Iterator {
        ItemType* current;
        uint      pos;
        
        Iterator(ItemType* item);
        Iterator(const ItemType* item);

        Collection<ItemType, CAPACITY>::Iterator& next(void);
        const Collection<ItemType, CAPACITY>::Iterator& prev(void);

        uint      currentPos() const;

        ItemType& operator * (void) const;
        ItemType* operator -> (void) const;
        Iterator& operator ++ (void);
        Iterator operator -- (int);

        bool      operator == (const Iterator& other) const;
        bool      operator != (const Iterator& other) const;

        Iterator  operator + (diffptr n) const;
        Iterator  operator - (diffptr n) const;
        diffptr   operator - (const Iterator& other) const;
        Iterator& operator += (diffptr n);
        Iterator& operator -= (diffptr n);

        bool      operator < (const Iterator& other) const;
        bool      operator > (const Iterator& other) const;
        bool      operator <= (const Iterator& other) const;
        bool      operator >= (const Iterator& other) const;
    };
    
    Iterator       begin(uint idx = 0);
    const Iterator begin(void) const;
    Iterator       end(void);
    Iterator       end(uint idx);
    const Iterator end(void) const;
    
    Collection();
    Collection(const Collection< ItemType, CAPACITY >& collection); 
    Collection(ItemType items[], uint nbItems);
    
    void              init(const ItemType items[], uint nbItems);
    void              reset(void);
    void              release(void);

    /**
     * Query family functions... 
     */

    bool              isFull(void);
    bool              isEmpty(void);
    ItemType&         at(uint idx);
    const ItemType&   at(uint idx) const;
    const ItemType&   firstItem();
    const ItemType&   nextItem();
    Collection        slice(uint pos, uint nbItems);
    bool              operator == (const Collection& rhs) const;
    bool              operator != (const Collection& rhs);
    int               indexOf(const ItemType& item);
     const int         indexOf(const ItemType& item) const;
    
    /**
     * Modify family functions...
     */
    const ItemType&   add(const ItemType& item);
    const Collection& add(const ItemType items[], uint nbItems);
    Collection&       add(Collection& rhs);
    const ItemType&   addAt(uint idx, const ItemType& item);
    bool              tryAdd(const ItemType& item);
    bool              tryAdd(const ItemType items[], uint nbItems);
    void              removeAt(uint idx);
    void              forAll(void (*callback)(const ItemType& item));

    /**
     * Private family functions...
     */

    inline void _ensureCollectionCapacity(void);
    inline bool _ensureCollectionCapacityWithoutFail(void);
    inline void _ensureIndexLengthInBounds(uint idx) const;
    inline void _ensureIndexLengthInBounds(const uint& idx);
    inline void _ensureIndexCapacityInBounds(uint idx) const;
    inline void _ensureCapacity(uint nbItems);
    inline void _ensureCapacityAddingArray(uint nbItems);
    inline void _ensureCapacityAddingArrayWithoutFail(uint nbItems);
    inline void _ensureIsInitialized(void);
};

template< class ItemType, uint CAPACITY >
Collection< ItemType, CAPACITY>::Iterator::Iterator(ItemType* item): current(item), pos(0) {}

template< class ItemType, uint CAPACITY >
ItemType& Collection< ItemType, CAPACITY >::Iterator::operator * (void) const { 
    return *current; 
}

template< class ItemType, uint CAPACITY >
ItemType* Collection< ItemType, CAPACITY >::Iterator::operator -> (void) const {
    return current;
}

template< class ItemType, uint CAPACITY >
Collection< ItemType, CAPACITY >::Iterator& Collection< ItemType, CAPACITY >::Iterator::operator ++ (void) {
    return next();
}

template< class ItemType, uint CAPACITY >
Collection< ItemType, CAPACITY >::Iterator& Collection< ItemType, CAPACITY >::Iterator::next(void) {
    ++current;
    ++pos;
    return *this;
}

template< class ItemType, uint CAPACITY >
Collection< ItemType, CAPACITY >::Iterator Collection< ItemType, CAPACITY >::Iterator::operator -- (int) {
    return prev();
}

template< class ItemType, uint CAPACITY >
const Collection< ItemType, CAPACITY >::Iterator& Collection< ItemType, CAPACITY >::Iterator::prev(void) {
    Iterator tmp = *this;
    --current;
    return tmp;
}

template< class ItemType, uint CAPACITY >
uint Collection< ItemType, CAPACITY >::Iterator::currentPos() const { 
    return pos; 
}

template< class ItemType, uint CAPACITY >
bool Collection< ItemType, CAPACITY >::Iterator::operator == (const Iterator& other) const { 
    return current == other.current; 
}

template< class ItemType, uint CAPACITY >
bool Collection< ItemType, CAPACITY >::Iterator::operator != (const Iterator& other) const { 
    return current != other.current; 
}

template< class ItemType, uint CAPACITY >
Collection< ItemType, CAPACITY >::Iterator 
Collection< ItemType, CAPACITY >::Iterator::operator + (diffptr n) const { 
    return Collection< ItemType, CAPACITY >::Iterator(current + n); 
}

template< class ItemType, uint CAPACITY >
Collection< ItemType, CAPACITY >::Iterator 
Collection< ItemType, CAPACITY >::Iterator::operator - (diffptr n) const { 
    return Collection< ItemType, CAPACITY >::Iterator(current - n); 
}

template< class ItemType, uint CAPACITY >
diffptr Collection< ItemType, CAPACITY >::Iterator::operator - (const Collection< ItemType, CAPACITY >::Iterator& other) const {
    return current - other.current; 
}

template< class ItemType, uint CAPACITY >
Collection< ItemType, CAPACITY >::Iterator& Collection< ItemType, CAPACITY >::Iterator::operator += (diffptr n) { 
    current += n; 
    return *this; 
}

template< class ItemType, uint CAPACITY >
Collection< ItemType, CAPACITY >::Iterator& Collection< ItemType, CAPACITY >::Iterator::operator -= (diffptr n) { 
    current -= n; 
    return *this; 
}

template< class ItemType, uint CAPACITY >
bool Collection< ItemType, CAPACITY >::Iterator::operator < (const Collection< ItemType, CAPACITY >::Iterator& other) const { 
    return current < other.current; 
}

template< class ItemType, uint CAPACITY >
bool Collection< ItemType, CAPACITY >::Iterator::operator > (const Collection< ItemType, CAPACITY >::Iterator& other) const { 
    return current > other.current; 
}

template< class ItemType, uint CAPACITY >
bool Collection< ItemType, CAPACITY >::Iterator::operator <= (const Collection< ItemType, CAPACITY >::Iterator& other) const { 
    return current <= other.current; 
}

template< class ItemType, uint CAPACITY >
bool Collection< ItemType, CAPACITY >::Iterator::operator >= (const Collection< ItemType, CAPACITY >::Iterator& other) const {
    return current >= other.current; 
}

template< class ItemType, uint CAPACITY >
Collection< ItemType, CAPACITY >::Iterator Collection< ItemType, CAPACITY >::begin(uint idx) { 
    return Collection< ItemType, CAPACITY >::Iterator(&items[idx]); 
}

template< class ItemType, uint CAPACITY >
const Collection< ItemType, CAPACITY >::Iterator Collection< ItemType, CAPACITY >::begin(void) const {
    return Collection< ItemType, CAPACITY >::Iterator(&items[0]); 
}

template< class ItemType, uint CAPACITY >
Collection< ItemType, CAPACITY >::Iterator Collection< ItemType, CAPACITY >::end(void) { 
    return Collection< ItemType, CAPACITY >::Iterator(&items[length]); 
}

template< class ItemType, uint CAPACITY >
const Collection< ItemType, CAPACITY >::Iterator Collection< ItemType, CAPACITY >::end(void) const {
    return Collection< ItemType, CAPACITY >::Iterator(&items[length]); 
}

template< class ItemType, uint CAPACITY >
Collection< ItemType, CAPACITY >::Iterator Collection< ItemType, CAPACITY >::end(uint idx) { 
    return Collection< ItemType, CAPACITY >::Iterator(&items[idx]); 
}

template< class ItemType, uint CAPACITY >
Collection< ItemType, CAPACITY >::Collection(): length(0), currentItemPos(0), initialized(true) {}

template< class ItemType, uint CAPACITY >
Collection< ItemType, CAPACITY >::Collection(const Collection< ItemType, CAPACITY >& collection): length(0), currentItemPos(0), initialized(true) {
    init(collection.items, collection.length);
}

template< class ItemType, uint CAPACITY >
Collection< ItemType, CAPACITY >::Collection(ItemType items[], uint nbItems): length(0), currentItemPos(0), initialized(true) {
    init(items, nbItems);
}

template< class ItemType, uint CAPACITY >
void Collection< ItemType, CAPACITY >::init(const ItemType items[], uint nbItems) {
    _ensureCapacity(nbItems);
    initialized = true;

    for (uint idx = 0; idx < nbItems; idx++) {
        add(items[idx]);
    }

    currentItemPos = 0;
}

template< class ItemType, uint CAPACITY >
void Collection< ItemType, CAPACITY >::reset(void) {}

template< class ItemType, uint CAPACITY >
void Collection< ItemType, CAPACITY >::release(void) {}

template< class ItemType, uint CAPACITY >
bool Collection< ItemType, CAPACITY >::isFull(void) {
    return length == DEFAULT_CAPACITY;
}

template< class ItemType, uint CAPACITY >
bool Collection< ItemType, CAPACITY >::isEmpty(void) {
    return length == 0;
}

template< class ItemType, uint CAPACITY >
ItemType& Collection< ItemType, CAPACITY >::at(uint idx) {
    _ensureIndexLengthInBounds(idx);
    return items[idx];
}

template< class ItemType, uint CAPACITY >
const ItemType& Collection< ItemType, CAPACITY >::at(uint idx) const {
    _ensureIndexLengthInBounds(idx);
    return items[idx];
}

template< class ItemType, uint CAPACITY >
const ItemType& Collection< ItemType, CAPACITY >::add(const ItemType& item) {
    _ensureIsInitialized();
    _ensureCollectionCapacity();
    items[length++] = item;
    return item;
}


template< class ItemType, uint CAPACITY >
const Collection< ItemType, CAPACITY >& Collection< ItemType, CAPACITY >::add(const ItemType items[], uint nbItems) {
    _ensureCapacityAddingArray(nbItems);

    for (uint idx = 0; idx < nbItems; idx++) {
        add(items[idx]);
    }

    return *this;
}

template< class ItemType, uint CAPACITY >
Collection< ItemType, CAPACITY >& Collection< ItemType, CAPACITY >::add(Collection< ItemType, CAPACITY >& rhs) {
    _ensureCapacity(rhs.length);

    for (auto&& it = rhs.begin(); it != rhs.end(); ++it) add(*it);
    return *this;
}

template< class ItemType, uint CAPACITY >
const ItemType& Collection< ItemType, CAPACITY >::addAt(uint idx, const ItemType& item) {
    _ensureIndexCapacityInBounds(idx);
    items[idx] = item;
    length++;
    return item;
}

template< class ItemType, uint CAPACITY >
bool Collection< ItemType, CAPACITY >::tryAdd(const ItemType& item) {
    if (!_ensureCollectionCapacityWithoutFail()) return false;

    add(item);

    return true;
}

template< class ItemType, uint CAPACITY >
bool Collection< ItemType, CAPACITY >::tryAdd(const ItemType items[], uint nbItems) {
    if (!_ensureCapacityAddingArrayWithoutFail(nbItems)) return false;

    add(items, nbItems);

    return true;
}

template< class ItemType, uint CAPACITY >
Collection< ItemType, CAPACITY > Collection< ItemType, CAPACITY >::slice(uint pos, uint nbItems) {
    Collection selected;
    for (auto&& it = begin(pos); it != end(nbItems -1); ++it) selected.add(*it);

    return selected;
}

template< class ItemType, uint CAPACITY >
const ItemType& Collection< ItemType, CAPACITY >::firstItem() {
    return at(0); 
}

template< class ItemType, uint CAPACITY >
const ItemType& Collection< ItemType, CAPACITY >::nextItem() {
    currentItemPos++;
    return at(currentItemPos);
}

template< class ItemType, uint CAPACITY >
void Collection< ItemType, CAPACITY >::removeAt(uint idx) {
    _ensureIndexLengthInBounds(idx);

    if (length == 1) {
        length = 0;
        return;
    }

    for (uint i = idx; i < length - 1; i++) {
        items[i] = items[i + 1];
    }
    
    length--;
}

template< class ItemType, uint CAPACITY >
void Collection< ItemType, CAPACITY >::forAll(void (*callback)(const ItemType& item)) {
    for (auto&& it = begin(); it != end(); ++it) callback(*it);
}

template< class ItemType, uint CAPACITY >
bool Collection< ItemType, CAPACITY >::operator == (const Collection< ItemType, CAPACITY >& rhs) const {
    if (rhs.length != length) return false;
    
    for (uint idx = 0; idx < length; idx++) {
        if (at(idx) != rhs.at(idx)) return false;
    }

    return true;
}

template< class ItemType, uint CAPACITY >
bool Collection< ItemType, CAPACITY >::operator != (const Collection< ItemType, CAPACITY >& rhs) {
    return !(*this == rhs);
}

template< class ItemType, uint CAPACITY >
int Collection< ItemType, CAPACITY >::indexOf(const ItemType& item) {
    for (auto&& it = begin(); it != end(); ++it) {
        if (*it == item) return it.currentPos();
    }
    return -1;
}

template< class ItemType, uint CAPACITY >
const int Collection< ItemType, CAPACITY >::indexOf(const ItemType& item) const {
    for (int idx = 0; idx < length; idx++) {
        if (items[idx] == item) return idx;

    }
    return -1;
}

/**
 * Private family functions...
 */

template< class ItemType, uint CAPACITY >
inline void Collection< ItemType, CAPACITY >::_ensureCollectionCapacity(void) {
    SA_ASSERT(length < DEFAULT_CAPACITY, "Max cap reached!");
}

template< class ItemType, uint CAPACITY >
inline bool Collection< ItemType, CAPACITY >::_ensureCollectionCapacityWithoutFail(void) {
    return length < DEFAULT_CAPACITY;
}

template< class ItemType, uint CAPACITY >
inline void Collection< ItemType, CAPACITY >::_ensureIndexLengthInBounds(uint idx) const {
    SA_ASSERT(idx < length, "Index out of bounds!");
}

template< class ItemType, uint CAPACITY >
inline void Collection< ItemType, CAPACITY >::_ensureIndexLengthInBounds(const uint& idx) {
    SA_ASSERT(idx < length, "Index out of bounds!");
}

template< class ItemType, uint CAPACITY >
inline void Collection< ItemType, CAPACITY >::_ensureIndexCapacityInBounds(uint idx) const {
    SA_ASSERT(idx < MAX_COLLECTION_CAPACITY, "Index out of bounds!");
}

template< class ItemType, uint CAPACITY >
inline void Collection< ItemType, CAPACITY >::_ensureCapacity(uint nbItems) {
    SA_ASSERT(nbItems <= MAX_COLLECTION_CAPACITY, "Error not enough capacity!");
}

template< class ItemType, uint CAPACITY >
inline void Collection< ItemType, CAPACITY >::_ensureCapacityAddingArray(uint nbItems) {
    _ensureIsInitialized();
    SA_ASSERT(nbItems + length <= DEFAULT_CAPACITY, "Index out of bound!");
}

template< class ItemType, uint CAPACITY >
inline void Collection< ItemType, CAPACITY >::_ensureCapacityAddingArrayWithoutFail(uint nbItems) {
    return nbItems + length <= DEFAULT_CAPACITY;
}

template < class ItemType, uint CAPACITY >
inline void Collection< ItemType, CAPACITY >::_ensureIsInitialized(void) {
    SA_ASSERT(initialized, "Uninitialised collection!");
}

#endif // collection_hpp