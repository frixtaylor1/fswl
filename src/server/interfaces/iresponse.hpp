#ifndef iresponse_hpp
#define iresponse_hpp

#include "../../stl/common.hpp"
#include "../../stl/safe_string.hpp"
#include "../types/http_header.hpp"

typedef HeaderContainer< 10 > ResponseHeaderContainer; 

interface IResponse {
    virtual const ResponseHeaderContainer& getHeaders(void) = 0;
    virtual void setStatus(int code, const char* text) = 0;
    virtual void addHeader(const char* key, const char* value) = 0;
    virtual void setBody(const char* data) = 0;
};

#endif // iresponse_hpp