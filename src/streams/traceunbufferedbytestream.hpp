#ifndef SILKITTRANSPORT_SRC_STREAMS_TRACEUNBUFFEREDBYTESTREAM_HPP_2E75E6480D9448348045C63BB057F50D
#define SILKITTRANSPORT_SRC_STREAMS_TRACEUNBUFFEREDBYTESTREAM_HPP_2E75E6480D9448348045C63BB057F50D


#include "iunbufferedbytestream.hpp"


namespace SilKitTransport {


class TraceUnbufferedByteStream final
    : public IUnbufferedByteStream
    , private IUnbufferedByteStreamListener
{
    IUnbufferedByteStream *_stream{nullptr};
    IUnbufferedByteStreamListener *_listener{nullptr};

public:
    TraceUnbufferedByteStream() = default;
    explicit TraceUnbufferedByteStream(IUnbufferedByteStream &unbufferedByteStream);

public: // IByteStream
    void SetListener(IUnbufferedByteStreamListener &listener) override;
    void ClearListener() override;
    void ReadSome(const IMutableBufferSequence &bufferSequence) override;
    void WriteSome(const IConstBufferSequence &bufferSequence) override;
    void Close() override;

private: // IUnbufferedByteStreamListener
    void OnReadDone(IUnbufferedByteStream &stream, size_t bytesTransferred) override;
    void OnWriteDone(IUnbufferedByteStream &stream, size_t bytesTransferred) override;
    void OnClose(IUnbufferedByteStream &stream, const std::error_code &errorCode) override;
};


} // namespace SilKitTransport


#endif //SILKITTRANSPORT_SRC_STREAMS_TRACEUNBUFFEREDBYTESTREAM_HPP_2E75E6480D9448348045C63BB057F50D
