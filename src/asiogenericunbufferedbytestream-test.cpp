#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "asiogenericunbufferedbytestream.hpp"

#include <cstring>

#include "asio.hpp"


namespace {


using namespace SilKitTransport;


struct WriteAll : public IUnbufferedByteStreamListener
{
    std::vector<char> wr;
    size_t transferred{0};

    explicit WriteAll(std::vector<char> wr)
        : wr{std::move(wr)}
    {
    }

    explicit WriteAll(const char* text)
        : wr{text, text + std::strlen(text)}
    {
    }

    void StartWrite(IUnbufferedByteStream& stream)
    {
        ASSERT_LE(transferred, wr.size());

        if (transferred < wr.size())
        {
            stream.WriteSome(ConstBuffer{wr.data() + transferred, wr.size()});
        }
    }

    void OnWriteDone(IUnbufferedByteStream& stream, size_t bytesTransferred) override
    {
        transferred += bytesTransferred;
        ASSERT_LE(transferred, wr.size());

        if (transferred < wr.size())
        {
            StartWrite(stream);
        }
        else
        {
            stream.Close();
        }
    }

    void OnReadDone(IUnbufferedByteStream& stream, size_t bytesTransferred) override
    {
        (void)(stream), (void)(bytesTransferred);
        FAIL();
    }

    void OnClose(IUnbufferedByteStream& stream, const std::error_code& errorCode) override
    {
        (void)(stream);
        ASSERT_FALSE(errorCode);
        ASSERT_EQ(transferred, wr.size());
    }
};


template <size_t RdSizeIncrement = 3>
struct ReadAll : IUnbufferedByteStreamListener
{
    std::vector<char> rd;
    size_t transferred{0};

    void StartRead(IUnbufferedByteStream& stream)
    {
        ASSERT_LE(transferred, rd.size());

        if (rd.size() == 0)
        {
            rd.resize(RdSizeIncrement);
        }

        if (transferred < rd.size())
        {
            stream.ReadSome(MutableBuffer{rd.data() + transferred, rd.size() - transferred});
        }
    }

    void OnWriteDone(IUnbufferedByteStream& stream, size_t bytesTransferred) override
    {
        (void)(stream), (void)(bytesTransferred);
        FAIL();
    }

    void OnReadDone(IUnbufferedByteStream& stream, size_t bytesTransferred) override
    {
        transferred += bytesTransferred;
        ASSERT_LE(transferred, rd.size());

        if (transferred == rd.size())
        {
            rd.resize(rd.size() + RdSizeIncrement);
        }

        StartRead(stream);
    }

    void OnClose(IUnbufferedByteStream& stream, const std::error_code& errorCode) override
    {
        (void)(stream);
        EXPECT_TRUE(errorCode) << "when the writer closes";
        rd.resize(transferred);
    }
};


TEST(AsioGenericUnbufferedByteStream, Basic)
{
    WriteAll writeAll{"Hello World!"};
    ReadAll<> readAll;

    {
        asio::io_context ioContext;

        asio::local::stream_protocol::socket socketOne{ioContext};
        asio::local::stream_protocol::socket socketTwo{ioContext};

        asio::local::connect_pair(socketOne, socketTwo);

        AsioGenericUnbufferedByteStream streamOne{std::move(socketOne)};
        AsioGenericUnbufferedByteStream streamTwo{std::move(socketTwo)};

        streamOne.SetUp(writeAll);
        writeAll.StartWrite(streamOne);

        streamTwo.SetUp(readAll);
        readAll.StartRead(streamTwo);

        ioContext.run();
    }

    ASSERT_THAT(writeAll.wr, testing::ContainerEq(readAll.rd));
}


} // namespace
