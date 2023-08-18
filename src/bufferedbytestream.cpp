#include "bufferedbytestream.hpp"

#include "exceptions.hpp"

#include <cassert>
#include <cstring>


namespace SilKitTransport {


BufferedByteStream::BufferedByteStream(IUnbufferedByteStream& stream)
    : _stream{&stream}
{
    _stream->SetUp(*this);
}


void BufferedByteStream::SetUp(IBufferedByteStreamListener& listener)
{
    _listener = &listener;
}


void BufferedByteStream::Read(const IMutableBufferSequence& bufferSequence)
{
    if (_reading)
    {
        throw ByteStreamAlreadyReadingError{};
    }

    _reading = true;

    size_t total{0};
    _readBufferSequence.resize(bufferSequence.GetSequenceSize());
    for (size_t index = 0; index != _readBufferSequence.size(); ++index)
    {
        _readBufferSequence[index] = bufferSequence.GetSequenceItem(index);
        total += _readBufferSequence[index].GetSize();
    }

    if (total == 0)
    {
        _reading = false;
        _listener->OnReadDone(*this);
    }

    _readBufferData.resize(total);
    _readBuffer = MutableBuffer{_readBufferData.data(), _readBufferData.size()};

    _stream->ReadSome(_readBuffer);
}


void BufferedByteStream::Write(const IConstBufferSequence& bufferSequence)
{
    if (_writing)
    {
        throw ByteStreamAlreadyWritingError{};
    }

    _writing = true;

    size_t total{0};
    for (size_t index = 0; index != bufferSequence.GetSequenceSize(); ++index)
    {
        total += bufferSequence.GetSequenceItem(index).GetSize();
    }

    _writeBufferData.reserve(total);
    for (size_t index = 0; index != bufferSequence.GetSequenceSize(); ++index)
    {
        const auto buffer = bufferSequence.GetSequenceItem(index);
        const auto first = static_cast<const char*>(buffer.GetData());
        const auto last = first + buffer.GetSize();
        _writeBufferData.insert(_writeBufferData.end(), first, last);
    }

    _writeBuffer = ConstBuffer{_writeBufferData.data(), _writeBufferData.size()};

    _stream->WriteSome(_writeBuffer);
}


void BufferedByteStream::Close()
{
    _stream->Close();
}


void BufferedByteStream::OnReadDone(IUnbufferedByteStream& stream, size_t bytesTransferred)
{
    (void)(stream);

    _readBuffer.SliceOff(bytesTransferred);

    if (_readBuffer.GetSize() == 0)
    {
        _readBuffer = MutableBuffer{_readBufferData.data(), _readBufferData.size()};
        for (auto& buffer : _readBufferSequence)
        {
            std::memcpy(buffer.GetData(), _readBuffer.GetData(), buffer.GetSize());
            _readBuffer.SliceOff(buffer.GetSize());
        }

        _reading = false;
        _listener->OnReadDone(*this);
        return;
    }

    _stream->ReadSome(_readBuffer);
}


void BufferedByteStream::OnWriteDone(IUnbufferedByteStream& stream, size_t bytesTransferred)
{
    (void)(stream);

    _writeBuffer.SliceOff(bytesTransferred);

    if (_writeBuffer.GetSize() == 0)
    {
        _writing = false;
        _listener->OnWriteDone(*this);
        return;
    }

    _stream->WriteSome(_writeBuffer);
}


void BufferedByteStream::OnClose(IUnbufferedByteStream& stream, const std::error_code& errorCode)
{
    (void)(stream);
    _listener->OnClose(*this, errorCode);
}


} // namespace SilKitTransport
