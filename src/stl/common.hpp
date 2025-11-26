/**
 * Copyright (c) 2025 Kevin Daniel Taylor
 * Licensed under the MIT License (see the LICENSE file in the project root).
 */
#ifndef common_hpp
#define common_hpp 

#ifdef DEBUG
#    include <assert.h>
#    include <stdio.h>
#    define SA_ASSERT(cond, msg) assert(cond && msg)
#    define SA_PRINT(...) printf(__VA_ARGS__)
#    define SA_PRINT_ERR(...) fprintf(stderr, __VA_ARGS__)
#else
/** @TODO: MUST IMPLEMENT... */
#endif

typedef unsigned int  uint;
typedef unsigned long ulong;
typedef long long     diffptr;

#endif // common_hpp