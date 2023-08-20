#include "asiotcpunbufferedbytestreamacceptor.hpp"

#include "asiogenericunbufferedbytestream.hpp"
#include "exceptions.hpp"


namespace SilKitTransport {


AsioTcpUnbufferedByteStreamAcceptor::AsioTcpUnbufferedByteStreamAcceptor(AsioAcceptor acceptor)
    : _acceptor{std::move(acceptor)}
{
}


void AsioTcpUnbufferedByteStreamAcceptor::SetListener(IUnbufferedByteStreamAcceptorListener& listener)
{
    _listener = &listener;
}


void AsioTcpUnbufferedByteStreamAcceptor::ClearListener()
{
    _listener = nullptr;
}


void AsioTcpUnbufferedByteStreamAcceptor::Accept()
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


void AsioTcpUnbufferedByteStreamAcceptor::Close()
{
    _acceptor.close();
}


void AsioTcpUnbufferedByteStreamAcceptor::OnAcceptComplete(asio::error_code const& errorCode,
                                                           AsioTcpUnbufferedByteStreamAcceptor::AsioSocket socket)
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


void AsioTcpUnbufferedByteStreamAcceptor::HandleIoError(asio::error_code const& errorCode)
{
    DoClose(errorCode);
}


void AsioTcpUnbufferedByteStreamAcceptor::DoClose(asio::error_code const& errorCode)
{
    bool expected{false};
    if (_closed.compare_exchange_strong(expected, true))
    {
        _acceptor.close();
        _listener->OnClose(*this, errorCode);
    }
}


} // namespace SilKitTransport
