#include "messagestream.hpp"


namespace SilKitTransport {


UnbufferedMessageStream::UnbufferedMessageStream(std::unique_ptr<IUnbufferedByteStream> stream)
    : _stream{std::move(stream)}
{
    _stream->SetUp(*this);
}


void UnbufferedMessageStream::SetUp(IUnbufferedMessageStreamListener& listener)
{
    _listener = &listener;
}


void UnbufferedMessageStream::ReadMessage()
{
}

void UnbufferedMessageStream::WriteMessage(std::vector<char> message)
{
}


void UnbufferedMessageStream::Close()
{
    _stream->Close();
}


void UnbufferedMessageStream::OnReadDone(IUnbufferedByteStream& stream, size_t bytesTransferred)
{
}


void UnbufferedMessageStream::OnWriteDone(IUnbufferedByteStream& stream, size_t bytesTransferred)
{
}


void UnbufferedMessageStream::OnClose(IUnbufferedByteStream& stream, const std::error_code& errorCode)
{
    _listener->OnClose(*this, errorCode);
}


} // namespace SilKitTransport
