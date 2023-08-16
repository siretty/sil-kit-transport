#include "asiogenericunbufferedbytestream.hpp"

#include "exceptions.hpp"
#include "log.hpp"

#include <iostream>
#include <string>


namespace SilKitTransport {


namespace {

auto FormatEndpoint(const asio::generic::stream_protocol::endpoint &ep) -> std::string
{
    const auto protocol = ep.protocol().protocol();

    std::ostringstream ss;
    if (protocol == asio::ip::tcp::v4().protocol())
    {
        ss << reinterpret_cast<const asio::ip::tcp::endpoint &>(ep);
    }
    else if (protocol == asio::local::stream_protocol().protocol())
    {
        ss << reinterpret_cast<const asio::ip::tcp::endpoint &>(ep);
    }
    else
    {
        ss << "unknown protocol endpoint";
    }
    return ss.str();
}

} // namespace


AsioGenericUnbufferedByteStream::AsioGenericUnbufferedByteStream(AsioSocket socket)
    : _socket{std::move(socket)}
{
}


void AsioGenericUnbufferedByteStream::SetUp(IUnbufferedByteStreamListener &listener)
{
    if (_listener != nullptr)
    {
        throw ByteStreamNotSetUpError{};
    }

    _listener = &listener;
}


void AsioGenericUnbufferedByteStream::ReadSome(void *data, size_t size)
{
    if (_reading)
    {
        throw ByteStreamAlreadyReadingError{};
    }

    if (_closed)
    {
        return;
    }

    _reading = true;
    _socket.async_read_some(asio::buffer(data, size), [this](const auto &e, auto s) {
        OnReadSomeComplete(e, s);
    });
}


void AsioGenericUnbufferedByteStream::WriteSome(const void *data, size_t size)
{
    if (_writing)
    {
        throw ByteStreamAlreadyWritingError{};
    }

    if (_closed)
    {
        return;
    }

    _writing = true;
    _socket.async_write_some(asio::buffer(data, size), [this](const auto &e, auto s) {
        OnWriteSomeComplete(e, s);
    });
}


void AsioGenericUnbufferedByteStream::Close()
{
    if (_closed)
    {
        return;
    }

    DoClose(std::error_code{});
}


void AsioGenericUnbufferedByteStream::OnReadSomeComplete(asio::error_code const &errorCode, size_t bytesTransferred)
{
    if (errorCode)
    {
        HandleIoError(errorCode);
        return;
    }

    _reading = false;
    _listener->OnReadDone(*this, bytesTransferred);
}


void AsioGenericUnbufferedByteStream::OnWriteSomeComplete(asio::error_code const &errorCode, size_t bytesTransferred)
{
    if (errorCode)
    {
        HandleIoError(errorCode);
        return;
    }

    _reading = false;
    _listener->OnWriteDone(*this, bytesTransferred);
}


void AsioGenericUnbufferedByteStream::HandleIoError(asio::error_code const &errorCode)
{
    DoClose(errorCode);
}


void AsioGenericUnbufferedByteStream::DoClose(asio::error_code const &errorCode)
{
    bool expected{false};
    if (_closed.compare_exchange_strong(expected, true))
    {
        _socket.close();
        _listener->OnClose(*this, errorCode);
    }
}


} // namespace SilKitTransport
