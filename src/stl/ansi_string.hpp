/**
 * Copyright (c) 2025 Kevin Daniel Taylor
 * Licensed under the MIT License (see the LICENSE file in the project root).
 */
#ifndef ansi_string_hpp
#define ansi_string_hpp

#ifndef static_collection_hpp
#    include "collection.hpp"
#endif // static_collection_hpp

#include <utility>
#include <string.h>
#include <stdio.h>

template< uint STR_CAPACITY = 128 >
struct AnsiString { 
    enum { MAX_STR_CAP = STR_CAPACITY, NULL_CHAR = 0 };
    Collection< char, MAX_STR_CAP > content;

    /**
     * Constructors family...
     */
    AnsiString(const char str[], uint size);
    AnsiString(const Collection< char >& collection): content(collection) {}
    AnsiString(const char str[]);
    AnsiString();

    /**
     * Initialize family functions...
     */

    void init(void);
    void init(const char str[], uint size);
    AnsiString& init(const char *str);
    AnsiString& operator = (const char* str);

    /** 
     * Query family functions...
     */

    bool        operator == (const AnsiString& rhs);
    const bool  operator == (const AnsiString& rhs) const;
    bool        operator != (const AnsiString& rhs);
    int         pos(const char& item) const;
    int         pos(const char* pattern) const;
    int         pos(AnsiString& string) const;
    uint        length(void) const;
    bool        greatherThan(const AnsiString& rhs) const;
    bool        greatherOrEqualThan(const AnsiString& rhs) const;
    bool        lessThan(const AnsiString& rhs) const;
    bool        lessOrEqualThan(const AnsiString& rhs) const;
    bool        equals(const AnsiString& rhs) const;
    char*       cstr(void);
    const char* cstr(void) const;
    AnsiString  subStr(uint start, uint end);
    char        at(uint idx) const;
    AnsiString& toLower();
    AnsiString& toUpper();

    static AnsiString< STR_CAPACITY > toString(const int& value);
    static AnsiString< STR_CAPACITY > toString(const double& value);
    static AnsiString< STR_CAPACITY > toString(const float& value);
    static AnsiString< STR_CAPACITY > toString(const uint& value);

    template <typename... Args>
    static AnsiString format(AnsiString& str, Args&& ...args);

    template <typename... Args>
    static AnsiString format(const char* str, Args&& ...args);

    void              initKmpPartialMatchTable(const char* pattern, int partialMatch[]) const;
    Collection< int > ocurrencesOf(AnsiString& pattern);

    uint size(void) const;
    
    /**
     * Modify family functions...
     */

    AnsiString& concat(const char* plainString);
    AnsiString& concat(const char c);
    AnsiString& concat(AnsiString& string);
    AnsiString& trim(void);

    /**
     * Iterator family functions...
     */

    Collection< char, STR_CAPACITY >::Iterator begin(void);
    Collection< char, STR_CAPACITY >::Iterator end(void);
};

typedef AnsiString< 256 > SafeString;
typedef SafeString        String;

template< uint STR_CAPACITY >
AnsiString< STR_CAPACITY >::AnsiString(const char str[], uint size) {
    init(str, size);
}

template< uint STR_CAPACITY >
AnsiString< STR_CAPACITY >::AnsiString(const char str[]) {
    init(str);
}

template< uint STR_CAPACITY >
AnsiString< STR_CAPACITY >::AnsiString() {
    init();
}

template< uint STR_CAPACITY >
void AnsiString< STR_CAPACITY >::init(void) {
    memset(content.items, NULL_CHAR, STR_CAPACITY);
}

template< uint STR_CAPACITY >
void AnsiString< STR_CAPACITY >::init(const char str[], uint size) {
    init();
    content.init(str, size);
}

template< uint STR_CAPACITY >
AnsiString< STR_CAPACITY >& AnsiString< STR_CAPACITY >::init(const char str[]) {
    init();
    content.init(str, strlen(str));
    return *this;
}

template< uint STR_CAPACITY >
AnsiString< STR_CAPACITY >& AnsiString< STR_CAPACITY >::operator = (const char* str) {
    return init(str);
}

template< uint STR_CAPACITY >
bool AnsiString< STR_CAPACITY >::operator == (const AnsiString< STR_CAPACITY >& rhs) {
    return equals(rhs);
}

template< uint STR_CAPACITY >
const bool AnsiString< STR_CAPACITY >::operator == (const AnsiString< STR_CAPACITY >& rhs) const {
    return equals(rhs);
}

template< uint STR_CAPACITY >
bool AnsiString< STR_CAPACITY >::operator != (const AnsiString< STR_CAPACITY >& rhs) {
    return !equals(rhs);
}

