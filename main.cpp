#include "asiogenericunbufferedbytestream.hpp"
#include "bufferedbytestream.hpp"
#include "messagestream.hpp"

#include <algorithm>
#include <deque>
#include <iostream>
#include <iterator>
#include <memory>
#include <vector>

#include "asio.hpp"


namespace {


using namespace SilKitTransport;


class MessageWriter
{
    const std::vector<char>* _message;

public:
    explicit MessageWriter(const std::vector<char>& message)
        : _message{&message}
    {
    }

    friend auto operator<<(std::ostream& ostream, const MessageWriter& messageWriter) -> std::ostream&
    {
        std::copy(messageWriter._message->begin(), messageWriter._message->end(), std::ostream_iterator<char>(ostream));
        return ostream;
    }
};


struct IConnectionListener;


struct IConnection
{
    virtual ~IConnection() = default;

    virtual void Start() = 0;

    virtual void Write(std::vector<char> message) = 0;

    virtual void Close() = 0;
};


struct IConnectionListener
{
    virtual ~IConnectionListener() = default;

    virtual void OnMessage(IConnection& connection, std::vector<char> message) = 0;

    virtual void OnClose(IConnection& connection) = 0;
};


class Connection
    : public IConnection
    , private IMessageStreamListener
{
    IConnectionListener* _listener{nullptr};
    std::unique_ptr<IUnbufferedByteStream> _unbufferedByteStream;
    std::unique_ptr<BufferedByteStream> _bufferedByteStream;
    std::unique_ptr<MessageStream> _messageStream;

    bool _writing{false};
    std::deque<std::vector<char>> _messageQueue;

public:
    Connection(IConnectionListener& listener, std::unique_ptr<IUnbufferedByteStream> unbufferedByteStream)
        : _listener{&listener}
        , _unbufferedByteStream{std::move(unbufferedByteStream)}
        , _bufferedByteStream{std::make_unique<BufferedByteStream>(*_unbufferedByteStream)}
        , _messageStream{std::make_unique<MessageStream>(*_bufferedByteStream)}
    {
    }

    void Start() override
    {
        LogThis() << "starting" << std::endl;
        _messageStream->SetListener(*this);
        _messageStream->ReadMessage();
        LogThis() << "starting done" << std::endl;
    }

    void Write(std::vector<char> message) override
    {
        _messageQueue.emplace_back(std::move(message));

        if (!_writing)
        {
            _writing = true;

            auto messageToWrite = std::move(_messageQueue.front());
            _messageQueue.pop_front();
            _messageStream->WriteMessage(std::move(messageToWrite));
        }
    }

    void Close() override
    {
        _messageStream->Close();
    }

private:
    void OnReadDone(IMessageStream&, std::vector<char> message) override
    {
        LogThis() << "on read done (" << message.size() << "): " << MessageWriter{message} << std::endl;

        _listener->OnMessage(*this, message);
        _messageStream->ReadMessage();
    }

    void OnWriteDone(IMessageStream&) override
    {
        LogThis() << "on write done" << std::endl;

        if (_messageQueue.empty())
        {
            _writing = false;
            return;
        }
        else
        {
            auto messageToWrite = std::move(_messageQueue.front());
            _messageQueue.pop_front();
            _messageStream->WriteMessage(std::move(messageToWrite));
        }
    }

    void OnClose(IMessageStream& stream, std::error_code const& errorCode) override
    {
        LogThis() << "on close" << std::endl;
    }

private:
    auto LogThis() -> std::ostream&
    {
        return std::cout << "Connection (" << static_cast<const void*>(this) << "): ";
    }
};


class Server : private IConnectionListener
{
    asio::ip::tcp::acceptor _acceptor;
    std::vector<std::unique_ptr<IConnection>> _connections;

public:
    Server(const asio::any_io_executor& ioExecutor, const asio::ip::tcp::endpoint& endpoint)
        : _acceptor{ioExecutor, endpoint}
    {
        LogThis() << "accepting on " << _acceptor.local_endpoint() << std::endl;
        Accept();
    }

private:
    void OnMessage(IConnection& from, std::vector<char> message) override
    {
        LogThis() << "message (" << message.size() << "): " << MessageWriter{message} << std::endl;

        for (const auto& connection : _connections)
        {
            if (connection.get() == &from)
            {
                continue;
            }

            connection->Write(message);
        }
    }

    void OnClose(IConnection& connection) override
    {
        LogThis() << "connection closed" << std::endl;

        auto it =
            std::find_if(_connections.begin(), _connections.end(), [needle = &connection](const auto& hay) -> bool {
                return hay.get() == needle;
            });

        if (it != _connections.end())
        {
            LogThis() << "connection removed" << std::endl;
            _connections.erase(it);
        }
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
            LogThis() << "i/o error: accept failed: " << errorCode.message() << std::endl;
            return;
        }

        LogThis() << "accepting " << socket.remote_endpoint() << " on " << socket.local_endpoint() << std::endl;

