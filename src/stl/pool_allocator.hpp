/**
 * Copyright (c) 2025 Kevin Daniel Taylor
 * Licensed under the MIT License (see the LICENSE file in the project root).
 */
#ifndef pool_allocator_hpp
#define pool_allocator_hpp

#include "common.hpp"

#include <pthread.h>
#include <cstring>

#ifndef collection_hpp
#   include "collection.hpp"
#endif // collection_hpp

struct PoolAllocator {
    enum { POOL_CAPACITY = 1024 * 1024 * 512 };
    Collection< char, POOL_CAPACITY > arena;
    uint32                              capacity = POOL_CAPACITY;
    char*                             arenaEnd;
    pthread_mutex_t                   allocatorMutex;

    struct __attribute__((packed)) Header {
        uint32 words: 30;
        bool alloced: 1;
        bool reserved: 1;
    };

    typedef uint32 word_t;

    #define $header (Header*)
    #define $void   (void*)
    #define $byte_t (char*)

    PoolAllocator();
    ~PoolAllocator();
    PoolAllocator(const PoolAllocator&) = delete;
    PoolAllocator&  operator=(const PoolAllocator&) = delete;
    void*           alloc(uint32 bytes);
    void            dealloc(void* ptr);
    void*           realloc(void* ptr, uint32 newBytes);
    const Header*   inspectHeader(void* ptr) const;

private:
    word_t          calculateWords(uint32 bytes) const;
    uint32            calculateBytes(word_t words) const;
    Header*         getHeader(void* ptr) const;
    Header*         nextHeader(Header* header) const;
    bool            headerFound(Header* header, word_t requestedWords) const;
    Header*         findBlock(Header* startHeader, word_t requestedWords);
    void*           getBlockArea(Header* header) const;
    void            initializeFirstHeader(void);

};


inline PoolAllocator::word_t PoolAllocator::calculateWords(uint32 bytes) const {
    return (bytes + 3) / 4;
}

inline uint32 PoolAllocator::calculateBytes(word_t words) const {
    return words * 4;
}

inline PoolAllocator::Header* PoolAllocator::getHeader(void* ptr) const {
    return $header ptr - 1;
}

inline PoolAllocator::Header* PoolAllocator::nextHeader(Header* header) const {
    char* data_ptr = $byte_t(header + 1);
    return $header(data_ptr + calculateBytes(header->words));
}

inline bool PoolAllocator::headerFound(Header* header, word_t requestedWords) const {
    return !header->alloced && !header->reserved && (header->words == 0 || header->words >= requestedWords);
}

inline PoolAllocator::Header* PoolAllocator::findBlock(Header* startHeader, word_t requestedWords) {
    Header* currentHeader = startHeader;

    while ($byte_t currentHeader < arenaEnd && !($byte_t(currentHeader + 1) >= arenaEnd)) {
        if (headerFound(currentHeader, requestedWords)) {
            return currentHeader;
        }

        currentHeader = nextHeader(currentHeader);
    }

    return nullptr;
}

inline void* PoolAllocator::getBlockArea(Header* header) const {
    return header + 1;
}

inline void PoolAllocator::initializeFirstHeader() {
    arena.length = capacity;
    char*  basePtr = arena.items;
    Header* h      = reinterpret_cast<Header*>(basePtr);

    uint32 usable = capacity - sizeof(Header);
    word_t words = calculateWords(usable);

    h->words    = words;
    h->alloced  = false;
    h->reserved = false;

    arenaEnd = basePtr + capacity;
}

inline PoolAllocator::PoolAllocator() {
    pthread_mutex_init(&allocatorMutex, nullptr);
    initializeFirstHeader();
}

inline PoolAllocator::~PoolAllocator() {
    pthread_mutex_destroy(&allocatorMutex);
}

inline void* PoolAllocator::alloc(uint32 bytes) {
    word_t requestedWords = calculateWords(bytes);
    
    pthread_mutex_lock(&allocatorMutex);

    Header* header = reinterpret_cast<Header*>(arena.items);

    Header* selected = findBlock(header, requestedWords);
    
    if (!selected) {
        pthread_mutex_unlock(&allocatorMutex);
        return nullptr;
    }

    word_t originalWords = selected->words;
    word_t remainingWords = originalWords - requestedWords;

    if (remainingWords >= (sizeof(Header) / 4) + 1) {
        Header* newHeader = nextHeader(selected);
        
        newHeader->words = remainingWords - (sizeof(Header) / 4);
        newHeader->alloced = false;
        newHeader->reserved = false;

        selected->words = requestedWords;
    } 
    
    selected->alloced = true;
    selected->reserved = false;

    void* block = getBlockArea(selected);

    pthread_mutex_unlock(&allocatorMutex);

    return block;
}

inline void PoolAllocator::dealloc(void* ptr) {
    if (!ptr) return;
    
    pthread_mutex_lock(&allocatorMutex);

    Header* header = getHeader(ptr);

    SA_ASSERT(header->alloced, "Double free detected or invalid pointer!");
    if (!header->alloced) throw std::runtime_error("Double free detected or invalid pointer!");

    header->alloced = false;
    header->reserved = false;

    ::memset(ptr, 0, calculateBytes(header->words));
    
    pthread_mutex_unlock(&allocatorMutex);
}

inline void* PoolAllocator::realloc(void* ptr, uint32 newBytes) {
    if (!ptr) {
        return alloc(newBytes);
    }
    if (newBytes == 0) {
        dealloc(ptr);
        return nullptr;
    }

    void* newPtr = alloc(newBytes);
    if (!newPtr) {
        return nullptr;
    }

    Header* oldHeader = getHeader(ptr);
    uint32    oldSize   = calculateBytes(oldHeader->words);

    size_t copySize = (oldSize < newBytes) ? oldSize : newBytes;
    memcpy(newPtr, ptr, copySize);
    dealloc(ptr);

    return newPtr;
}

inline const PoolAllocator::Header* PoolAllocator::inspectHeader(void* ptr) const {
    return getHeader(ptr);
}

#endif // pool_allocator_hpp