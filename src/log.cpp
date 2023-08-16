#include "log.hpp"

#include <iostream>


namespace SilKitTransport {


auto Log(IByteStream& byteStream) -> std::ostream&
{
    return std::cout << "ByteStream (" << static_cast<const void*>(&byteStream) << "): ";
}

auto Err(IByteStream& byteStream) -> std::ostream&
{
    return std::cerr << "ByteStream (" << static_cast<const void*>(&byteStream) << "): ";
}


} // namespace SilKitTransport
