#ifndef SILKITTRANSPORT_SRC_BUFFERSEQUENCE_HPP_09107D52E19D46D8AF16AEAF4BF45D3D
#define SILKITTRANSPORT_SRC_BUFFERSEQUENCE_HPP_09107D52E19D46D8AF16AEAF4BF45D3D


#include <atomic>
#include <memory>
#include <type_traits>
#include <vector>


namespace SilKitTransport {


template <typename Void>
class Buffer
{
    Void* _data{nullptr};
    size_t _size{0};

public:
    Buffer() = default;

    Buffer(Void* data, size_t size)
        : _data{data}
        , _size{size}
    {
    }

    auto GetData() const -> Void* { return _data; }

    auto GetSize() const -> size_t { return _size; }

    auto SliceOff(size_t size) -> Buffer
    {
        if (_size >= size)
        {
            Buffer prefix{_data, size};
            _data = reinterpret_cast<Void*>(reinterpret_cast<Byte*>(_data) + size);
            _size = _size - size;
            return prefix;
        }
        else
        {
            Buffer prefix{*this};
            _data = nullptr;
            _size = 0;
            return prefix;
        }
    }

private:
    static_assert(std::is_same<std::remove_cv_t<Void>, void>::value, "");

    template <typename T>
    struct ByteTypeFor
    {
        using Type = unsigned char;
    };

    template <typename T>
    struct ByteTypeFor<const T>
    {
        using Type = const unsigned char;
    };

    using Byte = typename ByteTypeFor<Void>::Type;
};


using MutableBuffer = Buffer<void>;
using ConstBuffer = Buffer<const void>;


template <typename Void>
class BufferSequence
{
public:
    BufferSequence() = default;

    void AppendBuffer(Buffer<Void> buffer)
    {

    }
};

using MutableBufferSequence = BufferSequence<void>;
using ConstBufferSequence = BufferSequence<const void>;


} // namespace SilKitTransport


#endif //SILKITTRANSPORT_SRC_BUFFERSEQUENCE_HPP_09107D52E19D46D8AF16AEAF4BF45D3D
