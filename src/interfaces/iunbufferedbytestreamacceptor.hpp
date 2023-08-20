#ifndef SILKITTRANSPORT_SRC_INTERFACES_IUNBUFFEREDBYTESTREAMACCEPTOR_HPP_92F1C9DC0CB246169C8FC14E0872AD83
#define SILKITTRANSPORT_SRC_INTERFACES_IUNBUFFEREDBYTESTREAMACCEPTOR_HPP_92F1C9DC0CB246169C8FC14E0872AD83


#include <memory>
#include <system_error>


namespace SilKitTransport {


struct IUnbufferedByteStream;
struct IUnbufferedByteStreamAcceptorListener;


struct IUnbufferedByteStreamAcceptor
{
    virtual ~IUnbufferedByteStreamAcceptor() = default;

    virtual void SetListener(IUnbufferedByteStreamAcceptorListener& listener) = 0;

    virtual void ClearListener() = 0;

    virtual void Accept() = 0;

    virtual void Close() = 0;
};


struct IUnbufferedByteStreamAcceptorListener
{
    virtual ~IUnbufferedByteStreamAcceptorListener() = default;

    virtual void OnAccept(IUnbufferedByteStreamAcceptor& acceptor, std::unique_ptr<IUnbufferedByteStream> stream) = 0;

    virtual void OnClose(IUnbufferedByteStreamAcceptor& acceptor, std::error_code errorCode) = 0;
};


} // namespace SilKitTransport


#endif //SILKITTRANSPORT_SRC_INTERFACES_IUNBUFFEREDBYTESTREAMACCEPTOR_HPP_92F1C9DC0CB246169C8FC14E0872AD83
