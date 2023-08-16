#include "bufferedbytestream.hpp"
#include "messagestream.hpp"

#include "silkittransport.hpp"
#include "iiocontext.hpp"
#include "iconnector.hpp"
#include "iacceptor.hpp"

#include <algorithm>
#include <deque>
#include <functional>
#include <iostream>
#include <iterator>
#include <memory>
#include <vector>

#include <cstring>


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


class Server
    : private IAcceptorListener
    , private IConnectionListener
{
    IIoContext* _ioContext{nullptr};
    std::vector<std::unique_ptr<IAcceptor>> _acceptors;
    std::vector<std::unique_ptr<IConnection>> _connections;

public:
    explicit Server(IIoContext& ioContext)
        : _ioContext{&ioContext}
    {
    }

    void AcceptTcp(const std::string& address, uint16_t port)
    {
        auto acceptor{_ioContext->MakeTcpAcceptor(address, port)};
        acceptor->SetListener(*this);

        auto& acceptorRef = *acceptor;
        _acceptors.emplace_back(std::move(acceptor));
        acceptorRef.Accept();
    }

private: // IAcceptorListener
    void OnAccept(IAcceptor& acceptor, std::unique_ptr<IUnbufferedByteStream> stream) override
    {
        LogThis() << "accepting" << std::endl;

        auto connection = std::make_unique<Connection>(Connection{*this, std::move(stream)});
        connection->Start();
        _connections.emplace_back(std::move(connection));

        acceptor.Accept();
    }

    void OnClose(IAcceptor& acceptor, std::error_code errorCode) override
    {
        LogThis() << "acceptor closed" << std::endl;
        if (errorCode)
        {
            LogThis() << "i/o error: acceptor failed: " << errorCode.message() << std::endl;
        }

        auto it = std::find_if(_acceptors.begin(), _acceptors.end(), [needle = &acceptor](const auto& hay) -> bool {
            return hay.get() == needle;
        });

        if (it != _acceptors.end())
        {
            LogThis() << "acceptor removed" << std::endl;
            _acceptors.erase(it);
        }
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


class Client
    : private IConnectorListener
    , private IConnectionListener
{
    IIoContext* _ioContext{nullptr};

    std::unique_ptr<IConnector> _connector;
    std::unique_ptr<IConnection> _connection;

    IClientListener* _listener{nullptr};

public:
    Client(IIoContext& ioContext, const std::string& address, uint16_t port, IClientListener& listener)
        : _ioContext{&ioContext}
        , _connector{_ioContext->MakeTcpConnector()}
        , _listener{&listener}
    {
        _connector->SetListener(*this);
        _connector->Connect(address, port);
    }

    void Write(std::vector<char> message)
    {
        _connection->Write(std::move(message));
    }

private: // IConnectorListener
    void OnConnect(IConnector& connector, std::unique_ptr<IUnbufferedByteStream> stream) override
    {
        _connection = std::make_unique<Connection>(Connection{*this, std::move(stream)});
        _connection->Start();

        _listener->OnConnected(*this);
    }

    void OnClose(IConnector& connector, std::error_code const& errorCode) override
    {
        if (errorCode)
        {
            LogThis() << "i/o error: connect failed: " << errorCode.message() << std::endl;
            return;
        }

        LogThis() << "connector closed" << std::endl;
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
        std::cout << "usage: " << prog << " (client|server) <address> <port>" << std::endl;
        return EXIT_FAILURE;
    }

    const std::string mode{args[0]};
    const std::string address{args[1]};
    const std::string portString{args[2]};

    const auto port{static_cast<uint16_t>(std::strtol(portString.data(), nullptr, 10))};

    auto ioContext{MakeIoContext()};

    std::unique_ptr<Client> client;
    std::unique_ptr<Server> server;

    ClientCallbacks clientCallbacks;
    clientCallbacks.onConnected = [](Client& client) {
        client.Write(MakeMessage("Hello World 1!"));
        client.Write(MakeMessage("Hello World 2!"));
    };

    if (mode == "client")
    {
        client = std::make_unique<Client>(*ioContext, address, port, clientCallbacks);
    }
    else if (mode == "server")
    {
        server = std::make_unique<Server>(*ioContext);
        server->AcceptTcp(address, port);
    }
    else
    {
        std::cout << "invalid mode: " << mode << std::endl;
        return EXIT_FAILURE;
    }

    ioContext->Run();

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
