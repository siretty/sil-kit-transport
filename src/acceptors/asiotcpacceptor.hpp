#ifndef SILKITTRANSPORT_SRC_ACCEPTORS_ASIOTCPACCEPTOR_HPP_F5CA458A32624F569C5887E1AFA86A3A
#define SILKITTRANSPORT_SRC_ACCEPTORS_ASIOTCPACCEPTOR_HPP_F5CA458A32624F569C5887E1AFA86A3A


#include "iacceptor.hpp"

#include "asio.hpp"


namespace SilKitTransport {


class AsioTcpAcceptor : public IAcceptor
{
    using AsioAcceptor = asio::ip::tcp::acceptor;
    using AsioSocket = asio::ip::tcp::socket;

    IAcceptorListener* _listener{nullptr};

    std::atomic<bool> _closed{false};
    bool _accepting{false};

    AsioAcceptor _acceptor;

public:
    AsioTcpAcceptor(AsioAcceptor acceptor);

public: // IAcceptor
    void SetListener(IAcceptorListener& listener) override;
    void ClearListener() override;
    void Accept() override;
    void Close() override;

private:
    void OnAcceptComplete(const asio::error_code& errorCode, AsioSocket socket);
    void HandleIoError(const asio::error_code& errorCode);
    void DoClose(const asio::error_code& errorCode);
};


} // namespace SilKitTransport


#endif //SILKITTRANSPORT_SRC_ACCEPTORS_ASIOTCPACCEPTOR_HPP_F5CA458A32624F569C5887E1AFA86A3A
