/**
 * Copyright (c) 2025 Kevin Daniel Taylor
 * Licensed under the MIT License (see the LICENSE file in the project root).
 */
#include "http_server.hpp"

void HttpRequest::dump(void)  {
    SA_PRINT("Method: %s\n",      method.c_str());
    SA_PRINT("Path: %s\n",        path.c_str());
    SA_PRINT("Version: %s\n",     version.c_str());
    SA_PRINT("Body Length: %u\n", body.length());
    SA_PRINT("Headers:\n");

    RequestHeaderContainer::Iterator it;
    it.init(&headers);
    
    for (it.begin(); *it.key() != headers.end(); it.next()) {
        SA_PRINT("  %s: %s\n", it.key()->c_str(), it.value()->c_str());
    }
}

const String& HttpRequest::getMethod(void) const {
    return method;
}

const String& HttpRequest::getPath(void) const {
    return path;
}

const String& HttpRequest::getVersion(void) const {
    return version;
}

const RequestHeaderContainer& HttpRequest::getHeaders(void) const {
    return headers;
}

const String& HttpRequest::getBody(void) const {
    return body;
}


const bool HttpRequest::addHeader(const String& key, const String& value) {
    if (!hasHeader(key)) {
        headers.add(key, value);
        return true;
    }
    return false;
}

const bool HttpRequest::hasHeader(const String& key) const {
    return headers.exists(key);
}

const String& HttpRequest::get(const String& key) const {
    return headers.at(key);
}