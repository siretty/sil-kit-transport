#ifndef SILKITTRANSPORT_SRC_INTERFACES_IACCEPTOR_HPP_AF8F4D168B3A4ACB800E27E6F2CE52B9
#define SILKITTRANSPORT_SRC_INTERFACES_IACCEPTOR_HPP_AF8F4D168B3A4ACB800E27E6F2CE52B9


#include <memory>
#include <system_error>


namespace SilKitTransport {


struct IAcceptorListener;

struct IUnbufferedByteStream;


struct IAcceptor
{
    virtual ~IAcceptor() = default;

    virtual void SetListener(IAcceptorListener& listener) = 0;

    virtual void ClearListener() = 0;

    virtual void Accept() = 0;

    virtual void Close() = 0;
};


struct IAcceptorListener
{
    virtual ~IAcceptorListener() = default;

    virtual void OnAccept(IAcceptor& acceptor, std::unique_ptr<IUnbufferedByteStream> stream) = 0;

    virtual void OnClose(IAcceptor& acceptor, std::error_code errorCode) = 0;
};


} // namespace SilKitTransport


#endif //SILKITTRANSPORT_SRC_INTERFACES_IACCEPTOR_HPP_AF8F4D168B3A4ACB800E27E6F2CE52B9
