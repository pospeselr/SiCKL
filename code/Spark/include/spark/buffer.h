#pragma once

template<typename T>
struct get_typename;

namespace spark
{
    namespace client
    {
        template<typename T>
        struct buffer1d
        {
            // extern constructor
            buffer1d(std::nullptr_t)
            : _data(nullptr)
            , Size(nullptr)
            {

            }

            // move constructor
            buffer1d(buffer1d&& other)
            : _data(std::move(other._data))
            , Size(std::move(const_cast<Int&>(other.Size)))
            {

            }

            lvalue<T> operator[](const rvalue<Int>& index)
            {
                return _data[index];
            }

            rvalue<T> operator[](const rvalue<Int>& index) const
            {
                return _data[index];
            }

            rvalue<Pointer<T>> Data()
            {
                return rvalue<Pointer<T>>(_data._node);
            }

            const Int Size;
        private:
            Pointer<T> _data;
        };
    }
    template<typename T>
    using Buffer1D = client::buffer1d<T>;
}