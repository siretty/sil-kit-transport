#ifndef SILKITTRANSPORT_SRC_IUNBUFFEREDBYTESTREAM_HPP_35ECAABEC6A8445AAC30201FE08AD10B
#define SILKITTRANSPORT_SRC_IUNBUFFEREDBYTESTREAM_HPP_35ECAABEC6A8445AAC30201FE08AD10B


#include "buffersequence.hpp"

#include <cstddef>

#include <system_error>


namespace SilKitTransport {


struct IUnbufferedByteStreamListener;


struct IUnbufferedByteStream
{
    virtual ~IUnbufferedByteStream() = default;

    virtual void SetUp(IUnbufferedByteStreamListener& listener) = 0;

    virtual void ReadSome(const IMutableBufferSequence& bufferSequence) = 0;

    virtual void WriteSome(const IConstBufferSequence& bufferSequence) = 0;

    virtual void Close() = 0;
};


struct IUnbufferedByteStreamListener
{
    virtual ~IUnbufferedByteStreamListener() = default;

    virtual void OnReadDone(IUnbufferedByteStream& stream, size_t bytesTransferred) = 0;

    virtual void OnWriteDone(IUnbufferedByteStream& stream, size_t bytesTransferred) = 0;

    virtual void OnClose(IUnbufferedByteStream& stream, const std::error_code& errorCode) = 0;
};


} // namespace SilKitTransport


#endif //SILKITTRANSPORT_SRC_IUNBUFFEREDBYTESTREAM_HPP_35ECAABEC6A8445AAC30201FE08AD10B
