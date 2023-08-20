#include "bufferedbytestream.hpp"

#include "exceptions.hpp"

#include <cassert>
#include <cstring>


namespace SilKitTransport {


BufferedByteStream::BufferedByteStream(IUnbufferedByteStream& stream)
    : _stream{&stream}
{
    _stream->SetListener(*this);
}


void BufferedByteStream::SetListener(IBufferedByteStreamListener& listener)
{
    _listener = &listener;
}


void BufferedByteStream::ClearListener()
{
    _listener = nullptr;
}


void BufferedByteStream::Read(const IMutableBufferSequence& bufferSequence)
{
    if (_reading)
    {
        throw StreamAlreadyReadingError{};
    }

    _reading = true;

    _readBufferSequence.resize(bufferSequence.GetSequenceSize());
    for (size_t index = 0; index != _readBufferSequence.size(); ++index)
    {
        _readBufferSequence[index] = bufferSequence.GetSequenceItem(index);
    }

    size_t remainingByteCount{FillReadBufferSequence()};

    if (remainingByteCount == 0)
    {
        _reading = false;
        _listener->OnReadDone(*this);
        return;
    }

    assert(_readBufferData.GetSize() == 0);

    InitiateReadSome(remainingByteCount);
}


void BufferedByteStream::Write(const IConstBufferSequence& bufferSequence)
{
    if (_writing)
    {
        throw StreamAlreadyWritingError{};
    }

    _writing = true;

    size_t total{0};
    for (size_t index = 0; index != bufferSequence.GetSequenceSize(); ++index)
    {
        total += bufferSequence.GetSequenceItem(index).GetSize();
    }

    _writeBufferData.clear();
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

    _readBufferData = _readBufferFree.SliceOff(bytesTransferred);

    size_t remainingByteCount{FillReadBufferSequence()};

    if (remainingByteCount == 0)
    {
        _reading = false;
        _listener->OnReadDone(*this);
        return;
    }

    assert(_readBufferData.GetSize() == 0);

    InitiateReadSome(remainingByteCount);
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

auto BufferedByteStream::FillReadBufferSequence() -> size_t
{
    size_t remaining{0};

    for (auto& buffer : _readBufferSequence)
    {
        size_t byteCount{std::min(_readBufferData.GetSize(), buffer.GetSize())};
        std::memcpy(buffer.GetData(), _readBufferData.GetData(), byteCount);

        buffer.SliceOff(byteCount);
        _readBufferData.SliceOff(byteCount);

        remaining += buffer.GetSize();
    }

    return remaining;
}

void BufferedByteStream::InitiateReadSome(size_t bufferSize)
{
    _readBufferStorage.reserve(4096);
    _readBufferStorage.resize(std::max<size_t>(bufferSize, _readBufferStorage.capacity()));

    _readBufferData = MutableBuffer{};
    _readBufferFree = MutableBuffer{_readBufferStorage.data(), _readBufferStorage.size()};

    _stream->ReadSome(_readBufferFree);
}


} // namespace SilKitTransport