template< uint STR_CAPACITY >
int AnsiString< STR_CAPACITY >::pos(const char& item) const {
    int  notFound = -1;
    uint idx      = 0;
    for (; idx < (content.length) && (content.at(idx) != item); idx++) ;
    return (idx < content.length) ? idx : notFound;
}

template< uint STR_CAPACITY >
int AnsiString< STR_CAPACITY >::pos(const char* pattern) const {
    SA_ASSERT(content.initialized, "String not initialized!");

    uint patternLength = strlen(pattern);
    if (patternLength == 0) return -1;

    uint stringLength = length();
    int partialMatch[patternLength];

    initKmpPartialMatchTable(pattern, partialMatch);

    int currentStrChar  = 0;
    int currentPatChar = 0;
    while (currentStrChar < (int) stringLength) {
        if (currentPatChar == -1) {
            currentStrChar++;
            currentPatChar = 0;
        } else if (at(currentStrChar) == pattern[currentPatChar]) {
            currentStrChar++;
            currentPatChar++;
            if (currentPatChar == (int) patternLength) return currentStrChar - currentPatChar;
        } else {
            currentPatChar = partialMatch[currentPatChar];
        }
    }

    return -1;
}

template< uint STR_CAPACITY >
int AnsiString< STR_CAPACITY >::pos(AnsiString& string) const {
    return pos(string.cstr());
}

template< uint STR_CAPACITY >
uint AnsiString< STR_CAPACITY >::length(void) const {
    return content.length;
}

template< uint STR_CAPACITY >
bool AnsiString< STR_CAPACITY >::greatherThan(const AnsiString& rhs) const {
    return length() > rhs.length();
}

template< uint STR_CAPACITY >
bool AnsiString< STR_CAPACITY >::greatherOrEqualThan(const AnsiString& rhs) const {
    return length() >= rhs.length();
}

template< uint STR_CAPACITY >
bool AnsiString< STR_CAPACITY >::lessThan(const AnsiString& rhs) const {
    return length() < rhs.length();
}

template< uint STR_CAPACITY >
bool AnsiString< STR_CAPACITY >::lessOrEqualThan(const AnsiString& rhs) const {
    return length() <= rhs.length();
}

template< uint STR_CAPACITY >
bool AnsiString< STR_CAPACITY >::equals(const AnsiString& rhs) const {
    return content == rhs.content;
}

template< uint STR_CAPACITY >
char* AnsiString< STR_CAPACITY >::cstr(void) {
    content.items[content.length] = '\0';
    return content.items;
}

template< uint STR_CAPACITY >
const char* AnsiString< STR_CAPACITY >::cstr(void) const {
    return content.items;
}

template< uint STR_CAPACITY >
AnsiString< STR_CAPACITY > AnsiString< STR_CAPACITY >::subStr(uint start, uint end) {
    Collection< char > selected = content.slice(start, end);
    return { selected };
}

template< uint STR_CAPACITY >
char AnsiString< STR_CAPACITY >::at(uint idx) const {
    return content.at(idx);
}

template< uint STR_CAPACITY >
AnsiString< STR_CAPACITY >& AnsiString< STR_CAPACITY >::toLower() {
    for (auto&& it = begin(); it != end(); ++it) {
        if (*it >= 'A' && *it <= 'Z') {
            *it += 32;
        }
    }

    return *this;
}

template< uint STR_CAPACITY >
AnsiString< STR_CAPACITY >& AnsiString< STR_CAPACITY >::toUpper() {
    for (auto&& it = begin(); it != end(); ++it) {
        if (*it >= 'a' && *it <= 'z') {
            *it -= 32;
        }
    }

    return *this;
}

template< uint STR_CAPACITY >
AnsiString< STR_CAPACITY > AnsiString< STR_CAPACITY >::toString(const double& value) {
    char primitiveStr[32];
    sprintf(primitiveStr, "%f", value);
    return primitiveStr;
}

template< uint STR_CAPACITY >
AnsiString< STR_CAPACITY >
AnsiString< STR_CAPACITY >::toString(const float& value) {
    char primitiveStr[20];

    sprintf(primitiveStr, "%f", value);
    return primitiveStr;
}

template< uint STR_CAPACITY >
AnsiString< STR_CAPACITY > AnsiString< STR_CAPACITY >::toString(const int& value) {
    char primitiveStr[20];

    sprintf(primitiveStr, "%d", value);
    return primitiveStr;
}

