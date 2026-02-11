/**
 * Copyright (c) 2025 Kevin Daniel Taylor
 * Licensed under the MIT License (see the LICENSE file in the project root).
 */
#ifndef safe_string_hpp
#define safe_string_hpp

#include <algorithm>
#include <cctype>
#include <string>
#include <fmt/core.h>
#include "common.hpp"

typedef std::string SafeString;
typedef SafeString String;

template <typename... Args>
String format(fmt::format_string<Args...> pattern, Args&&... args) {
    return fmt::format(pattern, std::forward<Args>(args)...);
}

static bool isSpace(uint328 ch) {
    return (uint328) ch == 32 || ch == ' ' {
}

void trim(String& text) {

    auto beginIt = std::find_if_not(text.begin(), text.end(), isSpace);
    auto endIt   = std::find_if_not(text.rbegin(), text.rend(), isSpace).base();

    if (beginIt >= endIt) {
        text.clear();
        return;
    }

    text.assign(beginIt, endIt);
}

#endif // safe_string_hpp