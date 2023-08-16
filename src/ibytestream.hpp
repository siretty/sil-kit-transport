#ifndef SIL_KIT_TRANSPORT_SRC_IBYTESTREAM_HPP_A062C8D1B6C24F79AF80645939CC2361
#define SIL_KIT_TRANSPORT_SRC_IBYTESTREAM_HPP_A062C8D1B6C24F79AF80645939CC2361


#include <cstdint>

#include <system_error>


namespace SilKitTransport {
struct IUnbufferedByteStreamListener;


struct IUnbufferedByteStream
{
    virtual ~IUnbufferedByteStream() = default;

    virtual void SetUp(IUnbufferedByteStreamListener& listener) = 0;

    virtual void ReadSome(void* data, size_t size) = 0;

    virtual void WriteSome(const void* data, size_t size) = 0;

    virtual void Close() = 0;
};

struct IUnbufferedByteStreamListener
{
    virtual ~IUnbufferedByteStreamListener() = default;

    virtual void OnReadDone(IUnbufferedByteStream& stream, size_t bytesTransferred) = 0;

    virtual void OnWriteDone(IUnbufferedByteStream& stream, size_t bytesTransferred) = 0;

    virtual void OnClose(IUnbufferedByteStream& stream, const std::error_code& errorCode) = 0;
};




struct IByteStreamListener;


struct IByteStream
{
    virtual ~IByteStream() = default;

    virtual void Start(IByteStreamListener& listener) = 0;

    virtual void SendSome(const void* data, size_t size) = 0;

    virtual void Close() = 0;
};


struct IByteStreamListener
{
    virtual ~IByteStreamListener() = default;

    virtual void OnDataReceived(IByteStream& byteStream, const void* data, size_t size) = 0;

    virtual void OnSomeDataSent(IByteStream& byteStream, size_t bytesTransferred) = 0;

    virtual void OnByteStreamClosed(IByteStream& byteStream, const std::error_code& errorCode) = 0;
};


} // namespace SilKitTransport


#endif //SIL_KIT_TRANSPORT_SRC_IBYTESTREAM_HPP_A062C8D1B6C24F79AF80645939CC2361
