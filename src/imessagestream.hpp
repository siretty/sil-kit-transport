#ifndef SILKITTRANSPORT_SRC_IMESSAGESTREAM_HPP_7FA58CC5CA7540CDBD3160065CAF87DB
#define SILKITTRANSPORT_SRC_IMESSAGESTREAM_HPP_7FA58CC5CA7540CDBD3160065CAF87DB


#include <system_error>
#include <vector>


namespace SilKitTransport {


struct IMessageStreamListener;


struct IMessageStream
{
    virtual ~IMessageStream() = default;

    virtual void Start(IMessageStreamListener &listener) = 0;

    virtual void Send(std::vector<unsigned char> message) = 0;

    virtual void Close() = 0;
};


struct IMessageStreamListener
{
    virtual ~IMessageStreamListener() = default;

    virtual void OnMessageReceived(IMessageStream &messageStream, std::vector<unsigned char> message) = 0;

    virtual void OnMessageSent(IMessageStream &messageStream) = 0;

    virtual void OnMessageStreamClosed(IMessageStream &messageStream, std::error_code errorCode) = 0;
};


} // namespace SilKitTransport


#endif //SILKITTRANSPORT_SRC_IMESSAGESTREAM_HPP_7FA58CC5CA7540CDBD3160065CAF87DB
