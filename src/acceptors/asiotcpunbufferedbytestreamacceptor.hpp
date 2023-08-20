#ifndef SILKITTRANSPORT_SRC_ACCEPTORS_ASIOTCPUNBUFFEREDBYTESTREAMACCEPTOR_HPP_684314C39C4B44339BD2A4F2AD615763
#define SILKITTRANSPORT_SRC_ACCEPTORS_ASIOTCPUNBUFFEREDBYTESTREAMACCEPTOR_HPP_684314C39C4B44339BD2A4F2AD615763


#include "iunbufferedbytestreamacceptor.hpp"

#include "asio.hpp"


namespace SilKitTransport {


class AsioTcpUnbufferedByteStreamAcceptor : public IUnbufferedByteStreamAcceptor
{
    using AsioAcceptor = asio::ip::tcp::acceptor;
    using AsioSocket = asio::ip::tcp::socket;

    IUnbufferedByteStreamAcceptorListener* _listener{nullptr};

    std::atomic<bool> _closed{false};
    bool _accepting{false};

    AsioAcceptor _acceptor;

public:
    AsioTcpUnbufferedByteStreamAcceptor(AsioAcceptor acceptor);

public: // IUnbufferedByteStreamAcceptor
    void SetListener(IUnbufferedByteStreamAcceptorListener& listener) override;
    void ClearListener() override;
    void Accept() override;
    void Close() override;

private:
    void OnAcceptComplete(const asio::error_code& errorCode, AsioSocket socket);
    void HandleIoError(const asio::error_code& errorCode);
    void DoClose(const asio::error_code& errorCode);
};


} // namespace SilKitTransport


#endif //SILKITTRANSPORT_SRC_ACCEPTORS_ASIOTCPUNBUFFEREDBYTESTREAMACCEPTOR_HPP_684314C39C4B44339BD2A4F2AD615763
