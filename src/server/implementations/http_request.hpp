/**
 * Copyright (c) 2025 Kevin Daniel Taylor
 * Licensed under the MIT License (see the LICENSE file in the project root).
 */
#ifndef http_request_hpp
#define http_request_hpp

#include "../interfaces/irequest.hpp"
#include "../../stl/safe_string.hpp"
#include "../types/http_header.hpp"

struct HttpRequest: implements IRequest {
    String method;
    String path;
    String version;
    
    RequestHeaderContainer headers;
    String                 body;

    const String&                 getMethod(void) const;
    const String&                 getPath(void) const;
    const String&                 getVersion(void) const;
    const String&                 getBody(void) const;
    const RequestHeaderContainer& getHeaders(void) const;
    const bool                    addHeader(const String& key, const String& value);
    const bool                    hasHeader(const String& key) const;
    const String&                 get(const String& key) const;
    void                          dump(void);
};

#endif // http_request_hpp