#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "bufferedbytestream.hpp"

#include <functional>

#include <cstring>

#include "asio.hpp"


namespace {


using namespace SilKitTransport;


class DummyUnbufferedByteStream : public IUnbufferedByteStream
{
    IUnbufferedByteStreamListener* _listener{nullptr};

    std::vector<char> _pendingReadData;
    ConstBuffer _pendingReadBuffer;

    std::vector<char> _writtenData;

public:
    DummyUnbufferedByteStream() = default;

    DummyUnbufferedByteStream(const char* producedReadData)
        : _pendingReadData{producedReadData, producedReadData + std::strlen(producedReadData)}
        , _pendingReadBuffer{_pendingReadData.data(), _pendingReadData.size()}
    {
    }

    auto GetWritten() -> ConstBuffer { return ConstBuffer{_writtenData.data(), _writtenData.size()}; }

public: // IUnbufferedByteStream
    void SetUp(IUnbufferedByteStreamListener& listener) override { _listener = &listener; }

    void ReadSome(const IMutableBufferSequence& bufferSequence) override
    {
        size_t bytesTransferred{0};

        for (size_t index = 0; index != bufferSequence.GetSequenceSize(); ++index)
        {
            auto dstBuffer = bufferSequence.GetSequenceItem(index);

            // invariant: (pending read buffer is empty) => (destination buffer is empty)
            ASSERT_FALSE(_pendingReadBuffer.GetSize() == 0 && dstBuffer.GetSize() != 0);

            auto srcBuffer = _pendingReadBuffer.SliceOff(dstBuffer.GetSize());
            std::memcpy(dstBuffer.GetData(), srcBuffer.GetData(), srcBuffer.GetSize());

            bytesTransferred += srcBuffer.GetSize();
        }

        _listener->OnReadDone(*this, bytesTransferred);
    }

    void WriteSome(const IConstBufferSequence& bufferSequence) override
    {
        _writtenData.clear();

        for (size_t index = 0; index != bufferSequence.GetSequenceSize(); ++index)
        {
            const auto buffer = bufferSequence.GetSequenceItem(index);
            const auto first{static_cast<const char*>(buffer.GetData())};
            const auto last{first + buffer.GetSize()};
            _writtenData.insert(_writtenData.end(), first, last);
        }

        _listener->OnWriteDone(*this, _writtenData.size());
    }

    void Close() override {}
};

struct BufferedByteStreamCallbacks : IBufferedByteStreamListener
{
    std::function<void(IBufferedByteStream&)> onReadDone;
    std::function<void(IBufferedByteStream&)> onWriteDone;
    std::function<void(IBufferedByteStream&, const std::error_code&)> onClose;

    void OnReadDone(IBufferedByteStream& stream) override
    {
        if (onReadDone)
        {
            onReadDone(stream);
        }
    }
    void OnWriteDone(IBufferedByteStream& stream) override
    {
        if (onWriteDone)
        {
            onWriteDone(stream);
        }
    }
    void OnClose(IBufferedByteStream& stream, const std::error_code& errorCode) override
    {
        if (onClose)
        {
            onClose(stream, errorCode);
        }
    }
};


TEST(BufferedByteStream, Basic)
{
    DummyUnbufferedByteStream unbufferedByteStream{"Hello World!"};
    BufferedByteStream bufferedByteStream{unbufferedByteStream};

    std::vector<char> rd;
    rd.resize(12);

    std::string wr{"The weather is great!"};

    bool readDone{false};
    bool writeDone{false};

    BufferedByteStreamCallbacks callbacks;
    callbacks.onReadDone = [&rd, &readDone](SilKitTransport::IBufferedByteStream&) {
        ASSERT_EQ((std::string{rd.begin(), rd.end()}), "Hello World!");
        readDone = true;
    };
    callbacks.onWriteDone = [&writeDone, &unbufferedByteStream, &wr](SilKitTransport::IBufferedByteStream&) {
        writeDone = true;
        const auto written = unbufferedByteStream.GetWritten();
        ASSERT_EQ((std::string{static_cast<const char*>(written.GetData()), written.GetSize()}), wr);
    };

    bufferedByteStream.SetUp(callbacks);
    bufferedByteStream.Read(MutableBuffer{rd.data(), rd.size()});
    bufferedByteStream.Write(ConstBuffer{wr.data(), wr.size()});

    ASSERT_TRUE(readDone);
    ASSERT_TRUE(writeDone);
}


} // namespace
