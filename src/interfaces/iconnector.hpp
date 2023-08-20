#ifndef SILKITTRANSPORT_SRC_INTERFACES_ICONNECTOR_HPP_19BEACE5CFD94726890C3D48977A5DB0
#define SILKITTRANSPORT_SRC_INTERFACES_ICONNECTOR_HPP_19BEACE5CFD94726890C3D48977A5DB0


#include <memory>
#include <string>
#include <system_error>


namespace SilKitTransport {


struct IConnectorListener;

struct IUnbufferedByteStream;


struct IConnector
{
    virtual ~IConnector() = default;

    virtual void SetListener(IConnectorListener& listener) = 0;

    virtual void ClearListener() = 0;

    virtual void Connect(const std::string& address, uint16_t port) = 0;

    virtual void Close() = 0;
};


struct IConnectorListener
{
    virtual ~IConnectorListener() = default;

    virtual void OnConnect(IConnector& connector, std::unique_ptr<IUnbufferedByteStream> stream) = 0;

    virtual void OnClose(IConnector& connector, const std::error_code& errorCode) = 0;
};


} // namespace SilKitTransport


#endif //SILKITTRANSPORT_SRC_INTERFACES_ICONNECTOR_HPP_19BEACE5CFD94726890C3D48977A5DB0
