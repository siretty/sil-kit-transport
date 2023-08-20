#include "asiotcpconnector.hpp"

#include "asiogenericunbufferedbytestream.hpp"
#include "exceptions.hpp"


namespace SilKitTransport {


AsioTcpConnector::AsioTcpConnector(asio::any_io_executor const& ioExecutor)
    : _socket{ioExecutor}
{
}


void AsioTcpConnector::SetListener(IConnectorListener& listener)
{
    _listener = &listener;
}


void AsioTcpConnector::ClearListener()
{
    _listener = nullptr;
}


void AsioTcpConnector::Connect(std::string const& address, uint16_t port)
{
    if (_connecting)
    {
        throw ConnectorAlreadyConnectingError{};
    }

    _connecting = true;

    asio::ip::tcp::endpoint endpoint{asio::ip::address::from_string(address), port};

    _socket.async_connect(endpoint, [this](const asio::error_code& e) {
        OnConnectComplete(e);
    });
}


void AsioTcpConnector::Close()
{
    DoClose(asio::error_code{});
}


void AsioTcpConnector::OnConnectComplete(asio::error_code const& errorCode)
{
    if (errorCode)
    {
        HandleIoError(errorCode);
        return;
    }

    using std::swap;

    asio::ip::tcp::socket socket{_socket.get_executor()};
    swap(_socket, socket);

    auto stream{std::make_unique<AsioGenericUnbufferedByteStream>(std::move(socket))};

    _listener->OnConnect(*this, std::move(stream));
}


void AsioTcpConnector::HandleIoError(asio::error_code const& errorCode)
{
    DoClose(errorCode);
}


void AsioTcpConnector::DoClose(asio::error_code const& errorCode)
{
    bool expected{false};
    if (_closed.compare_exchange_strong(expected, true))
    {
        _socket.close();
        _listener->OnClose(*this, errorCode);
    }
}


} // namespace SilKitTransport
