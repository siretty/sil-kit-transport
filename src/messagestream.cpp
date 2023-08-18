#include "messagestream.hpp"

#include "byteorder.hpp"
#include "exceptions.hpp"


namespace SilKitTransport {


MessageStream::MessageStream(IBufferedByteStream& stream)
    : _stream{&stream}
{
    _stream->SetUp(*this);
}


void MessageStream::SetUp(IMessageStreamListener& listener)
{
    _listener = &listener;
}


void MessageStream::ReadMessage()
{
    if (_readState != READ_IDLE)
    {
        throw StreamAlreadyReadingError{};
    }

    _readState = READ_SIZE;

    _stream->Read(MutableBuffer{_readSize.data(), _readSize.size()});
}

void MessageStream::WriteMessage(std::vector<char> message)
{
    if (_writeState != WRITE_IDLE)
    {
        throw StreamAlreadyWritingError{};
    }

    _writeState = WRITE_PENDING;

    const auto length{static_cast<uint32_t>(message.size())};

    LittleEndian::PutU32(_writeSize.data(), length);
    _writeData = std::move(message);

    std::array<ConstBuffer, 2> bufferSequence{
        ConstBuffer{_writeSize.data(), _writeSize.size()},
        ConstBuffer{_writeData.data(), _writeData.size()},
    };

    _stream->Write(ConstBufferSpan{bufferSequence.data(), bufferSequence.size()});
}


void MessageStream::Close()
{
    _stream->Close();
}


void MessageStream::OnReadDone(IBufferedByteStream& stream)
{
    switch (_readState)
    {
    case READ_SIZE:
        _readState = READ_DATA;
        _readData.resize(LittleEndian::GetU32(_readSize.data()));
        _stream->Read(MutableBuffer{_readData.data(), _readData.size()});
        break;
    case READ_DATA:
        _readState = READ_IDLE;
        _listener->OnReadDone(*this, std::move(_readData));
        break;
    default:
        throw "todo";
    }
}


void MessageStream::OnWriteDone(IBufferedByteStream& stream)
{
    _writeState = WRITE_IDLE;
    _listener->OnWriteDone(*this);
}


void MessageStream::OnClose(IBufferedByteStream& stream, const std::error_code& errorCode)
{
    _listener->OnClose(*this, errorCode);
}


} // namespace SilKitTransport
