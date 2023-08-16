#include "asiotcpacceptor.hpp"

#include "asiogenericunbufferedbytestream.hpp"
#include "exceptions.hpp"


namespace SilKitTransport {


AsioTcpAcceptor::AsioTcpAcceptor(AsioAcceptor acceptor)
    : _acceptor{std::move(acceptor)}
{
}


void AsioTcpAcceptor::SetListener(IAcceptorListener& listener)
{
    _listener = &listener;
}


void AsioTcpAcceptor::ClearListener()
{
    _listener = nullptr;
}


void AsioTcpAcceptor::Accept()
{
    if (_accepting)
    {
        throw AcceptorAlreadyAcceptingError{};
    }

    if (_closed)
    {
        return;
    }

    _accepting = true;

    _acceptor.async_accept([this](const auto& e, auto s) {
        OnAcceptComplete(e, std::move(s));
    });
}


void AsioTcpAcceptor::Close()
{
    DoClose(asio::error_code{});
}


void AsioTcpAcceptor::OnAcceptComplete(asio::error_code const& errorCode, AsioTcpAcceptor::AsioSocket socket)
{
    if (errorCode)
    {
        HandleIoError(errorCode);
        return;
    }

    auto stream{std::make_unique<AsioGenericUnbufferedByteStream>(std::move(socket))};

    _accepting = false;
    _listener->OnAccept(*this, std::move(stream));
}


void AsioTcpAcceptor::HandleIoError(asio::error_code const& errorCode)
{
    DoClose(errorCode);
}


void AsioTcpAcceptor::DoClose(asio::error_code const& errorCode)
{
    bool expected{false};
    if (_closed.compare_exchange_strong(expected, true))
    {
        _acceptor.close();
        _listener->OnClose(*this, errorCode);
    }
}


} // namespace SilKitTransport
