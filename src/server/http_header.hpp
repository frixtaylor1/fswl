/**
 * Copyright (c) 2025 Kevin Daniel Taylor
 * Licensed under the MIT License (see the LICENSE file in the project root).
 */
#ifndef http_header_hpp
#define http_header_hpp

#ifndef associative_container_hpp
#    include "../stl/associative_container.hpp"
#endif // associative_container_hpp

template< uint MaxHeaders = 10 >
using HeaderContainer = AssociativeContainer< String, String, MaxHeaders>;

#endif // http_header_hpp