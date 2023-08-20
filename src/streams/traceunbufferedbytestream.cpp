#include "traceunbufferedbytestream.hpp"

#include <iostream>


namespace SilKitTransport {


TraceUnbufferedByteStream::TraceUnbufferedByteStream(IUnbufferedByteStream& unbufferedByteStream)
    : _stream{&unbufferedByteStream}
{
    _stream->SetListener(*this);
}


void TraceUnbufferedByteStream::SetListener(IUnbufferedByteStreamListener& listener)
{
    std::cout << "SetListener(" << static_cast<void*>(&listener) << ")" << std::endl;
    _listener = &listener;
}


void TraceUnbufferedByteStream::ClearListener()
{
    std::cout << "ClearListener()" << std::endl;
    _listener = nullptr;
}


void TraceUnbufferedByteStream::ReadSome(IMutableBufferSequence const& bufferSequence)
{
    std::cout << "ReadSome([";
    for (size_t index = 0; index != bufferSequence.GetSequenceSize(); ++index)
    {
        if (index != 0)
        {
            std::cout << ", ";
        }

        const auto buffer = bufferSequence.GetSequenceItem(index);
        std::cout << buffer.GetData() << " " << buffer.GetSize();
    }
    std::cout << "])" << std::endl;
    _stream->ReadSome(bufferSequence);
}


void TraceUnbufferedByteStream::WriteSome(IConstBufferSequence const& bufferSequence)
{
    std::cout << "WriteSome([";
    for (size_t index = 0; index != bufferSequence.GetSequenceSize(); ++index)
    {
        if (index != 0)
        {
            std::cout << ", ";
        }

        const auto buffer = bufferSequence.GetSequenceItem(index);
        std::cout << buffer.GetData() << " " << buffer.GetSize();
    }
    std::cout << "])" << std::endl;
    _stream->WriteSome(bufferSequence);
}


void TraceUnbufferedByteStream::Close()
{
    std::cout << "Close()" << std::endl;
    _stream->Close();
}


void TraceUnbufferedByteStream::OnReadDone(IUnbufferedByteStream& stream, size_t bytesTransferred)
{
    std::cout << "OnReadDone(" << static_cast<void*>(&stream) << ", " << bytesTransferred << ")" << std::endl;
    _listener->OnReadDone(*this, bytesTransferred);
}


void TraceUnbufferedByteStream::OnWriteDone(IUnbufferedByteStream& stream, size_t bytesTransferred)
{
    std::cout << "OnWriteDone(" << static_cast<void*>(&stream) << ", " << bytesTransferred << ")" << std::endl;
    _listener->OnWriteDone(*this, bytesTransferred);
}


void TraceUnbufferedByteStream::OnClose(IUnbufferedByteStream& stream, std::error_code const& errorCode)
{
    std::cout << "OnClose(" << static_cast<void*>(&stream) << ", " << errorCode.message() << ")" << std::endl;
    _listener->OnClose(*this, errorCode);
}


} // namespace SilKitTransport
