#include "messagestream.hpp"

#include "byteorder.hpp"

#include <iterator>


namespace SilKitTransport {


MessageStream::MessageStream(std::unique_ptr<IByteStream> byteStream)
    : _byteStream{std::move(byteStream)}
{
}


// IMessageStream


void MessageStream::Start(IMessageStream::IMessageStreamListener &listener)
{
    _byteStream->Start(*this);
}


void MessageStream::Send(std::vector<unsigned char> message)
{
}


void MessageStream::Close()
{
    _byteStream->Close();
}


// IByteStream::IByteStreamListener


void MessageStream::OnByteStreamSomeDataReceived(SilKitTransport::IByteStream &byteStream, void const *data,
                                                 size_t size)
{
}


void MessageStream::OnByteStreamSomeDataSent(SilKitTransport::IByteStream &byteStream, size_t bytesTransferred)
{
}


void MessageStream::OnByteStreamClosed(SilKitTransport::IByteStream &byteStream, std::error_code const &errorCode)
{
}


// MessageStream


void MessageStream::DoSend()
{
}


} // namespace SilKitTransport
