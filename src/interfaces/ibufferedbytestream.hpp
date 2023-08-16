#ifndef SILKITTRANSPORT_IBUFFEREDBYTESTREAM_HPP
#define SILKITTRANSPORT_IBUFFEREDBYTESTREAM_HPP


#include "buffersequence.hpp"

#include <cstddef>

#include <system_error>


namespace SilKitTransport {


struct IBufferedByteStreamListener;


struct IBufferedByteStream
{
    virtual ~IBufferedByteStream() = default;

    virtual void SetListener(IBufferedByteStreamListener& listener) = 0;

    virtual void ClearListener() = 0;

    virtual void Read(const IMutableBufferSequence& bufferSequence) = 0;

    virtual void Write(const IConstBufferSequence& bufferSequence) = 0;

    virtual void Close() = 0;
};


struct IBufferedByteStreamListener
{
    virtual ~IBufferedByteStreamListener() = default;

    virtual void OnReadDone(IBufferedByteStream& stream) = 0;

    virtual void OnWriteDone(IBufferedByteStream& stream) = 0;

    virtual void OnClose(IBufferedByteStream& stream, const std::error_code& errorCode) = 0;
};


} // namespace SilKitTransport


#endif //SILKITTRANSPORT_IBUFFEREDBYTESTREAM_HPP
