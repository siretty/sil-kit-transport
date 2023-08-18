#include "asiogenericunbufferedbytestream.hpp"
#include "bufferedbytestream.hpp"

#include "asio.hpp"


int main()
{
    asio::io_context ioContext;

    const auto endpoint = asio::ip::tcp::endpoint{asio::ip::address::from_string("0.0.0.0"), 0};


    SilKitTransport::AsioGenericUnbufferedByteStream asioGenericUnbufferedByteStream{};
    SilKitTransport::BufferedByteStream bufferedByteStream{};

    ioContext.run();

    return 0;
}

/*

#include "ibytestream.hpp"
#include "asiogenericbytestream.hpp"
#include "log.hpp"

#include <iostream>
#include <memory>
#include <vector>

#include "asio.hpp"


namespace {


using namespace SilKitTransport;


class Server : IByteStream::IByteStreamListener
{
    asio::ip::tcp::acceptor _acceptor;
    std::vector<std::unique_ptr<IByteStream>> _byteStreams;

public:
    Server(const asio::any_io_executor& ioExecutor, const asio::ip::tcp::endpoint& endpoint)
        : _acceptor{ioExecutor, endpoint}
    {
        LogThis() << "accepting on " << _acceptor.local_endpoint() << std::endl;
        Accept();
    }

private:
    void Accept()
    {
        _acceptor.async_accept([this](const auto& ec, auto s) {
            OnAccept(ec, std::move(s));
        });
    }

    void OnAccept(const asio::error_code& errorCode, asio::ip::tcp::socket socket)
    {
        if (errorCode)
        {
            std::cerr << "Server: I/O Error: accept failed: " << errorCode.message() << std::endl;
            return;
        }

        LogThis() << "accepting " << socket.remote_endpoint() << " on " << socket.local_endpoint() << std::endl;

        auto byteStream = std::make_unique<AsioGenericByteStream>(std::move(socket));
        byteStream->Start(*this);
        _byteStreams.emplace_back(std::move(byteStream));

        Accept();
    }

private: // IByteStream::IByteStreamListener
    void OnByteStreamSomeDataReceived(IByteStream& byteStream, void const* data, size_t size) override
    {
        Log(byteStream) << "rx " << size << std::endl;
        for (const auto& otherByteStream : _byteStreams)
        {
            otherByteStream->SendSome(data, size);
        }
    }

    void OnByteStreamSomeDataSent(IByteStream& byteStream, size_t bytesTransferred) override
    {
        Log(byteStream) << "tx " << bytesTransferred << std::endl;
    }

    void OnByteStreamClosed(IByteStream& byteStream, std::error_code const& errorCode) override
    {
        Log(byteStream) << "closed: " << errorCode.message() << std::endl;
    }

private:
    auto LogThis() -> std::ostream& { return std::cout << "Server (" << static_cast<const void*>(this) << "): "; }
};


} // namespace


int main()
{
    asio::io_context ioContext;

    const auto endpoint = asio::ip::tcp::endpoint{asio::ip::address::from_string("0.0.0.0"), 0};
    Server server{ioContext.get_executor(), endpoint};

    ioContext.run();

    return 0;
}

 */