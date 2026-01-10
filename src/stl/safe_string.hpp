/**
 * Copyright (c) 2025 Kevin Daniel Taylor
 * Licensed under the MIT License (see the LICENSE file in the project root).
 */
#ifndef safe_string_hpp
#define safe_string_hpp

#include <string>
#include <fmt/core.h>

typedef std::string SafeString;
typedef SafeString String;

template <typename... Args>
String format(fmt::format_string<Args...> pattern, Args&&... args) {
    return fmt::format(pattern, std::forward<Args>(args)...);
}

#endif // safe_string_hpp