        auto unbufferedByteStream = std::make_unique<AsioGenericUnbufferedByteStream>(std::move(socket));
        auto connection = std::make_unique<Connection>(Connection{*this, std::move(unbufferedByteStream)});
        connection->Start();
        _connections.emplace_back(std::move(connection));

        Accept();
    }

private:
    auto LogThis() -> std::ostream&
    {
        return std::cout << "Server (" << static_cast<const void*>(this) << "): ";
    }
};


class Client;


struct IClientListener
{
    virtual ~IClientListener() = default;

    virtual void OnConnected(Client& client) = 0;
};


class Client : private IConnectionListener
{
    asio::ip::tcp::socket _socket;
    std::unique_ptr<IConnection> _connection;

    IClientListener* _listener{nullptr};

public:
    Client(const asio::any_io_executor& ioExecutor, const asio::ip::tcp::endpoint& endpoint, IClientListener& listener)
        : _socket{ioExecutor}
        , _listener{&listener}
    {
        Connect(endpoint);
    }

    void Write(std::vector<char> message)
    {
        _connection->Write(std::move(message));
    }

private:
    void OnMessage(IConnection& from, std::vector<char> message) override
    {
        auto& ostream = (LogThis() << "Message (" << message.size() << "): ");
        std::copy(message.begin(), message.end(), std::ostream_iterator<char>(ostream));
        ostream << std::endl;
    }

    void OnClose(IConnection& connection) override
    {
        LogThis() << "connection closed" << std::endl;
    }

private:
    void Connect(const asio::ip::tcp::endpoint& endpoint)
    {
        _socket.async_connect(endpoint, [this](const asio::error_code& errorCode) {
            OnConnect(errorCode);
        });
    }

    void OnConnect(const asio::error_code& errorCode)
    {
        if (errorCode)
        {
            LogThis() << "i/o error: connect failed: " << errorCode.message() << std::endl;
            return;
        }

        LogThis() << "connected " << _socket.remote_endpoint() << " on " << _socket.local_endpoint() << std::endl;

        auto unbufferedByteStream = std::make_unique<AsioGenericUnbufferedByteStream>(std::move(_socket));
        _connection = std::make_unique<Connection>(Connection{*this, std::move(unbufferedByteStream)});
        _connection->Start();

        _listener->OnConnected(*this);
    }

private:
    auto LogThis() -> std::ostream&
    {
        return std::cout << "Client (" << static_cast<const void*>(this) << "): ";
    }
};


struct ClientCallbacks : public IClientListener
{
    std::function<void(Client&)> onConnected;

    void OnConnected(Client& client) override
    {
        if (onConnected)
        {
            onConnected(client);
        }
    }
};


auto MakeMessage(const char* message) -> std::vector<char>
{
    return std::vector<char>{message, message + std::strlen(message)};
}


int Main(std::string prog, std::vector<std::string> args)
{
    if (args.size() != 3)
    {
        std::cout << "usage: " << prog << " (client|server) <host> <port>" << std::endl;
        return EXIT_FAILURE;
    }

    const std::string mode{args[0]};
    const std::string host{args[1]};
    const std::string portString{args[2]};

    const auto port{static_cast<uint16_t>(std::strtol(portString.data(), nullptr, 10))};

    asio::io_context ioContext;

    const auto endpoint = asio::ip::tcp::endpoint{asio::ip::address::from_string(host), port};

    std::unique_ptr<Client> client;
    std::unique_ptr<Server> server;

    ClientCallbacks clientCallbacks;
    clientCallbacks.onConnected = [](Client& client) {
        client.Write(MakeMessage("Hello World 1!"));
        client.Write(MakeMessage("Hello World 2!"));
    };

    if (mode == "client")
    {
        client = std::make_unique<Client>(ioContext.get_executor(), endpoint, clientCallbacks);
    }
    else if (mode == "server")
    {
        server = std::make_unique<Server>(ioContext.get_executor(), endpoint);
    }
    else
    {
        std::cout << "invalid mode: " << mode << std::endl;
        return EXIT_FAILURE;
    }

    ioContext.run();

    return EXIT_SUCCESS;
}


} // namespace


int main(int argc, char** argv)
{
    std::string prog{"<unknown-program>"};
    std::vector<std::string> args;

    if (argc != 0)
    {
        prog = std::string{argv[0]};
    }

    for (int argi = 1; argi != argc; ++argi)
    {
        args.emplace_back(argv[argi]);
    }

#ifdef MAIN_CATCH_EXCEPTIONS
    try
    {
        return Main(std::move(prog), std::move(args));
    }
    catch (const std::exception& exception)
    {
        std::cerr << "exception: " << exception.what() << std::endl;
    }
    catch (...)
    {
        std::cerr << "exception of unknown type" << std::endl;
    }

    return EXIT_FAILURE;
#else
    return Main(std::move(prog), std::move(args));
#endif
}
