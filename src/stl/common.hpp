#ifndef common_hpp
#define common_hpp 

#ifdef DEBUG
#    include <assert.h>
#    include <stdio.h>
#    define SA_ASSERT(cond, msg) assert(cond && msg)
#    define SA_PRINT(...) printf(__VA_ARGS__)
#    define SA_PRINT_ERR(...) fprintf(stderr, __VA_ARGS__)
#else
#    define SA_ASSERT(cond, msg) do { (void)(cond); (void)(msg); } while(0)
#    define SA_PRINT(...) do { (void)__VA_ARGS__ } while(0)
#    define SA_PRINT_ERR(...) do { (void)__VA_ARGS__ } while(0)
#endif

typedef unsigned int  uint;
typedef unsigned long ulong;
typedef long long     diffptr;

#endif // common_hpp