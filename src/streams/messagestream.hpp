#ifndef SILKITTRANSPORT_MESSAGESTREAM_HPP
#define SILKITTRANSPORT_MESSAGESTREAM_HPP


#include "imessagestream.hpp"

#include "ibufferedbytestream.hpp"

#include <array>
#include <deque>
#include <memory>
#include <vector>

#include <cstdint>


namespace SilKitTransport {


class MessageStream
    : public IMessageStream
    , private IBufferedByteStreamListener
{
    enum ReadState
    {
        READ_IDLE,
        READ_SIZE,
        READ_DATA,
    };

    enum WriteState
    {
        WRITE_IDLE,
        WRITE_PENDING,
    };

    IBufferedByteStream* _stream{nullptr};
    IMessageStreamListener* _listener{nullptr};

    ReadState _readState{READ_IDLE};
    std::array<uint8_t, 4> _readSize{};
    std::vector<char> _readData;

    WriteState _writeState{WRITE_IDLE};
    std::array<uint8_t, 4> _writeSize{};
    std::vector<char> _writeData;

public:
    MessageStream() = default;

    explicit MessageStream(IBufferedByteStream& stream);

public: // IUnbufferedMessageStream
    void SetListener(IMessageStreamListener& listener) override;
    void ClearListener() override;
    void ReadMessage() override;
    void WriteMessage(std::vector<char> message) override;
    void Close() override;

private: // IUnbufferedByteStreamListener
    void OnReadDone(IBufferedByteStream& stream) override;
    void OnWriteDone(IBufferedByteStream& stream) override;
    void OnClose(IBufferedByteStream& stream, const std::error_code& errorCode) override;
};


} // namespace SilKitTransport


#endif //SILKITTRANSPORT_MESSAGESTREAM_HPP