template< uint STR_CAPACITY >
AnsiString< STR_CAPACITY > AnsiString< STR_CAPACITY >::toString(const uint& value) {
    char primitiveStr[20];

    sprintf(primitiveStr, "%u", value);
    return primitiveStr;
}

template< uint STR_CAPACITY >
template< typename... Args > 
AnsiString< STR_CAPACITY > AnsiString< STR_CAPACITY >::format(AnsiString& str, Args&& ...args) {
    char buffer[1024];

    int written = snprintf(buffer, sizeof(buffer), str.cstr(), std::forward<Args>(args)...);

    if (written < 0) {
        buffer[0] = '\0';
    } else if (written >= static_cast<int>(sizeof(buffer))) {
        buffer[sizeof(buffer) - 1] = '\0';
    }

    return { buffer };
}

template< uint STR_CAPACITY >
template< typename... Args > 
AnsiString< STR_CAPACITY > AnsiString< STR_CAPACITY >::format(const char* str, Args&& ...args) {
    char buffer[1024];

    int written = snprintf(buffer, sizeof(buffer), str, std::forward<Args>(args)...);

    if (written < 0) {
        buffer[0] = '\0';
    } else if (written >= static_cast<int>(sizeof(buffer))) {
        buffer[sizeof(buffer) - 1] = '\0';
    }

    return { buffer };
}

template< uint STR_CAPACITY >
void AnsiString< STR_CAPACITY >::initKmpPartialMatchTable(const char* pattern, int partialMatch[]) const {
    uint patternLength = strlen(pattern);
    int currentPatChar = 0;
    partialMatch[0] = -1;
    for (uint i = 1; i < patternLength; i++) {
        currentPatChar = partialMatch[ i - 1];
        while (currentPatChar >= 0) {
            if (pattern[currentPatChar] == pattern[i - 1]) break;
            else currentPatChar = partialMatch[currentPatChar];
        }
        partialMatch[i] = currentPatChar + 1;
    }
}

template< uint STR_CAPACITY >
Collection< int > AnsiString<STR_CAPACITY>::ocurrencesOf(AnsiString& pattern) {
    int stringLength = length();
    int patternLength = pattern.length();

    Collection< int > ocurrences;
    
    if (patternLength != 0 || stringLength != 0) {
        int partialMatch[patternLength];
        initKmpPartialMatchTable(pattern.cstr(), partialMatch);
        int currentStrChar = 0;
        int currentPatChar = 0;
        int matches        = 0;
        while (currentStrChar < stringLength) {
            if (currentPatChar == -1) {
                currentPatChar++;
                currentPatChar = 0;
            } else if (at(currentStrChar) == pattern.at(currentPatChar)) {
                currentStrChar++;
                currentPatChar++;
                if (currentPatChar == patternLength) {
                    ocurrences.add(currentStrChar - currentPatChar);
                    currentPatChar = 0;
                    matches++;
                }
            } else {
                currentPatChar = partialMatch[currentPatChar];
            }
        }
    }
    return ocurrences;
}

template< uint STR_CAPACITY >
uint AnsiString< STR_CAPACITY >::size(void) const {
    return length() + 1;
}

template< uint STR_CAPACITY >
AnsiString< STR_CAPACITY >& AnsiString< STR_CAPACITY >::concat(const char* plainString) {
    content.add(plainString, strlen(plainString));
    return *this;
}

template< uint STR_CAPACITY >
AnsiString< STR_CAPACITY >& AnsiString< STR_CAPACITY >::concat(const char c) {
    content.add(c);
    return *this;
}

template< uint STR_CAPACITY >
AnsiString< STR_CAPACITY >& AnsiString< STR_CAPACITY >::concat(AnsiString& string) {
    content.add(string.content);
    return *this;
}

template< uint STR_CAPACITY >
AnsiString< STR_CAPACITY >& AnsiString< STR_CAPACITY >::trim() {
    const unsigned char SPACE_CODE = 32;

    char *start = content.items;
    char *end   = content.items + length() - 1;

    while ((uint)*start == SPACE_CODE && start++);
    while (end >= start && ((uint)*end == SPACE_CODE) && end--);

    *(end + 1) = '\0';

    content.length = strlen(start);
    
    if (start != content.items) {
        memmove(content.items, start, length() + 1);
    }

    return *this;
}

template< uint STR_CAPACITY >
Collection< char, STR_CAPACITY >::Iterator AnsiString< STR_CAPACITY >::begin(void) {
    return content.begin();
}

template< uint STR_CAPACITY >
Collection< char, STR_CAPACITY >::Iterator AnsiString< STR_CAPACITY >::end(void) {
    return content.end();
}

#endif // ansi_string_hpp