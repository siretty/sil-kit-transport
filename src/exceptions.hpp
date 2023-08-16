#ifndef SILKITTRANSPORT_SRC_EXCEPTIONS_HPP_7DD8148AC28A4ED2ACAC70C4104A26F9
#define SILKITTRANSPORT_SRC_EXCEPTIONS_HPP_7DD8148AC28A4ED2ACAC70C4104A26F9


#include <exception>
#include <system_error>


namespace SilKitTransport {


struct ByteStreamNotSetUpError : std::exception
{
    auto what() const noexcept -> const char* override;
};

struct ByteStreamAlreadyReadingError : std::exception
{
    auto what() const noexcept -> const char* override;
};

struct ByteStreamAlreadyWritingError : std::exception
{
    auto what() const noexcept -> const char* override;
};


struct ByteStreamAlreadyStartedError : std::exception
{
    auto what() const noexcept -> const char* override;
};

struct ByteStreamAlreadyClosedError : std::exception
{
    auto what() const noexcept -> const char* override;
};

struct ByteStreamAlreadySendingError : std::exception
{
    auto what() const noexcept -> const char* override;
};


} // namespace SilKitTransport


#endif //SILKITTRANSPORT_SRC_EXCEPTIONS_HPP_7DD8148AC28A4ED2ACAC70C4104A26F9
