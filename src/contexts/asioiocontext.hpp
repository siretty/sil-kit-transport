#ifndef SILKITTRANSPORT_SRC_CONTEXTS_ASIOIOCONTEXT_HPP_3134B80505FF4ACA82AF6A86D43AB5ED
#define SILKITTRANSPORT_SRC_CONTEXTS_ASIOIOCONTEXT_HPP_3134B80505FF4ACA82AF6A86D43AB5ED


#include "iiocontext.hpp"

#include "asio.hpp"


namespace SilKitTransport {


class AsioIoContext : public IIoContext
{
    asio::io_context _ioContext;

public:
    void Run() override;

    auto MakeTcpAcceptor(std::string const& address, uint16_t port) -> std::unique_ptr<IAcceptor> override;

    auto MakeTcpConnector() -> std::unique_ptr<IConnector> override;
};


} // namespace SilKitTransport


#endif //SILKITTRANSPORT_SRC_CONTEXTS_ASIOIOCONTEXT_HPP_3134B80505FF4ACA82AF6A86D43AB5ED
