#ifndef SILKITTRANSPORT_SRC_BUFFERSEQUENCE_HPP_09107D52E19D46D8AF16AEAF4BF45D3D
#define SILKITTRANSPORT_SRC_BUFFERSEQUENCE_HPP_09107D52E19D46D8AF16AEAF4BF45D3D


#include <atomic>
#include <memory>
#include <type_traits>
#include <vector>
#include <list>

#include <cassert>


namespace SilKitTransport {

template <typename Void>
class Buffer;


template <typename Void>
struct IBufferSequence
{
    ~IBufferSequence() = default;

    virtual auto GetSequenceItem(size_t index) const -> Buffer<Void> = 0;

    virtual auto GetSequenceSize() const -> size_t = 0;
};


using IMutableBufferSequence = IBufferSequence<void>;
using IConstBufferSequence = IBufferSequence<const void>;


template <typename Void>
class Buffer : public IBufferSequence<Void>
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

public: // IBufferSequence<Void>
    auto GetSequenceItem(size_t index) const -> Buffer<Void> override
    {
        (void)(index);
        return *this;
    }

    auto GetSequenceSize() const -> size_t override { return 1; }

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
class BufferSpan : public IBufferSequence<Void>
{
    Buffer<Void>* _first{nullptr};
    size_t _size{0};

public:
    BufferSpan() = default;

    BufferSpan(Buffer<Void>* first, size_t size)
        : _first{first}
        , _size{size}
    {
    }

    auto GetSequenceItem(size_t index) const -> Buffer<Void> override { return *(_first + index); }

    auto GetSequenceSize() const -> size_t override { return _size; }
};

using MutableBufferSpan = BufferSpan<void>;
using ConstBufferSpan = BufferSpan<const void>;


} // namespace SilKitTransport


#endif //SILKITTRANSPORT_SRC_BUFFERSEQUENCE_HPP_09107D52E19D46D8AF16AEAF4BF45D3D
