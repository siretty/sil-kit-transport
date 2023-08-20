#ifndef SILKITTRANSPORT_SRC_ASIOGENERICUNBUFFEREDBYTESTREAM_HPP_9588C06564D648A1832B9D020FCE52CB
#define SILKITTRANSPORT_SRC_ASIOGENERICUNBUFFEREDBYTESTREAM_HPP_9588C06564D648A1832B9D020FCE52CB


#include "iunbufferedbytestream.hpp"

#include <array>
#include <atomic>
#include <memory>

#include "asio.hpp"


namespace SilKitTransport {


class AsioGenericUnbufferedByteStream final : public IUnbufferedByteStream
{
    using AsioSocket = asio::generic::stream_protocol::socket;

    IUnbufferedByteStreamListener *_listener{nullptr};

    std::atomic<bool> _closed{false};
    bool _reading{false};
    bool _writing{false};

    std::vector<asio::mutable_buffer> _readBufferSequence;
    std::vector<asio::const_buffer> _writeBufferSequence;

    AsioSocket _socket;

public:
    explicit AsioGenericUnbufferedByteStream(AsioSocket socket);

public: // IByteStream
    void SetListener(IUnbufferedByteStreamListener &listener) override;
    void ClearListener() override;
    void ReadSome(const IMutableBufferSequence &bufferSequence) override;
    void WriteSome(const IConstBufferSequence &bufferSequence) override;
    void Close() override;

private:
    void OnReadSomeComplete(const asio::error_code &errorCode, size_t bytesTransferred);
    void OnWriteSomeComplete(const asio::error_code &errorCode, size_t bytesTransferred);
    void HandleIoError(const asio::error_code &errorCode);
    void DoClose(const asio::error_code &errorCode);
};


} // namespace SilKitTransport


#endif //SILKITTRANSPORT_SRC_ASIOGENERICUNBUFFEREDBYTESTREAM_HPP_9588C06564D648A1832B9D020FCE52CB
