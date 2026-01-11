#ifndef irequest_hpp
#define irequest_hpp

#include "../../stl/common.hpp"
#include "../../stl/safe_string.hpp"
#include "../types/http_header.hpp"

typedef HeaderContainer< 30 > RequestHeaderContainer;

interface IRequest {
    virtual const String&                 getMethod(void) const = 0;
    virtual const String&                 getPath(void) const = 0;
    virtual const String&                 getVersion(void) const = 0;
    virtual const String&                 getBody(void) const = 0;
    virtual const RequestHeaderContainer& getHeaders(void) const = 0;
    virtual const bool                    addHeader(const String& key, const String& value) = 0;
    virtual const bool                    hasHeader(const String& key) const = 0;
    virtual const String&                 get(const String& key) const = 0;
    virtual void                          dump(void) = 0;
};

#endif // irequest_hpp