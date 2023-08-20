#ifndef SILKITTRANSPORT_SRC_BYTEORDER_HPP_B830FA2B356049D68F6E038850928468
#define SILKITTRANSPORT_SRC_BYTEORDER_HPP_B830FA2B356049D68F6E038850928468


#include <cstdint>


namespace SilKitTransport {
namespace LittleEndian {


auto GetU32(const void *data) -> uint32_t;

void PutU32(void *data, uint32_t value);


} // namespace LittleEndian
} // namespace SilKitTransport


#endif //SILKITTRANSPORT_SRC_BYTEORDER_HPP_B830FA2B356049D68F6E038850928468
