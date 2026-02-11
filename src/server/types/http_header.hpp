/**
 * Copyright (c) 2025 Kevin Daniel Taylor
 * Licensed under the MIT License (see the LICENSE file in the project root).
 */

#ifndef http_header_hpp
#define http_header_hpp

#include "../../stl/associative_container.hpp"

template< uint32 MaxHeaders = 10 >
using HeaderContainer = AssociativeContainer< String, String, MaxHeaders>;

#endif // http_header_hpp