#include "silkittransport.hpp"

#include "asioiocontext.hpp"


namespace SilKitTransport {


auto MakeIoContext() -> std::unique_ptr<IIoContext>
{
    return std::make_unique<AsioIoContext>();
}


} // namespace SilKitTransport
