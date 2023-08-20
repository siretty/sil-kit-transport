#ifndef SILKITTRANSPORT_SRC_CONTEXTS_ASIOTCPUNBUFFEREDBYTESTREAMCONNECTOR_HPP_06BD96630BEE46D29F2BADD3D262A0FB
#define SILKITTRANSPORT_SRC_CONTEXTS_ASIOTCPUNBUFFEREDBYTESTREAMCONNECTOR_HPP_06BD96630BEE46D29F2BADD3D262A0FB


#include "iconnector.hpp"

#include "asio.hpp"


namespace SilKitTransport {


class AsioTcpConnector : public IConnector
{
    IConnectorListener* _listener{nullptr};

    std::atomic<bool> _closed{false};
    bool _connecting{false};

    asio::ip::tcp::socket _socket;

public:
    explicit AsioTcpConnector(const asio::any_io_executor& ioExecutor);

public: // IConnector
    void SetListener(IConnectorListener& listener) override;
    void ClearListener() override;
    void Connect(std::string const& address, uint16_t port) override;
    void Close() override;

private:
    void OnConnectComplete(const asio::error_code& errorCode);
    void HandleIoError(const asio::error_code& errorCode);
    void DoClose(const asio::error_code& errorCode);
};


} // namespace SilKitTransport


#endif //SILKITTRANSPORT_SRC_CONTEXTS_ASIOTCPUNBUFFEREDBYTESTREAMCONNECTOR_HPP_06BD96630BEE46D29F2BADD3D262A0FB
