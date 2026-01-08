/**
 * Copyright (c) 2025 Kevin Daniel Taylor
 * Licensed under the MIT License (see the LICENSE file in the project root).
 */
#ifndef ansi_string_hpp
#define ansi_string_hpp

#include <string>
#include <fmt/core.h>

typedef std::string SafeString;
typedef SafeString String;

template <typename... Args>
String format(fmt::format_string<Args...> pattern, Args&&... args) {
    return fmt::format(pattern, std::forward<Args>(args)...);
}

#endif // ansi_string_hpp