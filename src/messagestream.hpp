#ifndef SILKITTRANSPORT_SRC_MESSAGESTREAM_HPP_722E90D82A704A4E8C3006884378E074
#define SILKITTRANSPORT_SRC_MESSAGESTREAM_HPP_722E90D82A704A4E8C3006884378E074


#include "ibytestream.hpp"
#include "imessagestream.hpp"
#include "message.hpp"

#include <array>
#include <deque>
#include <memory>
#include <vector>


namespace SilKitTransport {


class MessageStream
    : public IMessageStream
    , private IByteStream::IByteStreamListener
{
    IMessageStream::IMessageStreamListener *_listener{nullptr};

    std::unique_ptr<IByteStream> _byteStream;

    std::vector<unsigned char> _rcvBuf;

public:
    explicit MessageStream(std::unique_ptr<IByteStream> byteStream);

public: // IMessageStream
    void Start(IMessageStream::IMessageStreamListener &listener) override;

    void Send(std::vector<unsigned char> message) override;

    void Close() override;

private: // IByteStream::IByteStreamListener
    void OnByteStreamSomeDataReceived(IByteStream &byteStream, void const *data, size_t size) override;

    void OnByteStreamSomeDataSent(IByteStream &byteStream, size_t bytesTransferred) override;

    void OnByteStreamClosed(IByteStream &byteStream, std::error_code const &errorCode) override;

private:
    void DoSend();
};


} // namespace SilKitTransport


#endif //SILKITTRANSPORT_SRC_MESSAGESTREAM_HPP_722E90D82A704A4E8C3006884378E074
