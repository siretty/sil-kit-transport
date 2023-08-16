#ifndef SILKITTRANSPORT_SRC_ASIOGENERICBYTESTREAM_HPP_BA433613803E42F9A67DBB46C66B3DB5
#define SILKITTRANSPORT_SRC_ASIOGENERICBYTESTREAM_HPP_BA433613803E42F9A67DBB46C66B3DB5


#include "ibytestream.hpp"

#include <array>
#include <atomic>
#include <memory>

#include "asio.hpp"


namespace SilKitTransport {


class AsioGenericByteStream final : public IByteStream
{
    using SocketType = asio::generic::stream_protocol::socket;
    using ReadBuffer = std::array<char, 4096>;

    IByteStreamListener *_listener{nullptr};
    std::atomic<bool> _closed{false};
    std::atomic<bool> _sending{false};

    SocketType _socket;
    std::unique_ptr<ReadBuffer> _readBuffer;

public:
    explicit AsioGenericByteStream(SocketType socket);

public: // IByteStream
    void Start(IByteStreamListener &listener) override;

    void SendSome(void const *data, size_t size) override;

    void Close() override;

private:
    void StartReading();

    void OnReadSomeComplete(const asio::error_code &errorCode, size_t bytesTransferred);

    void OnWriteSomeComplete(const asio::error_code &errorCode, size_t bytesTransferred);

    void HandleIoError(const asio::error_code &errorCode);

    void DoClose(const asio::error_code &errorCode);
};


} // namespace SilKitTransport


#endif //SILKITTRANSPORT_SRC_ASIOGENERICBYTESTREAM_HPP_BA433613803E42F9A67DBB46C66B3DB5
