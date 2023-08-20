#include "exceptions.hpp"


namespace SilKitTransport {


auto ByteStreamAlreadyStartedError::what() const noexcept -> const char*
{
    return "byte stream has already been started";
}

auto ByteStreamNotSetUpError::what() const noexcept -> const char*
{
    return "byte stream has not been started yet";
}

auto ByteStreamAlreadyClosedError::what() const noexcept -> const char*
{
    return "byte stream has already been closed";
}

auto ByteStreamAlreadySendingError::what() const noexcept -> const char*
{
    return "byte stream is already sending data";
}

auto AcceptorAlreadyAcceptingError::what() const noexcept -> const char*
{
    return "acceptor is already accepting";
}

auto StreamAlreadyReadingError::what() const noexcept -> const char*
{
    return "byte stream is already reading";
}

auto StreamAlreadyWritingError::what() const noexcept -> const char*
{
    return "byte stream is already writing";
}


} // namespace SilKitTransport
