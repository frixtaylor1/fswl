#ifndef http_header_hpp
#define http_header_hpp

#ifndef associative_container_hpp
#    include "../stl/associative_container.hpp"
#endif // associative_container_hpp

template< uint MaxHeaders = 10, uint KeyStringLength = 20, uint ValueStringLength = 64 >
using HeaderContainer = AssociativeContainer< AnsiString< KeyStringLength >, AnsiString< ValueStringLength >, MaxHeaders>;

#endif // http_header_hpp