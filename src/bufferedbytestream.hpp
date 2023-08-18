#ifndef SILKITTRANSPORT_BUFFEREDBYTESTREAM_HPP
#define SILKITTRANSPORT_BUFFEREDBYTESTREAM_HPP


#include "ibufferedbytestream.hpp"

#include "iunbufferedbytestream.hpp"

#include <atomic>
#include <memory>


namespace SilKitTransport {


class BufferedByteStream
    : public IBufferedByteStream
    , private IUnbufferedByteStreamListener
{
    IUnbufferedByteStream* _stream;
    IBufferedByteStreamListener* _listener{nullptr};

    bool _reading{false};
    bool _writing{false};

    std::vector<MutableBuffer> _readBufferSequence;
    std::vector<char> _readBufferData{};
    MutableBuffer _readBuffer{};

    std::vector<char> _writeBufferData{};
    ConstBuffer _writeBuffer{};

public:
    BufferedByteStream() = default;

    explicit BufferedByteStream(IUnbufferedByteStream& stream);

public: // IBufferedByteStream
    void SetUp(IBufferedByteStreamListener& listener) override;
    void Read(const IMutableBufferSequence& bufferSequence) override;
    void Write(const IConstBufferSequence& bufferSequence) override;
    void Close() override;

private: // IUnbufferedByteStreamListener
    void OnReadDone(IUnbufferedByteStream& stream, size_t bytesTransferred) override;
    void OnWriteDone(IUnbufferedByteStream& stream, size_t bytesTransferred) override;
    void OnClose(IUnbufferedByteStream& stream, const std::error_code& errorCode) override;
};


} // namespace SilKitTransport


#endif //SILKITTRANSPORT_BUFFEREDBYTESTREAM_HPP
