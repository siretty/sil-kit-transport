#include "asioiocontext.hpp"

#include "asiotcpacceptor.hpp"
#include "asiotcpconnector.hpp"


namespace SilKitTransport {


void AsioIoContext::Run()
{
    _ioContext.run();
}


auto AsioIoContext::MakeTcpAcceptor(std::string const& address, uint16_t port) -> std::unique_ptr<IAcceptor>
{
    asio::ip::tcp::endpoint endpoint{asio::ip::address::from_string(address), port};
    asio::ip::tcp::acceptor acceptor{_ioContext.get_executor(), endpoint};
    return std::make_unique<AsioTcpAcceptor>(std::move(acceptor));
}


std::unique_ptr<IConnector> AsioIoContext::MakeTcpConnector()
{
    return std::make_unique<AsioTcpConnector>(_ioContext.get_executor());
}


} // namespace SilKitTransport
