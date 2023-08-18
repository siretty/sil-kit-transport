#include "byteorder.hpp"


namespace SilKitTransport {
namespace LittleEndian {


auto GetU32(const void *data) -> uint32_t
{
    const auto bytes = static_cast<const unsigned char *>(data);

    uint32_t value{0};
    for (size_t index = 0; index != 4; ++index)
    {
        value |= (static_cast<uint32_t>(bytes[index]) << (8u * index));
    }
    return value;
}


void PutU32(void *data, uint32_t value)
{
    const auto bytes = static_cast<unsigned char *>(data);

    for (size_t index = 0; index != 4; ++index)
    {
        bytes[index] = static_cast<unsigned char>((value >> (8u * index)) & static_cast<uint32_t>(0xFF));
    }
}


} // namespace LittleEndian
} // namespace SilKitTransport
