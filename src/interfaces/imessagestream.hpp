#ifndef SILKITTRANSPORT_SRC_IMESSAGESTREAM_HPP_A73B9A437F59460D84B5918560479A9A
#define SILKITTRANSPORT_SRC_IMESSAGESTREAM_HPP_A73B9A437F59460D84B5918560479A9A


#include <system_error>
#include <vector>


namespace SilKitTransport {


struct IMessageStreamListener;


struct IMessageStream
{
    virtual ~IMessageStream() = default;

    virtual void SetListener(IMessageStreamListener& listener) = 0;

    virtual void ClearListener() = 0;

    virtual void ReadMessage() = 0;

    virtual void WriteMessage(std::vector<char> message) = 0;

    virtual void Close() = 0;
};


struct IMessageStreamListener
{
    virtual ~IMessageStreamListener() = default;

    virtual void OnReadDone(IMessageStream& stream, std::vector<char> message) = 0;

    virtual void OnWriteDone(IMessageStream& stream) = 0;

    virtual void OnClose(IMessageStream& stream, const std::error_code& errorCode) = 0;
};


} // namespace SilKitTransport


#endif //SILKITTRANSPORT_SRC_IMESSAGESTREAM_HPP_A73B9A437F59460D84B5918560479A9A
