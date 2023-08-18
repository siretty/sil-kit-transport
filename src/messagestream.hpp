#ifndef SILKITTRANSPORT_MESSAGESTREAM_HPP
#define SILKITTRANSPORT_MESSAGESTREAM_HPP


#include "iunbufferedbytestream.hpp"

#include <array>
#include <deque>
#include <memory>
#include <vector>


namespace SilKitTransport {


struct IUnbufferedMessageStreamListener;


struct IUnbufferedMessageStream
{
    virtual ~IUnbufferedMessageStream() = default;

    virtual void SetUp(IUnbufferedMessageStreamListener& listener) = 0;

    virtual void ReadMessage() = 0;

    virtual void WriteMessage(std::vector<char> message) = 0;

    virtual void Close() = 0;
};


struct IUnbufferedMessageStreamListener
{
    virtual ~IUnbufferedMessageStreamListener() = default;

    virtual void OnReadDone(IUnbufferedMessageStream& stream, std::vector<char> message) = 0;

    virtual void OnWriteDone(IUnbufferedMessageStream& stream) = 0;

    virtual void OnClose(IUnbufferedMessageStream& stream, const std::error_code& errorCode) = 0;
};


class UnbufferedMessageStream
    : public IUnbufferedMessageStream
    , private IUnbufferedByteStreamListener
{
    template <typename Void>
    struct BufferSequence : IBufferSequence<Void>
    {
        struct Item
        {
            std::array<char, 4> length{};
            std::vector<char> message;
        };

        std::vector<Item> buffers;

        auto GetSequenceItem(size_t index) const -> Buffer<Void> override
        {
            const auto div = std::div(index, 2);
            const auto& item = buffers[div.quot];
            if (div.rem == 0)
            {
                return Buffer<Void>{item.length.data(), item.length.size()};
            }
            else
            {
                return Buffer<void>{item.message.data(), item.message.size()};
            }
        }

        auto GetSequenceSize() const -> size_t override { return buffers.size(); }
    };

    IUnbufferedMessageStreamListener* _listener{nullptr};

    std::unique_ptr<IUnbufferedByteStream> _stream{nullptr};

public:
    UnbufferedMessageStream() = default;

    explicit UnbufferedMessageStream(std::unique_ptr<IUnbufferedByteStream> stream);

public: // IUnbufferedMessageStream
    void SetUp(IUnbufferedMessageStreamListener& listener) override;
    void ReadMessage() override;
    void WriteMessage(std::vector<char> message) override;
    void Close() override;

private: // IUnbufferedByteStreamListener
    void OnReadDone(IUnbufferedByteStream& stream, size_t bytesTransferred) override;
    void OnWriteDone(IUnbufferedByteStream& stream, size_t bytesTransferred) override;
    void OnClose(IUnbufferedByteStream& stream, const std::error_code& errorCode) override;
};


} // namespace SilKitTransport


#endif //SILKITTRANSPORT_MESSAGESTREAM_HPP
