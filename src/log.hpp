#ifndef SILKITTRANSPORT_SRC_LOG_HPP_7146FFC617934B55A9AECB448C5F117B
#define SILKITTRANSPORT_SRC_LOG_HPP_7146FFC617934B55A9AECB448C5F117B


#include <iosfwd>


namespace SilKitTransport {


// Forward Declarations

struct IByteStream;


// Declarations

auto Log(IByteStream&) -> std::ostream&;
auto Err(IByteStream&) -> std::ostream&;


} // namespace SilKitTransport


#endif //SILKITTRANSPORT_SRC_LOG_HPP_7146FFC617934B55A9AECB448C5F117B
