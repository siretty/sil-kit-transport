#ifndef SILKITTRANSPORT_SRC_INTERFACES_IIOCONTEXT_HPP_86EF60F54F8841B39BBD5DF363D623D2
#define SILKITTRANSPORT_SRC_INTERFACES_IIOCONTEXT_HPP_86EF60F54F8841B39BBD5DF363D623D2


#include <memory>
#include <string>
#include <system_error>


namespace SilKitTransport {


struct IAcceptor;
struct IConnector;


struct IIoContext
{
    virtual ~IIoContext() = default;

    virtual void Run() = 0;

    virtual auto MakeTcpAcceptor(const std::string& address, uint16_t port) -> std::unique_ptr<IAcceptor> = 0;

    virtual auto MakeTcpConnector() -> std::unique_ptr<IConnector> = 0;
};


} // namespace SilKitTransport


#endif //SILKITTRANSPORT_SRC_INTERFACES_IIOCONTEXT_HPP_86EF60F54F8841B39BBD5DF363D623D2
