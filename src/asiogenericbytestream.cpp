#include "asiogenericbytestream.hpp"

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


AsioGenericByteStream::AsioGenericByteStream(SocketType socket)
    : _socket{std::move(socket)}
    , _readBuffer{new ReadBuffer}
{
    Log(*this) << "handling " << FormatEndpoint(_socket.remote_endpoint()) << " connected on "
               << FormatEndpoint(_socket.local_endpoint()) << std::endl;
}


void AsioGenericByteStream::Start(IByteStreamListener &listener)
{
    if (_listener != nullptr)
    {
        throw ByteStreamAlreadyStartedError{};
    }

    Log(*this) << "started" << std::endl;

    _listener = &listener;
    StartReading();
}


void AsioGenericByteStream::SendSome(void const *data, size_t size)
{
    if (_listener == nullptr)
    {
        throw ByteStreamNotSetUpError{};
    }

    if (_closed)
    {
        throw ByteStreamAlreadyClosedError{};
    }

    if (_sending)
    {
        throw ByteStreamAlreadySendingError{};
    }

    Log(*this) << "Attempting to send " << size << " bytes" << std::endl;

    _socket.async_write_some(asio::buffer(data, size), [this](const auto &ec, size_t bt) {
        OnWriteSomeComplete(ec, bt);
    });
}


void AsioGenericByteStream::Close()
{
    if (_listener == nullptr)
    {
        throw ByteStreamNotSetUpError{};
    }

    DoClose(asio::error_code{});
}


void AsioGenericByteStream::StartReading()
{
    _socket.async_read_some(asio::buffer(*_readBuffer), [this](const auto &ec, size_t bt) {
        OnReadSomeComplete(ec, bt);
    });
}


void AsioGenericByteStream::OnReadSomeComplete(const asio::error_code &errorCode, size_t bytesTransferred)
{
    if (errorCode)
    {
        HandleIoError(errorCode);
        return;
    }

    Log(*this) << "read " << bytesTransferred << " bytes" << std::endl;

    _listener->OnDataReceived(*this, _readBuffer->data(), bytesTransferred);
    StartReading();
}


void AsioGenericByteStream::OnWriteSomeComplete(const asio::error_code &errorCode, size_t bytesTransferred)
{
    if (errorCode)
    {
        HandleIoError(errorCode);
        return;
    }

    Log(*this) << "written " << bytesTransferred << " bytes" << std::endl;

    // set the sending flag to false before informing the listener that sending has completed
    _sending = false;
    _listener->OnSomeDataSent(*this, bytesTransferred);
}


void AsioGenericByteStream::HandleIoError(const asio::error_code &errorCode)
{
    Err(*this) << "i/o error: " << errorCode.message() << std::endl;
    DoClose(errorCode);
}

void AsioGenericByteStream::DoClose(const asio::error_code &errorCode)
{
    bool expected{false};
    if (_closed.compare_exchange_strong(expected, true))
    {
        Log(*this) << "closing" << std::endl;
        _socket.close();
        _listener->OnByteStreamClosed(*this, errorCode);
    }
    else
    {
        Log(*this) << "already closed" << std::endl;
    }
}


} // namespace SilKitTransport
